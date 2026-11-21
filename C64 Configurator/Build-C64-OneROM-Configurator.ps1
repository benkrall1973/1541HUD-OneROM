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

function Get-LfTextSha256 {
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
    @{ Path = $source; Expected = $manifest.source.sha256; Text = $true },
    @{ Path = $tokenizer; Expected = $manifest.tokenizer.sha256; Text = $true },
    @{ Path = $releasedPrg; Expected = $manifest.program.sha256; Text = $false },
    @{ Path = $builtPrg; Expected = $manifest.program.sha256; Text = $false }
)
foreach ($check in $checks) {
    $actual = if ($check.Text) { Get-LfTextSha256 -Path $check.Path } else { (Get-FileHash -LiteralPath $check.Path -Algorithm SHA256).Hash }
    if ($actual -ne $check.Expected) {
        throw "SHA-256 mismatch for $($check.Path). Expected $($check.Expected), got $actual."
    }
}

foreach ($check in @(@{ Path = $source; Bytes = $manifest.source.bytes }, @{ Path = $tokenizer; Bytes = $manifest.tokenizer.bytes })) {
    $text = [System.IO.File]::ReadAllText($check.Path, [System.Text.Encoding]::UTF8)
    $text = $text.Replace("`r`n", "`n").Replace("`r", "`n")
    $bytes = [System.Text.UTF8Encoding]::new($false).GetByteCount($text)
    if ($bytes -ne $check.Bytes) { throw "Normalized text byte count mismatch for $($check.Path)." }
}

if ((Get-Item -LiteralPath $builtPrg).Length -ne $manifest.program.bytes) {
    throw 'Rebuilt PRG size does not match manifest.json.'
}
Write-Host "PASS: rebuilt PRG matches the published V1.4.31 artifact ($($manifest.program.sha256))."
