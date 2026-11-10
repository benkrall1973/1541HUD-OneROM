[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$InputFile,
    [Parameter(Mandatory)][string]$OutputFile
)
$ErrorActionPreference = 'Stop'
$bytes = [IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $InputFile).Path)
$text = [Text.Encoding]::Latin1.GetString($bytes)
# Only absolute, NUL-terminated TinyUSB diagnostic filename strings qualify.
# Equal-length replacement retains every instruction, address and terminator.
$pattern = '(?<=\x00)/mnt/[^\x00\r\n]+/tinyusb/src/portable/raspberrypi/rp2040/(dcd_rp2040|rp2040_usb)\.c(?=\x00)'
$matches = [regex]::Matches($text, $pattern)
if ($matches.Count -ne 2) { throw 'Expected exactly two TinyUSB diagnostic filename strings.' }
foreach ($match in $matches) {
    $suffix = $match.Value.Substring($match.Value.LastIndexOf('/tinyusb/'))
    $prefixLength = $match.Length - $suffix.Length
    $portable = '/build' + ('_' * ($prefixLength - 6)) + $suffix
    $replacement = [Text.Encoding]::ASCII.GetBytes($portable)
    if ($replacement.Length -ne $match.Length) { throw 'Replacement length mismatch.' }
    [Array]::Copy($replacement, 0, $bytes, $match.Index, $replacement.Length)
}
$resultPath = [IO.Path]::GetFullPath((Join-Path $PWD $OutputFile))
[IO.File]::WriteAllBytes($resultPath, $bytes)
foreach ($match in $matches) {
    [pscustomobject]@{Offset=$match.Index; Length=$match.Length}
}
