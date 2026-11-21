[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$source = Join-Path $root 'source/C64-OneROM-Configurator-r20-V1.4.31.bas'
$tokenizer = Join-Path $root 'source/scripts/tokenize_basic_v2.py'
$releasedPrg = Join-Path $root 'releases/C64-OneROM-Configurator-r20-V1.4.31.PRG'
$buildDirectory = Join-Path $root 'build'
$builtPrg = Join-Path $buildDirectory 'C64-OneROM-Configurator-r20-V1.4.31.PRG'
$manifestPath = Join-Path $root 'manifest.json'

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$pythonLauncher = Get-Command py -ErrorAction SilentlyContinue
if ($pythonLauncher) {
    & $pythonLauncher.Source -3 $tokenizer $source $builtPrg
} else {
    & python $tokenizer $source $builtPrg
}
if ($LASTEXITCODE -ne 0) { throw 'BASIC tokenization failed.' }

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$checks = @(
    @{ Path = $source; Expected = $manifest.source.sha256 },
    @{ Path = $tokenizer; Expected = $manifest.tokenizer.sha256 },
    @{ Path = $releasedPrg; Expected = $manifest.program.sha256 },
    @{ Path = $builtPrg; Expected = $manifest.program.sha256 }
)
foreach ($check in $checks) {
    $actual = (Get-FileHash -LiteralPath $check.Path -Algorithm SHA256).Hash
    if ($actual -ne $check.Expected) {
        throw "SHA-256 mismatch for $($check.Path). Expected $($check.Expected), got $actual."
    }
}

if ((Get-Item -LiteralPath $builtPrg).Length -ne $manifest.program.bytes) {
    throw 'Rebuilt PRG size does not match manifest.json.'
}
Write-Host "PASS: rebuilt PRG matches the published V1.4.31 artifact ($($manifest.program.sha256))."
