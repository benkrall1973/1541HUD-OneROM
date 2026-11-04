[CmdletBinding()]
param(
    [string]$RepoRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)),
    [Parameter(Mandatory)]
    [string]$UpstreamRoot,
    [string]$OutputPatch = '',
    [switch]$KeepWork
)

<#
.SYNOPSIS
Generates the reviewable Fire-24G V8.1 source overlay.

.DESCRIPTION
The Control and Monitor use separate copies of the same upstream OneROM
release. This script creates a disposable project-shaped baseline containing
two clean upstream copies, layers only the project-owned core changes and
active plugins over it, then emits one git patch. It does not read roms/,
release images, build artifacts, historical trees, or the C64 configurator.

The caller supplies a checkout of one-rom at releases/0.8.1 commit
379584110988b407064ddf1d18582dab5df1da4a. If it is a Git checkout the
commit is verified. An exported source archive is also accepted, but is
labelled unverified in the generated patch header.
#>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$expectedCommit = '379584110988b407064ddf1d18582dab5df1da4a'
$RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
$UpstreamRoot = (Resolve-Path -LiteralPath $UpstreamRoot).Path
if (-not $OutputPatch) {
    $OutputPatch = Join-Path $RepoRoot 'streamlining\generated\fire24g-v8.1-project-overlay.patch'
}
$OutputPatch = [IO.Path]::GetFullPath($OutputPatch)

function Invoke-Checked {
    param([Parameter(Mandatory)][scriptblock]$Command, [Parameter(Mandatory)][string]$Step)
    & $Command
    if ($LASTEXITCODE -ne 0) { throw "$Step failed with exit code $LASTEXITCODE." }
}

function Copy-Tree {
    param(
        [Parameter(Mandatory)][string]$Source,
        [Parameter(Mandatory)][string]$Destination,
        [string[]]$ExcludeDirectories = @()
    )
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
    $args = @($Source, $Destination, '/E', '/NFL', '/NDL', '/NJH', '/NJS', '/NC', '/NS')
    if ($ExcludeDirectories.Count) { $args += '/XD'; $args += $ExcludeDirectories }
    & robocopy @args | Out-Null
    # Robocopy exit values 0-7 are success/warnings; 8+ is failure.
    if ($LASTEXITCODE -ge 8) { throw "Copy from $Source failed with robocopy exit code $LASTEXITCODE." }
}

function Add-ProjectTree {
    param(
        [Parameter(Mandatory)][string]$SourceRelative,
        [Parameter(Mandatory)][string]$DestinationRoot
    )
    $source = Join-Path $RepoRoot $SourceRelative
    if (-not (Test-Path -LiteralPath $source)) { throw "Required project input missing: $source" }
    $destination = Join-Path $DestinationRoot $SourceRelative
    Copy-Tree $source $destination
}

if (-not (Test-Path -LiteralPath (Join-Path $UpstreamRoot 'firmware\src\rp235x.c'))) {
    throw "UpstreamRoot is not a OneROM source tree: $UpstreamRoot"
}

$upstreamVerification = 'source archive supplied; Git commit not independently verified'
if (Test-Path -LiteralPath (Join-Path $UpstreamRoot '.git')) {
    $actualCommit = (& git -C $UpstreamRoot rev-parse HEAD).Trim()
    if ($actualCommit -ne $expectedCommit) {
        throw "UpstreamRoot is $actualCommit; expected releases/0.8.1 $expectedCommit."
    }
    $upstreamVerification = "verified Git checkout at $actualCommit"
}

$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
$work = Join-Path ([IO.Path]::GetTempPath()) "1541hud-streamlining-$stamp"
$baseline = Join-Path $work 'baseline'
$overlay = Join-Path $work 'overlay'
$generatedDir = Split-Path -Parent $OutputPatch
New-Item -ItemType Directory -Force -Path $generatedDir, $baseline, $overlay | Out-Null

try {
    # A baseline has two deliberately independent upstream source trees.
    foreach ($target in @(
        'firmware-control\source\control-baseline-v0.8.1-release',
        'firmware-monitor\source\monitor-baseline-v0.8.1-upstream'
    )) {
        Copy-Tree $UpstreamRoot (Join-Path $baseline $target)
    }

    Copy-Tree $baseline $overlay
    Invoke-Checked { git -C $overlay init --quiet } 'Initialize overlay baseline repository'
    Invoke-Checked { git -C $overlay config user.name '1541HUD overlay generator' } 'Configure overlay author name'
    Invoke-Checked { git -C $overlay config user.email 'noreply@example.invalid' } 'Configure overlay author email'
    Invoke-Checked { git -C $overlay add -A } 'Stage upstream baseline'
    Invoke-Checked { git -C $overlay commit --quiet -m 'Pinned upstream OneROM v0.8.1 baseline' } 'Commit upstream baseline'

    # Copy the two patched cores. Generated and build output is intentionally
    # excluded because the normal build regenerates it and it does not belong
    # in a source patch.
    Copy-Tree (Join-Path $RepoRoot 'firmware-control\source\control-baseline-v0.8.1-release') `
        (Join-Path $overlay 'firmware-control\source\control-baseline-v0.8.1-release') `
        @('.git', '.git-local-metadata', 'build', 'build-*', 'generated', 'target')
    Copy-Tree (Join-Path $RepoRoot 'firmware-monitor\source\monitor-baseline-v0.8.1-upstream') `
        (Join-Path $overlay 'firmware-monitor\source\monitor-baseline-v0.8.1-upstream') `
        @('.git', '.git-local-metadata', 'build', 'build-*', 'generated', 'target')

    # Project-owned active source only. No ROM tree, release binary, historic
    # material, bootloader output, or configurator source is selected here.
    @(
        'firmware-control\active-plugins\system\1541HUD-USB-v0.8.1',
        'firmware-control\active-plugins\user\1541HUD-Host-Control-v0.8.1',
        'firmware-monitor\active-plugins\system\1541HUD-Monitor-USB',
        'firmware-monitor\active-plugins\user\1541HUD-Monitor',
        'firmware-monitor\shared'
    ) | ForEach-Object { Add-ProjectTree $_ $overlay }

    $patchText = & git -C $overlay diff --binary --full-index --no-ext-diff HEAD
    if (-not $patchText) { throw 'Overlay generation produced an empty patch.' }
    $header = @(
        '# Fire-24G V8.1 project source overlay',
        "# Upstream: piersfinlayson/one-rom releases/0.8.1 $expectedCommit",
        "# Verification: $upstreamVerification",
        '# Scope: patched UB3/UB4 cores and active project-owned plugins only.',
        '# Excludes: ROM payloads, composed images, historical trees, generated output, and configurator.',
        ''
    ) -join [Environment]::NewLine
    # Windows PowerShell's Set-Content defaults to UTF-16LE. Git patches are
    # text interchange files, so write a UTF-8 file without a BOM instead.
    $patchBody = $patchText -join [Environment]::NewLine
    $patchFile = $header + [Environment]::NewLine + $patchBody + [Environment]::NewLine
    $utf8NoBom = New-Object -TypeName System.Text.UTF8Encoding -ArgumentList $false
    [IO.File]::WriteAllText($OutputPatch, $patchFile, $utf8NoBom)

    Invoke-Checked { git -C $overlay apply --check $OutputPatch } 'Validate generated overlay'
    Write-Host "Generated and validated: $OutputPatch"
}
finally {
    if (-not $KeepWork -and (Test-Path -LiteralPath $work)) {
        Remove-Item -LiteralPath $work -Recurse -Force
    }
    elseif ($KeepWork) {
        Write-Host "Kept generator work directory: $work"
    }
}
