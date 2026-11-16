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

function Get-Sha256Hex {
    param([Parameter(Mandatory)][string]$Path)
    $stream = [System.IO.File]::OpenRead($Path)
    try {
        $sha256 = [System.Security.Cryptography.SHA256]::Create()
        try {
            return ([System.BitConverter]::ToString($sha256.ComputeHash($stream))).Replace('-', '')
        }
        finally {
            $sha256.Dispose()
        }
    }
    finally {
        $stream.Dispose()
    }
}

function Get-LfTextSha256Hex {
    param([Parameter(Mandatory)][string]$Path)
    $text = [System.IO.File]::ReadAllText($Path, [System.Text.Encoding]::UTF8)
    $text = $text.Replace("`r`n", "`n").Replace("`r", "`n")
    $bytes = [System.Text.UTF8Encoding]::new($false).GetBytes($text)
    $sha256 = [System.Security.Cryptography.SHA256]::Create()
    try {
        return ([System.BitConverter]::ToString($sha256.ComputeHash($bytes))).Replace('-', '')
    }
    finally {
        $sha256.Dispose()
    }
}

$required = @(
    'README.md',
    '1541hud/README.md',
    '1541hud/requirements.txt',
    'tools/maintenance/Check-Documentation.py',
    'tools/maintenance/Verify-Source-Dependencies.py',
    'source/one-rom-v0.8.1/1541HUD-SOURCE.json',
    'LICENSE',
    'NOTICE.md',
    'CONTRIBUTING.md',
    'SECURITY.md',
    '.github/ISSUE_TEMPLATE/bug_report.md',
    'docs/FIRE24G_V81_ENGINEERING_DOSSIER.md',
    'docs/FIRE24G_V81_BUILD_AND_INSTALL.md',
    'docs/BUILD_UB3_CONTROL.md',
    'docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md',
    'docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md',
    'docs/PUBLIC_FIRE24G_RELEASES.md',
    'docs/STOCK_CONTROL_APPLY_ONLY_CANDIDATE_2026-11-10.md',
    'docs/PUBLIC_PACKAGE_MANIFEST.md',
    'roms/README.md',
    'roms/ASSETS.md',
    'roms/ASSETS.sha256',
    'roms/configs/README.md',
    'roms/configs/control-compose.template.json',
    'roms/configs/UB3-Control-Lower-Socket.template.json',
    'roms/configs/UB3-Control-Upper-Socket.template.json',
    'roms/configs/monitor-compose.template.json',
    'roms/stock-dos-2.6/README.md',
    'roms/ramboard/README.md',
    'roms/speeddos-plus/README.md',
    'roms/dolphindos-2/README.md',
    'roms/speeddos-plus/source-images/1541lower',
    'roms/speeddos-plus/source-images/1541upper_sd',
    'roms/speeddos-plus/source-images/1541both_sd',
    'roms/speeddos-plus/source-images/kernal_en_sd',
    'roms/jiffydos/README.md',
    'roms/s-jiffydos/README.md',
    'roms/dolphindos-2/source/1541.asm',
    'roms/dolphindos-2/source/kernal.asm',
    'roms/dolphindos-2/source/LICENSE',
    'roms/dolphindos-2/source/Makefile',
    'roms/dolphindos-2/source/README-upstream.md',
    'roms/s-jiffydos/source/one-rom-autostart/autostart.s',
    'roms/s-jiffydos/source/one-rom-autostart/autostart.cfg',
    'roms/s-jiffydos/source/one-rom-autostart/Makefile',
    'roms/s-jiffydos/source/one-rom-autostart/test_autostart.py',
    'docs/RELEASE_CHECKLIST.md',
    'docs/VALIDATION_MATRIX.md',
    'docs/DEPENDENCY_ALERT_TRIAGE.md',
    'tools/maintenance/Test-Public-Package.ps1',
    'tools/maintenance/Sanitize-Control-Diagnostics.ps1',
    'tools/maintenance/Apply-Host-Dependencies.sh',
    'tools/maintenance/Test-Host-Dependencies.sh',
    'tools/maintenance/Audit-Host-Dependencies.sh',
    'tools/maintenance/tests/host_dependency_zip.rs',
    'overlays/host-dependencies-v0.8.1.patch',
    'overlays/host-dependencies-v0.8.1.json',
    'consolidation/scripts/Build-Control-V8.1-Clean.ps1',
    'consolidation/scripts/build-control-v0.8.1.sh',
    'firmware-monitor/Build-Monitor-Fire24G-V0.8.1.ps1',
    'firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh',
    'firmware-monitor/COMPOSE_UB4_MONITOR_FIRE24G_V081.json',
    'firmware-monitor/inputs/1541-OneROM-Bootloader-Universal-v1.1.0.bin',
    'firmware-monitor/inputs/1541-c000.325302-01.bin',
    'firmware-control/active-plugins/system/1541HUD-USB-v0.8.1',
    'firmware-control/overlays/otp-output-init-v0.8.1.patch',
    'firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1',
    'firmware-monitor/active-plugins/system/1541HUD-Monitor-USB',
    'firmware-monitor/active-plugins/user/1541HUD-Monitor',
    'firmware-monitor/shared/1541hud_mailbox.h',
    'firmware-monitor/overlays/ub4-passive-v0.8.1.patch',
    'releases/v8.1-manifest.json',
    'docs/V81_BASELINE_RUNBOOK.md',
    'source/one-rom-v0.8.1/firmware/Makefile',
    'source/one-rom-v0.8.1/plugins/system/usb/Makefile',
    '1541hud/source/1541HUD_V0.8.0.51.py',
    '1541hud/source/1541HUD_V0.8.0.53.py',
    '1541hud/source/Run-1541HUD-V0.8.0.53.cmd',
    '1541hud/Build-1541HUD-V0.8.0.53.ps1',
    '1541hud/source/1541HUD_V0.8.0.58.py',
    '1541hud/source/Run-1541HUD-V0.8.0.58.cmd',
    '1541hud/Build-1541HUD-V0.8.0.58.ps1',
    'releases/stock-2rom-apply-only-compose.json',
    'releases/r21-basic-no-jiffydos-compose.json',
    '1541hud/tests/test_onerom_usb.py',
    'utilities/rom-pages/Split-1541-RomPages.ps1',
    'tools/onerom-v0.6.0-hud1.exe',
    'tools/windows-cli-manifest.json',
    'tools/windows-cli-dependencies.json',
    'tools/THIRD-PARTY-LICENSES.txt',
    'tools/maintenance/Build-Windows-CLI.ps1',
    'tools/maintenance/Package-CLI-Licenses.py',
    'tools/maintenance/Prepare-Host-Fixtures.py',
    'docs/WINDOWS_CLI.md',
    'releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0.8.1.bin',
    'releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-apply-only-v0.8.1.17.bin',
    'releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin'
)

