[CmdletBinding()]
param(
    [string]$Python = "py",
    [string]$PythonVersion = "3.12",
    [string]$Version = "V0.8.0.58",
    [string]$OutputDirectory = ""
)

$ErrorActionPreference = "Stop"
$Source = Join-Path $PSScriptRoot "source\1541HUD_V0.8.0.58.py"
if (-not (Test-Path -LiteralPath $Source)) { throw "1541HUD V0.8.0.58 source was not found: $Source" }
if ($PythonVersion -notmatch '^3\.\d+$') { throw "PythonVersion must be a Python 3 minor version, such as 3.12." }
$PythonArgs = @()
if ([System.IO.Path]::GetFileNameWithoutExtension($Python) -ieq 'py') { $PythonArgs = @("-$PythonVersion") }
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) { $OutputDirectory = Join-Path $PSScriptRoot "releases\v0.8.0.58" }
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$BuildTemp = Join-Path $PSScriptRoot "..\.buildtools\hud-v0.8.0.58"
$BuildTemp = [System.IO.Path]::GetFullPath($BuildTemp)
New-Item -ItemType Directory -Force -Path $BuildTemp | Out-Null
$StagingDirectory = Join-Path $BuildTemp 'output'
New-Item -ItemType Directory -Force -Path $StagingDirectory | Out-Null
$CacheDirectory = Join-Path $BuildTemp ".nuitka-cache"
New-Item -ItemType Directory -Force -Path $CacheDirectory | Out-Null
$OriginalTemp = $env:TEMP
$OriginalTmp = $env:TMP
$OriginalNuitkaCache = $env:NUITKA_CACHE_DIR
try {
    $env:TEMP = $BuildTemp
    $env:TMP = $BuildTemp
    $env:NUITKA_CACHE_DIR = $CacheDirectory

    & $Python @PythonArgs -m py_compile $Source
    if ($LASTEXITCODE -ne 0) { throw "Python syntax check failed using Python $PythonVersion." }

    & $Python @PythonArgs -c "import nuitka, serial, tkinter"
    if ($LASTEXITCODE -ne 0) { throw "Python $PythonVersion must have Nuitka, pyserial, and Tkinter installed. See 1541hud/README.md." }

    $TempSpec = "{CACHE_DIR}\1541HUD\$Version"
    $OutputName = "1541HUD-$Version-win-x64.exe"
    & $Python @PythonArgs -m nuitka --update-check=never --mode=onefile --enable-plugin=tk-inter --windows-console-mode=disable --assume-yes-for-downloads --onefile-tempdir-spec=$TempSpec --output-dir=$StagingDirectory --output-filename=$OutputName $Source
    if ($LASTEXITCODE -ne 0) { throw "Nuitka failed with exit code $LASTEXITCODE." }

    $StagedArtifact = Join-Path $StagingDirectory $OutputName
    if (-not (Test-Path -LiteralPath $StagedArtifact)) { throw "Nuitka did not create: $StagedArtifact" }
    $Artifact = Join-Path $OutputDirectory $OutputName
    Copy-Item -LiteralPath $StagedArtifact -Destination $Artifact -Force
    $Hash = (Get-FileHash -LiteralPath $Artifact -Algorithm SHA256).Hash
    $PythonVersionFull = (& $Python @PythonArgs --version 2>&1 | Out-String).Trim()
    $NuitkaVersion = (& $Python @PythonArgs -c "import importlib.metadata; print(importlib.metadata.version('Nuitka'))").Trim()
    $PySerialVersion = (& $Python @PythonArgs -c "import importlib.metadata; print(importlib.metadata.version('pyserial'))").Trim()
    $SourceRevision = (& git -C (Join-Path $PSScriptRoot '..') rev-parse HEAD 2>$null | Out-String).Trim()
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($SourceRevision)) { $SourceRevision = 'uncommitted-source' }
    $BuildInfo = [ordered]@{
        product = '1541HUD'
        version = $Version
        artifact = $OutputName
        platform = 'Windows x64'
        bytes = (Get-Item -LiteralPath $Artifact).Length
        sha256 = $Hash
        source_revision = $SourceRevision
        source_sha256 = (Get-FileHash -LiteralPath $Source -Algorithm SHA256).Hash
        python = $PythonVersionFull
        nuitka = $NuitkaVersion
        pyserial = $PySerialVersion
        build_mode = 'Nuitka onefile; console disabled'
        signed = $false
        hardware_tested = $false
    }
    $BuildInfoPath = Join-Path $OutputDirectory 'BUILD-INFO.json'
    $BuildInfo | ConvertTo-Json | Set-Content -LiteralPath $BuildInfoPath -Encoding utf8
    Write-Host "Built $Artifact"
    Write-Host "SHA-256 $Hash"
    Write-Host "Build record $BuildInfoPath"
}
finally {
    $env:TEMP = $OriginalTemp
    $env:TMP = $OriginalTmp
    $env:NUITKA_CACHE_DIR = $OriginalNuitkaCache
}
