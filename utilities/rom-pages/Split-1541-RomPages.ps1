[CmdletBinding()]
param(
    [Parameter(Mandatory)] [string] $InputPath,
    [Parameter(Mandatory)] [string] $OutputDirectory,
    [ValidateSet('CE', 'CAE')] [string] $PageOrder = 'CE',
    [switch] $MirrorEToA,
    [string] $Prefix = '1541-drive'
)

$ErrorActionPreference = 'Stop'
$input = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputPath))
$expected = if ($PageOrder -eq 'CE') { 16384 } else { 24576 }
if ($input.Length -ne $expected) {
    throw "Expected a $expected-byte $PageOrder image, got $($input.Length) bytes. Do not guess the ROM layout."
}
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
function Write-Page([string]$name, [int]$offset) {
    $page = New-Object byte[] 8192
    [Array]::Copy($input, $offset, $page, 0, 8192)
    $path = Join-Path $OutputDirectory "$Prefix-$name.bin"
    [IO.File]::WriteAllBytes($path, $page)
    "$(Get-FileHash -Algorithm SHA256 -LiteralPath $path | Select-Object -ExpandProperty Hash)  $path"
}

Write-Page 'c000-dfff' 0
if ($PageOrder -eq 'CAE') { Write-Page 'a000-bfff' 8192; Write-Page 'e000-ffff' 16384 }
else {
    Write-Page 'e000-ffff' 8192
    if ($MirrorEToA) {
        Copy-Item -LiteralPath (Join-Path $OutputDirectory "$Prefix-e000-ffff.bin") -Destination (Join-Path $OutputDirectory "$Prefix-a000-bfff.bin") -Force
        'Mirrored E page to A page as requested.'
    }
}
