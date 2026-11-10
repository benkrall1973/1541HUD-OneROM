[CmdletBinding()]
param([string]$OutputDirectory = '.buildtools/windows-cli-package')
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
Set-Location $root
function Check-Exit { if ($LASTEXITCODE -ne 0) { throw "Command failed with exit code $LASTEXITCODE" } }
$core = Join-Path $root '.buildtools/windows-cli-core'
New-Item -ItemType Directory -Force -Path $core, $OutputDirectory | Out-Null
$output = (Resolve-Path $OutputDirectory).Path
$tar = Join-Path $root '.buildtools/windows-cli-source.tar'
git archive HEAD:source/one-rom-v0.8.1 --format=tar --output=$tar
Check-Exit
tar -xf $tar -C $core
Check-Exit
bash tools/maintenance/Apply-Host-Dependencies.sh ($core.Replace('\','/'))
Check-Exit
Copy-Item tools/maintenance/tests/host_dependency_zip.rs (Join-Path $core 'rust/fw/tests/hud_host_dependency_zip.rs')
python tools/maintenance/Prepare-Host-Fixtures.py $core
Check-Exit
$env:HUD_ZIP_FIXTURES = Join-Path $core 'rust/fw/tests/hud-generated-fixtures'
Push-Location (Join-Path $core 'rust')
try {
    cargo test --locked --release -p onerom-fw
    Check-Exit
    cargo test --locked --release -p onerom-cli --bins
    Check-Exit
    cargo test --locked --release -p onerom-cli --test image_convert --test image_transform --test general --test args --test scan
    Check-Exit
    cargo build --locked --release -p onerom-cli
    Check-Exit
} finally { Pop-Location }
$exe = Join-Path $output 'onerom-v0.6.0-hud1.exe'
Copy-Item (Join-Path $core 'rust/target/release/onerom.exe') $exe
& $exe --version
Check-Exit
# Inspect both published images offline. No USB scan, program or reboot occurs.
foreach ($image in @('releases/onerom-1541HUD-Control-UB3-fire-24-g-stock-test-v0.8.1.bin', 'releases/onerom-1541HUD-Monitor-UB4-fire-24-g-v0.8.1.32.bin')) {
    & $exe firmware inspect --firmware $image
    Check-Exit
}
$record = [ordered]@{
    name = '1541HUD Windows CLI'; upstream_cli = '0.6.0'; distribution = 'hud1'
    source_commit = (git rev-parse HEAD); upstream = '1b189260d85a5feced19e969399986f5f1d1cabe'
    rustc = (rustc --version); cargo = (cargo --version); target = 'x86_64-pc-windows-msvc'
    overlay_sha256 = (Get-FileHash overlays/host-dependencies-v0.8.1.patch -Algorithm SHA256).Hash
    lock_sha256 = (Get-FileHash (Join-Path $core 'rust/Cargo.lock') -Algorithm SHA256).Hash
    file = (Split-Path $exe -Leaf); bytes = (Get-Item $exe).Length
    sha256 = (Get-FileHash $exe -Algorithm SHA256).Hash
    scope = 'Windows release build, native CLI/ZIP regressions and offline inspection of both published images; no hardware programmed.'
}
$record | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $output 'windows-cli-manifest.json') -Encoding utf8
Write-Output "PASS: Windows CLI built and tested: $exe"
