[CmdletBinding()]
param(
    [string] $Toolchain = '',
    [string] $Make = '',
    [string] $OneRomCli = '',
    [string] $OutputPath = ''
)

$ErrorActionPreference = 'Stop'
$monitorRoot = $PSScriptRoot
$repoRoot = Split-Path -Parent $monitorRoot
$core = Join-Path $repoRoot 'source\one-rom-v0.8.1'
$systemPlugin = Join-Path $monitorRoot 'active-plugins\system\1541HUD-Monitor-USB'
$userPlugin = Join-Path $monitorRoot 'active-plugins\user\1541HUD-Monitor'
$tools = Join-Path $repoRoot 'tools'
$dependencyKit = Join-Path $monitorRoot 'build-kits\ub4-monitor-v08003-gcc14\third_party'

# Tool locations are explicit overrides.  Otherwise use the caller's PATH.
# Nothing in this script refers to an operator-specific Desktop or ChatGPT
# directory; a fresh checkout needs only the prerequisites in the runbook.
if (-not $Toolchain) {
    $gcc = Get-Command 'arm-none-eabi-gcc.exe' -ErrorAction SilentlyContinue
    if (-not $gcc) { throw 'ARM GNU Toolchain is not on PATH. Pass -Toolchain <bin-directory>.' }
    $Toolchain = Split-Path -Parent $gcc.Source
}
if (-not $Make) {
    $makeCommand = Get-Command 'make.exe' -ErrorAction SilentlyContinue
    if (-not $makeCommand) { throw 'GNU Make is not on PATH. Pass -Make <make.exe>.' }
    $Make = $makeCommand.Source
}
if (-not $OneRomCli) {
    $oneRomCommand = Get-Command 'onerom.exe' -ErrorAction SilentlyContinue
    if (-not $oneRomCommand) { throw 'OneROM CLI is not on PATH. Pass -OneRomCli <onerom.exe>.' }
    $OneRomCli = $oneRomCommand.Source
}
if (-not $OutputPath) {
    $OutputPath = Join-Path $monitorRoot 'releases\v8.1\candidates\onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin'
}

foreach ($required in @(
    (Join-Path $Toolchain 'arm-none-eabi-gcc.exe'),
    $Make,
    $OneRomCli,
    (Join-Path $core 'Makefile'),
    (Join-Path $monitorRoot 'COMPOSE_UB4_MONITOR_FIRE24G_V081.json')
)) {
    if (-not (Test-Path -LiteralPath $required)) { throw "Required build input not found: $required" }
}

# The upstream source intentionally does not vendor these dependencies, but the
# monitored build kit does, at the exact revisions its Makefiles pin.  Stage
# those local snapshots before Make can fall back to a live network clone.
$coreDependencies = Join-Path $core 'third_party'
foreach ($dependency in @('tinyusb', 'picobootx', 'pico-sdkless')) {
    $source = Join-Path $dependencyKit $dependency
    $destination = Join-Path $coreDependencies $dependency
    if (-not (Test-Path -LiteralPath $destination)) {
        if (-not (Test-Path -LiteralPath $source)) {
            throw "Pinned Monitor dependency is missing from the root source and local kit: $source"
        }
        New-Item -ItemType Directory -Force -Path $coreDependencies | Out-Null
        Copy-Item -LiteralPath $source -Destination $coreDependencies -Recurse -Force
    }
}

$temp = Join-Path $monitorRoot '.tmp-build-fire24g-v081'
New-Item -ItemType Directory -Force -Path $temp | Out-Null
$env:TEMP = $temp
$env:TMP = $temp
$env:TOOLCHAIN = ($Toolchain -replace '\\', '/')
$gitCommit = (& 'C:\Program Files\Git\cmd\git.exe' -C $core rev-parse HEAD).Substring(0, 7)
# The runbook's native build-tools bundle supplies a Windows-safe BusyBox
# shell for the upstream POSIX Makefiles (mkdir -p, date, find and pipes).
$env:SHELL = Join-Path (Split-Path -Parent $Make) 'sh.exe'
if (-not (Test-Path -LiteralPath $env:SHELL)) {
    $env:SHELL = Join-Path $tools 'sh.exe'
}
if (-not (Test-Path -LiteralPath $env:SHELL)) {
    $shellCommand = Get-Command 'sh.exe' -ErrorAction SilentlyContinue
    if (-not $shellCommand) { throw 'Required POSIX shell not found. Put sh.exe on PATH.' }
    $env:SHELL = $shellCommand.Source
}
$env:PATH = "$Toolchain;$(Split-Path -Parent $Make);$tools;$env:PATH"

function Invoke-Checked {
    param([scriptblock] $Command, [string] $Step)
    & $Command
    if ($LASTEXITCODE -ne 0) { throw "$Step failed with exit code $LASTEXITCODE." }
}

# HUD1541_PASSIVE_UB4 prevents the normal ROM-serving PIO path from starting.
# The Monitor user plugin claims and configures its capture resources itself.
# Generate metadata from the top-level source tree, then build the firmware
# Makefile directly.  Passing a firmware-prefixed BUILD_DIR through the
# top-level recursive make would resolve it a second time as firmware/firmware.
Invoke-Checked { & $Make -C $core generated } 'V8.1 metadata generation'
Invoke-Checked { & $Make -C (Join-Path $core 'firmware') -j2 `
    BUILD_DIR=build-ub4-fire24g-v081 `
    BIN_PREFIX=onerom-rp235x-passive-ub4-fire24g-v081 `
    VERSION_MAJOR=0 VERSION_MINOR=8 VERSION_PATCH=1 BUILD_NUMBER=1 GIT_COMMIT=$gitCommit `
    EXTRA_C_FLAGS=-DHUD1541_PASSIVE_UB4 } 'Passive V8.1 core build'

Invoke-Checked { & $Make -B -C $systemPlugin -j2 BUILD_DIR=build `
    RWX_SEGMENT_FLAG=-Wl,--no-warn-rwx-segments } 'Monitor USB plugin build'
Invoke-Checked { & $Make -B -C $userPlugin -j2 BUILD_DIR=build `
    RWX_SEGMENT_FLAG=-Wl,--no-warn-rwx-segments } 'Monitor capture plugin build'

New-Item -ItemType Directory -Force -Path (Split-Path -Parent $OutputPath) | Out-Null
Push-Location $monitorRoot
try {
    Invoke-Checked { & $OneRomCli firmware build `
        --config 'COMPOSE_UB4_MONITOR_FIRE24G_V081.json' `
        --board fire-24-g `
        --base-firmware (Join-Path $core 'firmware\build-ub4-fire24g-v081\onerom-rp235x-passive-ub4-fire24g-v081.bin') `
        --output $OutputPath `
        --force } 'Fire-24G image composition'
}
finally { Pop-Location }

$hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $OutputPath).Hash
Write-Host "Build complete: $OutputPath"
Write-Host "SHA256: $hash"
