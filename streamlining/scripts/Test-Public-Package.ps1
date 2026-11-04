[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$PackageRoot
)

<##
Validates an exported Fire-24G V8.1 public package. It deliberately validates
the package directory, not this private engineering repository.
##>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path

$required = @(
    'README.md',
    'LICENSE',
    'docs/FIRE24G_V81_ENGINEERING_DOSSIER.md',
    'docs/FIRE24G_V81_BUILD_AND_INSTALL.md',
    'docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md',
    'docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md',
    'docs/PUBLIC_FIRE24G_RELEASES.md',
    'streamlining/PUBLIC_PACKAGE_MANIFEST.md',
    'streamlining/scripts/Generate-Fire24G-V81-Overlay.ps1',
    'streamlining/scripts/Test-Public-Package.ps1',
    'streamlining/public-template/control-compose.template.json',
    'streamlining/public-template/UB3-Control-Lower-Socket.template.json',
    'streamlining/public-template/UB3-Control-Upper-Socket.template.json',
    'streamlining/public-template/monitor-compose.template.json',
    'consolidation/scripts/Build-Control-V8.1-Clean.ps1',
    'consolidation/scripts/build-control-v0.8.1.sh',
    'firmware-monitor/Build-Monitor-Fire24G-V0.8.1.ps1',
    'firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh',
    'firmware-control/active-plugins/system/1541HUD-USB-v0.8.1',
    'firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1',
    'firmware-monitor/active-plugins/system/1541HUD-Monitor-USB',
    'firmware-monitor/active-plugins/user/1541HUD-Monitor',
    'firmware-monitor/shared/1541hud_mailbox.h',
    '1541hud/source/1541HUD_V0.8.0.51.py',
    '1541hud/tests/test_onerom_usb.py',
    'utilities/rom-pages/Split-1541-RomPages.ps1',
    'tools/onerom-v0.5.0.exe',
    'releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin',
    'releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin',
    'private-roms/README.md',
    'private-roms/UB3-Control-DolphinDOS-CAE.json',
    'private-roms/UB3-Control-Lower-Socket.json',
    'private-roms/UB3-Control-Upper-Socket.json',
    'private-roms/monitor-compose.json'
)

$missing = $required | Where-Object { -not (Test-Path -LiteralPath (Join-Path $PackageRoot $_)) }
if ($missing) { throw "Public package is incomplete:`n$($missing -join "`n")" }

$forbiddenDirectories = @('roms', 'archives', 'c64-onerom-configurator', 'outputs', 'work', 'build-kits')
foreach ($name in $forbiddenDirectories) {
    if (Test-Path -LiteralPath (Join-Path $PackageRoot $name)) {
        throw "Forbidden private/generated directory in public package: $name"
    }
}

$forbiddenPath = '(?i)(s-jiffy|sjiffydos|jiffydos|dolphindos|speeddos|\.bin$|\.uf2$|\.exe$|\.d64$|\.prg$)'
$badPaths = Get-ChildItem -LiteralPath $PackageRoot -Recurse -Force -File |
    Where-Object {
        $relative = $_.FullName.Substring($PackageRoot.Length + 1).Replace('\', '/')
        $relative -notmatch '^(private-roms/(UB3-Control-DolphinDOS-CAE|UB3-Control-Lower-Socket|UB3-Control-Upper-Socket|monitor-compose)\.json|releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0\.8\.1\.bin|releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0\.8\.1\.28\.bin|tools/onerom-v0\.5\.0\.exe)$' -and $relative -match $forbiddenPath
    }
if ($badPaths) {
    throw "Forbidden payload/history file(s) in public package:`n$($badPaths.FullName -join "`n")"
}

$textExtensions = @('.md', '.ps1', '.cmd', '.json', '.py', '.h', '.c', '.txt', '.yml', '.yaml')
# Upstream source and manuals legitimately contain example chip IDs and hashes.
# Only reject known private identifiers and local operator paths, not every
# sixteen-digit hexadecimal string in a third-party manual.
$forbiddenText = '(?i)(C:\\Users\\A[d]min\\|/Users/A[d]min/|8FAEB3DEC304ADCF|77920CCCA13D0FC9)'
$textHits = foreach ($file in Get-ChildItem -LiteralPath $PackageRoot -Recurse -Force -File) {
    $relative = $file.FullName.Substring($PackageRoot.Length + 1).Replace('\', '/')
    if ($relative -eq 'streamlining/scripts/Test-Public-Package.ps1') { continue }
    if ($textExtensions -notcontains $file.Extension) { continue }
    Select-String -LiteralPath $file.FullName -Pattern $forbiddenText | ForEach-Object {
        "$($file.FullName.Substring($PackageRoot.Length + 1)):$($_.LineNumber):$($_.Line.Trim())"
    }
}
if ($textHits) { throw "Private path or serial-like data in public package:`n$($textHits -join "`n")" }

Write-Host "PASS: $PackageRoot is structurally complete and contains no denied payload, history, serial, or local-path markers."