$missing = $required | Where-Object { -not (Test-Path -LiteralPath (Join-Path $PackageRoot $_)) }
if ($missing) { throw "Public package is incomplete:`n$($missing -join "`n")" }

$forbiddenDirectories = @('roms/private', 'archives', 'c64-onerom-configurator', 'outputs', 'work', 'build-kits')
foreach ($name in $forbiddenDirectories) {
    if (Test-Path -LiteralPath (Join-Path $PackageRoot $name)) {
        throw "Forbidden private/generated directory in public package: $name"
    }
}

$forbiddenPath = '(?i)(\.bin$|\.rom$|\.uf2$|\.exe$|\.d64$|\.prg$)'
$romAssetManifestPath = Join-Path $PackageRoot 'roms/ASSETS.sha256'
$romAssetEntries = @{}
foreach ($line in Get-Content -LiteralPath $romAssetManifestPath) {
    if ($line -notmatch '^([0-9a-fA-F]{64})\s+(.+)$') {
        throw "Malformed ROM library checksum line: $line"
    }
    $relative = 'roms/' + $Matches[2].Replace('\', '/')
    if ($romAssetEntries.ContainsKey($relative)) { throw "Duplicate ROM library asset: $relative" }
    $romAssetEntries[$relative] = $Matches[1].ToUpperInvariant()
}
$speedSourceDir = Join-Path $PackageRoot 'roms/speeddos-plus/source-images'
$expectedSpeedSourceNames = @('1541both_sd', '1541lower', '1541upper_sd', 'kernal_en_sd')
$actualSpeedSourceNames = @(Get-ChildItem -LiteralPath $speedSourceDir -File | ForEach-Object { $_.Name } | Sort-Object)
if (($actualSpeedSourceNames -join "`n") -ne (@($expectedSpeedSourceNames | Sort-Object) -join "`n")) {
    throw 'SpeedDOS factory-named source images are missing or unexpected.'
}
$actualRomFiles = Get-ChildItem -LiteralPath (Join-Path $PackageRoot 'roms') -Recurse -Force -File |
    Where-Object {
        $_.Extension -in @('.bin', '.rom', '.d64', '.prg') -or
        $_.DirectoryName -eq (Join-Path $PackageRoot 'roms/speeddos-plus/source-images')
    } |
    ForEach-Object { $_.FullName.Substring($PackageRoot.Length + 1).Replace('\', '/') }
if ((@($actualRomFiles | Sort-Object) -join "`n") -ne (@($romAssetEntries.Keys | Sort-Object) -join "`n")) {
    throw 'ROM library binary files and checksum manifest do not match exactly.'
}
foreach ($relative in $romAssetEntries.Keys) {
    if ($relative -match '(?i)(jiffydos|s-jiffy|sjiffydos)') {
        throw "Licensed/generated JiffyDOS payload is forbidden in the public ROM library: $relative"
    }
    $asset = Join-Path $PackageRoot $relative
    if (-not (Test-Path -LiteralPath $asset) -or (Get-Sha256Hex -Path $asset) -ne $romAssetEntries[$relative]) {
        throw "ROM library asset hash mismatch: $relative"
    }
    $expectedBytes = if ($relative -match '(?i)dolphindos-2/reference/.*\.rom$') { 32768 }
        elseif ($relative -match '(?i)speeddos-plus/source-images/1541both_sd$') { 16384 }
        elseif ($relative -match '(?i)ramboard/original-cld/ramboard\.d64$') { 174848 }
        else { 8192 }
    if ((Get-Item -LiteralPath $asset).Length -ne $expectedBytes) {
        throw "ROM library asset has an unexpected byte count (expected $expectedBytes): $relative"
    }
}
$badPaths = Get-ChildItem -LiteralPath $PackageRoot -Recurse -Force -File |
    Where-Object {
        $relative = $_.FullName.Substring($PackageRoot.Length + 1).Replace('\', '/')
        # These are synthetic fixtures shipped in the pinned upstream source,
        # not commercial 1541/C64 ROM images. Keep the allowlist exact.
        $relative -notmatch '^(source/one-rom-v0\.8\.1/images/test/(stub-(system|user)-plugin\.bin|0_63_(2048|4096|8192)\.rom|ff8192\.rom|rand_(0\.5KB|1KB|2KB|4KB|8KB|16KB|32KB|48KB|64KB|128KB|256KB|512KB|512KB_alt)\.rom|zero(3|8192)\.rom)|releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-v0\.8\.1\.bin|releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-2rom-apply-only-v0\.8\.1\.17\.bin|releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0\.8\.1\.32\.bin|tools/onerom-v0\.6\.0-hud1\.exe|firmware-monitor/inputs/(1541-OneROM-Bootloader-Universal-v1\.1\.0|1541-c000\.325302-01)\.bin|roms/ramboard/original-cld/ramboard\.d64)$' -and
        $relative -notmatch '^roms/(stock-dos-2\.6|speeddos-plus|dolphindos-2)/.*\.(bin|rom)$' -and
        $relative -match $forbiddenPath
    }
if ($badPaths) {
    throw "Forbidden payload/history file(s) in public package:`n$($badPaths.FullName -join "`n")"
}

