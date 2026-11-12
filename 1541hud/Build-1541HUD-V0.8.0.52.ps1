[CmdletBinding()]
param([string]$Python = "py", [string]$Version = "V0.8.0.52", [string]$OutputDirectory = "")
$ErrorActionPreference = "Stop"
$Source = Join-Path $PSScriptRoot "source\1541HUD_V0.8.0.52.py"
if (-not (Test-Path -LiteralPath $Source)) { throw "1541HUD V0.8.0.52 source was not found: $Source" }
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) { $OutputDirectory = Join-Path $PSScriptRoot "releases\v0.8.0\candidates\$Version-win-x64" }
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$BuildTemp = Join-Path $OutputDirectory ".build-temp"
New-Item -ItemType Directory -Force -Path $BuildTemp | Out-Null
$env:TEMP = $BuildTemp; $env:TMP = $BuildTemp; $env:NUITKA_CACHE_DIR = Join-Path $BuildTemp ".nuitka-cache"
New-Item -ItemType Directory -Force -Path $env:NUITKA_CACHE_DIR | Out-Null
& $Python -3 -m py_compile $Source
if ($LASTEXITCODE -ne 0) { throw "Python syntax check failed." }
& $Python -3 -m unittest discover -s (Join-Path $PSScriptRoot "tests") -v
if ($LASTEXITCODE -ne 0) { throw "1541HUD regression tests failed." }
$TempSpec = "{CACHE_DIR}\1541HUD\$Version"; $OutputName = "1541HUD-$Version-win-x64.exe"
& $Python -m nuitka --update-check=never --mode=onefile --enable-plugin=tk-inter --windows-console-mode=disable --assume-yes-for-downloads --onefile-tempdir-spec=$TempSpec --output-dir=$OutputDirectory --output-filename=$OutputName $Source
if ($LASTEXITCODE -ne 0) { throw "Nuitka failed with exit code $LASTEXITCODE." }
$Artifact = Join-Path $OutputDirectory $OutputName
if (-not (Test-Path -LiteralPath $Artifact)) { throw "Nuitka did not create: $Artifact" }
& "$env:SystemRoot\System32\certutil.exe" -hashfile $Artifact SHA256
if ($LASTEXITCODE -ne 0) { throw "Unable to calculate SHA-256 for: $Artifact" }
