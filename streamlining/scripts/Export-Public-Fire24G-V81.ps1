[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$Destination,
    [string]$RepoRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

<##
Creates a new ROM-free Fire-24G V8.1 repository tree. It never modifies the
private source repository and refuses a non-empty destination.
##>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
if (Test-Path -LiteralPath $Destination) {
    if ((Get-ChildItem -LiteralPath $Destination -Force | Measure-Object).Count -ne 0) {
        throw "Destination must be empty: $Destination"
    }
} else {
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
}
$Destination = (Resolve-Path -LiteralPath $Destination).Path

function Copy-PublicTree {
    param([string]$Relative)
    $source = Join-Path $RepoRoot $Relative
    $target = Join-Path $Destination $Relative
    if (-not (Test-Path -LiteralPath $source)) { throw "Missing public input: $source" }
    $item = Get-Item -LiteralPath $source
    if (-not $item.PSIsContainer) {
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
        Copy-Item -LiteralPath $source -Destination $target -Force
        return
    }

    # Copy source selectively rather than first copying gigabytes of local
    # compiler products and then deleting them. Robocopy is used deliberately:
    # a PowerShell Copy-Item loop over the upstream source tree is needlessly
    # slow enough to make a normal export look hung.
    New-Item -ItemType Directory -Force -Path $target | Out-Null
    & robocopy $source $target /E /R:1 /W:1 /NFL /NDL /NJH /NJS /NP `
        /XD .git target build build-* `
        /XF *.bin *.uf2 *.d64 *.prg *.exe | Out-Null
    # Robocopy uses 0-7 for successful copies, including "files copied".
    if ($LASTEXITCODE -gt 7) { throw "Robocopy failed while exporting $Relative (exit $LASTEXITCODE)." }
}

# Only this allowlist becomes the public repository. Historical, generated and
# ROM-bearing material is omitted by construction.
@(
    'LICENSE',
    'streamlining',
    'docs/FIRE24G_V81_ENGINEERING_DOSSIER.md',
    'docs/FIRE24G_V81_BUILD_AND_INSTALL.md',
    'docs/FIRE24G-VERIFIED-WIRING.md',
    'docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md',
    'docs/THIRD_PARTY_DOS_PAGE_PREPARATION.md',
    'docs/PUBLIC_FIRE24G_RELEASES.md',
    'consolidation/scripts/Build-Control-V8.1-Clean.cmd',
    'consolidation/scripts/Build-Control-V8.1-Clean.ps1',
    'firmware-control/source/control-baseline-v0.8.1-release',
    'firmware-control/active-plugins/system/1541HUD-USB-v0.8.1',
    'firmware-control/active-plugins/user/1541HUD-Host-Control-v0.8.1',
    'firmware-monitor/source/monitor-baseline-v0.8.1-upstream',
    'firmware-monitor/active-plugins/system/1541HUD-Monitor-USB',
    'firmware-monitor/active-plugins/user/1541HUD-Monitor',
    'firmware-monitor/shared',
    'firmware-monitor/Build-Monitor-Fire24G-V0.8.1.ps1',
    'firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh',
    '1541hud/source/1541HUD_V0.8.0.51.py',
    '1541hud/source/Run-1541HUD-V0.8.0.51.cmd',
    '1541hud/Build-1541HUD-V0.8.0.51.ps1',
    '1541hud/tests/test_onerom_usb.py',
    'utilities/rom-pages/Split-1541-RomPages.ps1'
) | ForEach-Object { Copy-PublicTree $_ }

# These two stock-only images are the sole deliberate binary release
# exceptions. Do not add a wildcard here.
New-Item -ItemType Directory -Force -Path (Join-Path $Destination 'releases') | Out-Null
Copy-Item -LiteralPath (Join-Path $RepoRoot 'firmware-control\releases\v8.1\public\onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin') -Destination (Join-Path $Destination 'releases\onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin') -Force
Copy-Item -LiteralPath (Join-Path $RepoRoot 'firmware-monitor\releases\v8.1\candidates\onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin') -Destination (Join-Path $Destination 'releases\onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin') -Force

# Core source trees may contain local compiler products. They are neither
# source nor publishable payloads; remove them from the exported tree before
# the policy check. This only touches the newly-created destination.
Get-ChildItem -LiteralPath $Destination -Recurse -Force -File |
    Where-Object { $_.Extension -in @('.bin', '.uf2', '.d64', '.prg', '.exe') } |
    Remove-Item -Force

New-Item -ItemType Directory -Force -Path (Join-Path $Destination 'private-roms') | Out-Null
@'
# Private ROM inputs

This directory is intentionally empty and ignored by Git. Supply only ROM
images you are licensed to use. The public source repository does not contain
ROM bytes, bootloader binaries, composed OneROM firmware, disk images, or
third-party DOS payloads.

Use the Fire-24G V8.1 engineering dossier and the local/private composition
record to create a private manifest appropriate to your legally held ROMs.
'@ | Set-Content -LiteralPath (Join-Path $Destination 'private-roms/README.md') -Encoding utf8

Copy-Item -LiteralPath (Join-Path $Destination 'streamlining/public-template/control-compose.template.json') `
    -Destination (Join-Path $Destination 'private-roms/UB3-Control-DolphinDOS-CAE.json')
Copy-Item -LiteralPath (Join-Path $Destination 'streamlining/public-template/UB3-Control-Lower-Socket.template.json') `
    -Destination (Join-Path $Destination 'private-roms/UB3-Control-Lower-Socket.json')
Copy-Item -LiteralPath (Join-Path $Destination 'streamlining/public-template/UB3-Control-Upper-Socket.template.json') `
    -Destination (Join-Path $Destination 'private-roms/UB3-Control-Upper-Socket.json')
Copy-Item -LiteralPath (Join-Path $Destination 'streamlining/public-template/monitor-compose.template.json') `
    -Destination (Join-Path $Destination 'private-roms/monitor-compose.json')

@'
private-roms/*
!private-roms/README.md
**/build/
**/build-*/
*.bin
*.uf2
*.d64
*.prg
*.exe
!releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin
!releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.28.bin
'@ | Set-Content -LiteralPath (Join-Path $Destination '.gitignore') -Encoding utf8

@'
# 1541HUD / OneROM Fire-24G V8.1

ROM-free source package for the paired Fire-24G 1541HUD system. Start with
`docs/FIRE24G_V81_ENGINEERING_DOSSIER.md` and
`docs/UB3_CONTROL_CLI_COMPOSITION_GUIDE.md`. Supply your own lawful ROM inputs
under `private-roms/`; this repository cannot compose a drive image without
them.

Run `streamlining/scripts/Test-Public-Package.ps1 -PackageRoot .` before
initializing or publishing the resulting repository.
'@ | Set-Content -LiteralPath (Join-Path $Destination 'README.md') -Encoding utf8

& (Join-Path $Destination 'streamlining/scripts/Test-Public-Package.ps1') -PackageRoot $Destination
Write-Host "Public package exported: $Destination"
