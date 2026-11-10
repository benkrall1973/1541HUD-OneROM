[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string] $Toolchain,
    [string] $OneRomCli = 'onerom',
    [string] $OutputPath = ''
)
$ErrorActionPreference = 'Stop'
# Toolchain, CLI and optional output are Linux paths inside WSL.
# Upstream requires POSIX tools; use the same build implementation on Windows.
$scriptPath = Join-Path $PSScriptRoot 'Build-Monitor-Fire24G-V0.8.1.sh'
$linuxScript = & wsl.exe --exec wslpath -u $scriptPath
if ($LASTEXITCODE -ne 0) { throw 'Unable to translate build script path in WSL.' }
$buildArgs = @('--exec', 'env', "TOOLCHAIN=$Toolchain", "ONEROM_CLI=$OneRomCli")
if ($OutputPath) { $buildArgs += "OUTPUT_PATH=$OutputPath" }
$buildArgs += @('bash', $linuxScript.Trim())
& wsl.exe @buildArgs
if ($LASTEXITCODE -ne 0) { throw "Monitor candidate build failed ($LASTEXITCODE)." }
