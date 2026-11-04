[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$RepoRoot,

    [Parameter(Mandatory)]
    [string]$Cli
)

$ErrorActionPreference = 'Stop'

function Invoke-OneRom {
    param([Parameter(Mandatory)][string[]]$Arguments)
    & $Cli @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "OneROM CLI failed with exit code $LASTEXITCODE."
    }
}

$RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
$Cli = (Resolve-Path -LiteralPath $Cli).Path
$base = Join-Path $RepoRoot 'firmware-control\source\control-baseline-v0.8.1-release\firmware\build\onerom-rp235x.bin'
$configDirectory = Join-Path $RepoRoot 'firmware-control\config-templates'
$config = '1541-DriveHUD-Control-UB3-74LS138-8-ROM-Collection-v0.8.1.json'
$output = Join-Path $RepoRoot 'firmware-control\releases\v8.1\reset-integration\onerom-1541HUD-Control-UB3-74LS138-8-ROM-fire-24-g-c46reset350-v0.8.1.bin'

if (-not (Test-Path -LiteralPath $base)) { throw "Missing locally built Control base: $base" }
if (-not (Test-Path -LiteralPath (Join-Path $configDirectory $config))) { throw "Missing Control config: $config" }

Push-Location $configDirectory
try {
    Invoke-OneRom @('--verbose', 'firmware', 'build', '--board', 'fire-24-g', '--config', $config, '--base-firmware', $base, '--output', $output, '--force')
    Invoke-OneRom @('firmware', 'inspect', '--firmware', $output)
    # certutil is part of supported Windows and remains available in the
    # constrained PowerShell host used by the batch launcher, unlike the
    # Get-FileHash module import on some locked-down systems.
    & (Join-Path $env:SystemRoot 'System32\certutil.exe') -hashfile $output SHA256
    if ($LASTEXITCODE -ne 0) { throw "SHA-256 calculation failed with exit code $LASTEXITCODE." }
}
finally {
    Pop-Location
}