$cliRecord = Get-Content -Raw -LiteralPath (Join-Path $PackageRoot 'tools/windows-cli-manifest.json') | ConvertFrom-Json
$hostPins = Get-Content -Raw -LiteralPath (Join-Path $PackageRoot 'overlays/host-dependencies-v0.8.1.json') | ConvertFrom-Json
$sourceManifestPath = Join-Path $PackageRoot 'source/one-rom-v0.8.1/1541HUD-SOURCE.json'
$sourcePins = Get-Content -Raw -LiteralPath $sourceManifestPath | ConvertFrom-Json
& python (Join-Path $PackageRoot 'tools/maintenance/Verify-Source-Dependencies.py') (Join-Path $PackageRoot 'source/one-rom-v0.8.1')
if ($LASTEXITCODE -ne 0) { throw 'Checked-in supported source verification failed.' }
$expectedCliLock = $hostPins.lock_sha256
if ($cliRecord.PSObject.Properties.Name -contains 'source_hardening_sha256') {
    if ($cliRecord.source_hardening_sha256 -ne (Get-LfTextSha256Hex -Path $sourceManifestPath)) {
        throw 'Rebuilt CLI source provenance differs from the checked-in source manifest.'
    }
    $expectedCliLock = $sourcePins.files_sha256_lf.'rust/Cargo.lock'
}
$cliPath = Join-Path $PackageRoot 'tools/onerom-v0.6.0-hud1.exe'
if ($cliRecord.file -ne 'onerom-v0.6.0-hud1.exe' -or
    $cliRecord.upstream_cli -ne '0.6.0' -or
    $cliRecord.target -ne 'x86_64-pc-windows-msvc' -or
    $cliRecord.overlay_sha256 -ne $hostPins.patch_sha256 -or
    $cliRecord.lock_sha256 -ne $expectedCliLock -or
    (Get-Item -LiteralPath $cliPath).Length -ne $cliRecord.bytes -or
    (Get-Sha256Hex -Path $cliPath) -ne $cliRecord.sha256) {
    throw 'Windows CLI binary, dependency overlay and provenance manifest disagree.'
}

