[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$RepoRoot,

    [Parameter(Mandatory)]
    [string]$Cli,

    [Parameter(Mandatory)]
    [string]$Config,

    [string]$Output = ''
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
$Config = (Resolve-Path -LiteralPath $Config).Path
$base = Join-Path $RepoRoot 'firmware-control\.work\control-core-v0.8.1\firmware-control\source\control-baseline-v0.8.1-release\firmware\build\onerom-rp235x.bin'
if (-not $Output) { $Output = Join-Path $RepoRoot 'output\onerom-ub3-control-fire24g.bin' }
$outputParent = Split-Path -Parent $Output
New-Item -ItemType Directory -Force -Path $outputParent | Out-Null

if (-not (Test-Path -LiteralPath $base)) { throw "Missing locally built Control base: $base" }

Push-Location (Split-Path -Parent $Config)
try {
    Invoke-OneRom @('--verbose', 'firmware', 'build', '--board', 'fire-24-g', '--config', $Config, '--base-firmware', $base, '--output', $Output, '--force')
    Invoke-OneRom @('firmware', 'inspect', '--firmware', $Output)
    # certutil is part of supported Windows and remains available in the
    # constrained PowerShell host used by the batch launcher, unlike the
    # Get-FileHash module import on some locked-down systems.
    & (Join-Path $env:SystemRoot 'System32\certutil.exe') -hashfile $Output SHA256
    if ($LASTEXITCODE -ne 0) { throw "SHA-256 calculation failed with exit code $LASTEXITCODE." }
}
finally {
    Pop-Location
}
