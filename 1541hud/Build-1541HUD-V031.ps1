param(
    [string]$Repo = "",
    [string]$OneRomCli = "",
    [string]$Toolchain = "/usr/bin",
    [string]$Picotool = "/opt/picotool/build/picotool"
)

$ErrorActionPreference = "Stop"

function Get-WslPath {
    param([Parameter(Mandatory=$true)][string]$WindowsPath)

    $path = $WindowsPath -replace '^Microsoft\.PowerShell\.Core\\FileSystem::',''

    if ($path -match '^\\\\wsl(?:\$|\.localhost)\\[^\\]+\\(.*)$') {
        $rest = $Matches[1] -replace '\\','/'
        return "/" + $rest.TrimStart("/")
    }

    $full = [System.IO.Path]::GetFullPath($path)

    if ($full -match '^([A-Za-z]):[\\/](.*)$') {
        $drive = $Matches[1].ToLowerInvariant()
        $rest = $Matches[2] -replace '\\','/'
        return "/mnt/$drive/$rest"
    }

    throw "Could not convert Windows or WSL path to WSL path: $WindowsPath"
}

function Quote-Bash {
    param([Parameter(Mandatory=$true)][string]$Text)
    return "'" + ($Text -replace "'", "'""'""'") + "'"
}

Write-Host ""
Write-Host "1541HUD V0.0.31 - canonical source build"
Write-Host ""

if ([string]::IsNullOrWhiteSpace($Repo)) {
    $candidate = Split-Path -Parent $PSScriptRoot

    if ((Test-Path -LiteralPath (Join-Path $candidate "firmware\ora\plugin.mk")) -and
        (Test-Path -LiteralPath (Join-Path $candidate "plugins"))) {
        $Repo = (Resolve-Path -LiteralPath $candidate).Path
    }
}

if ([string]::IsNullOrWhiteSpace($Repo)) {
    throw "Could not locate OneROM repository. Use -Repo."
}

$Repo = (Resolve-Path -LiteralPath $Repo).Path

if ([string]::IsNullOrWhiteSpace($OneRomCli)) {
    if ($env:ONEROM_CLI -and (Test-Path -LiteralPath $env:ONEROM_CLI)) {
        $OneRomCli = (Resolve-Path -LiteralPath $env:ONEROM_CLI).Path
    } else {
        $cmd = Get-Command onerom.exe -ErrorAction SilentlyContinue
        if ($cmd) {
            $OneRomCli = $cmd.Source
        } else {
            $desktop = Join-Path $HOME "Desktop"
            if (Test-Path -LiteralPath $desktop) {
                $matches = Get-ChildItem -LiteralPath $desktop -Directory -Filter "onerom-cli-win-*" -ErrorAction SilentlyContinue |
                    Sort-Object Name -Descending
                foreach ($folder in $matches) {
                    $candidate = Join-Path $folder.FullName "onerom.exe"
                    if (Test-Path -LiteralPath $candidate) {
                        $OneRomCli = (Resolve-Path -LiteralPath $candidate).Path
                        break
                    }
                }
            }
        }
    }
}

if ([string]::IsNullOrWhiteSpace($OneRomCli)) {
    throw "Could not locate onerom.exe. Use -OneRomCli or ONEROM_CLI."
}

# Resolve-Path on a WSL UNC path may include the PowerShell provider prefix.
# Strip it once so .NET file APIs and child paths receive a normal filesystem path.
$Repo = $Repo -replace '^Microsoft\.PowerShell\.Core\\FileSystem::',''

$RepoWsl = Get-WslPath $Repo
$RepoQ = Quote-Bash $RepoWsl
$ToolchainQ = Quote-Bash $Toolchain

$BuildDir = Join-Path $Repo "build-1541hud"
$OutBase = Join-Path $BuildDir "1541HUD_OneROM_V0.0.31"
$Companion = Join-Path $BuildDir "1541hud_companion_8k.bin"

New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null

Write-Host "Creating passive 8K FF companion ROM..."
[byte[]]$bytes = New-Object byte[] 8192
for ($i = 0; $i -lt $bytes.Length; $i++) {
    $bytes[$i] = 0xFF
}
[System.IO.File]::WriteAllBytes($Companion, $bytes)

Write-Host "Building passive OneROM base firmware..."
$baseBuild = "cd $RepoQ && make firmware TOOLCHAIN=$ToolchainQ EXTRA_C_FLAGS=-DHUD1541_PASSIVE_UB4"
& wsl bash -lc $baseBuild
if ($LASTEXITCODE -ne 0) {
    throw "Passive OneROM base firmware build failed"
}

Write-Host "Building 1541HUD USER plugin..."
$userBuild = "cd $RepoQ && make -C plugins/user/1541hud-probe clean && make -C plugins/user/1541hud-probe TOOLCHAIN=$ToolchainQ"
& wsl bash -lc $userBuild
if ($LASTEXITCODE -ne 0) {
    throw "1541HUD USER plugin build failed"
}

Write-Host "Building USB SYSTEM plugin..."
$usbBuild = "cd $RepoQ && make -C plugins/system/usb clean && make -C plugins/system/usb TOOLCHAIN=$ToolchainQ"
& wsl bash -lc $usbBuild
if ($LASTEXITCODE -ne 0) {
    throw "USB SYSTEM plugin build failed"
}

$baseFirmware = Join-Path $Repo "firmware\build\onerom-rp235x.bin"
$userPlugin = Join-Path $Repo "plugins\user\1541hud-probe\build\plugin_user.bin"
$systemPlugin = Join-Path $Repo "plugins\system\usb\build\usb_system_plugin.bin"

foreach ($artifact in @($baseFirmware, $userPlugin, $systemPlugin)) {
    if (!(Test-Path -LiteralPath $artifact)) {
        throw "Required build artifact missing: $artifact"
    }
}

Write-Host "Composing Fire-24-E firmware..."
& $OneRomCli firmware build `
    --board fire-24-e `
    --base-firmware $baseFirmware `
    --slot "file=$Companion,type=2364,cs1=active-low" `
    --plugin "file=$systemPlugin" `
    --plugin "file=$userPlugin" `
    --out "$OutBase.bin"

if ($LASTEXITCODE -ne 0) {
    throw "OneROM firmware composition failed"
}

$OutBinWsl = Get-WslPath "$OutBase.bin"
$OutUf2Wsl = Get-WslPath "$OutBase.uf2"

Write-Host "Converting BIN to UF2..."
$uf2Command =
    (Quote-Bash $Picotool) + " uf2 convert " +
    (Quote-Bash $OutBinWsl) + " " +
    (Quote-Bash $OutUf2Wsl)

& wsl bash -lc $uf2Command
if ($LASTEXITCODE -ne 0) {
    throw "UF2 conversion failed"
}

Write-Host ""
Write-Host "Build complete:"
Get-Item "$OutBase.bin", "$OutBase.uf2" | Format-Table Name,Length,LastWriteTime

Write-Host "SHA256:"
Get-FileHash "$OutBase.bin" -Algorithm SHA256
Get-FileHash "$OutBase.uf2" -Algorithm SHA256