$expectedMonitorInputs = @{
    'source/one-rom-v0.8.1/images/test/stub-system-plugin.bin' = 'D645C477FD96C25020EE67B35D8E3C90834DC6C1931E7B1DF1A7DDEF3695C981'
    'source/one-rom-v0.8.1/images/test/stub-user-plugin.bin' = '2616755B5219400AB4842F71C19BF48E9157E6AABB50F78A817A7886381CA7EF'
    'firmware-monitor/inputs/1541-OneROM-Bootloader-Universal-v1.1.0.bin' = '7313B07F26FCC3778664B0C53F1AFF46AF883ED8B0A3CBF525EFC9BB40432B0E'
    'firmware-monitor/inputs/1541-c000.325302-01.bin' = '6FA7B07AFF92DA66B0A28A52BB3C82FFE310AB0FAD2CC473B40137A8D299C7E5'
}
foreach ($relative in $expectedMonitorInputs.Keys) {
    $input = Join-Path $PackageRoot $relative
    if ($relative -like 'firmware-monitor/inputs/*' -and (Get-Item -LiteralPath $input).Length -ne 8192) {
        throw "Monitor input is not 8192 bytes: $relative"
    }
    $actual = Get-Sha256Hex -Path $input
    if ($actual -ne $expectedMonitorInputs[$relative]) {
        throw "Monitor input hash mismatch: $relative"
    }
}

$textExtensions = @('.md', '.ps1', '.cmd', '.json', '.py', '.h', '.c', '.txt', '.yml', '.yaml')
# Upstream source and manuals legitimately contain example chip IDs and hashes.
# Only reject known private identifiers and local operator paths, not every
# sixteen-digit hexadecimal string in a third-party manual.
$forbiddenText = '(?i)(C:\\Users\\A[d]min\\|/Users/A[d]min/|8FAEB3DEC304ADCF|77920CCCA13D0FC9)'
$textHits = foreach ($file in Get-ChildItem -LiteralPath $PackageRoot -Recurse -Force -File) {
    $relative = $file.FullName.Substring($PackageRoot.Length + 1).Replace('\', '/')
    if ($relative -eq 'tools/maintenance/Test-Public-Package.ps1') { continue }
    if ($textExtensions -notcontains $file.Extension) { continue }
    Select-String -LiteralPath $file.FullName -Pattern $forbiddenText | ForEach-Object {
        "$($file.FullName.Substring($PackageRoot.Length + 1)):$($_.LineNumber):$($_.Line.Trim())"
    }
}
if ($textHits) { throw "Private path or serial-like data in public package:`n$($textHits -join "`n")" }

$manifest = Get-Content -LiteralPath (Join-Path $PackageRoot 'releases/v8.1-manifest.json') -Raw | ConvertFrom-Json
foreach ($image in $manifest.images) {
    $path = Join-Path $PackageRoot "releases/$($image.file)"
    if ((Get-Sha256Hex -Path $path) -ne $image.sha256 -or
        (Get-Item -LiteralPath $path).Length -ne $image.bytes) {
        throw "Release image hash/size mismatch: $($image.file)"
    }
    $binaryText = [Text.Encoding]::GetEncoding(28591).GetString([IO.File]::ReadAllBytes($path))
    if ($binaryText -match $forbiddenText -or $binaryText -match '/mnt/[a-z]/Users/|/tmp/ub3-public-') {
        throw "Private build marker in release image: $($image.file)"
    }
}

Write-Host "PASS: $PackageRoot is structurally complete and contains no denied payload, history, serial, or local-path markers."


