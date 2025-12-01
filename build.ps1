# 3_build.ps1 - Compiles and Runs (Fixed Path)
$ErrorActionPreference = "Stop"
Write-Host "[BUILD] STARTING BUILD PROCESS..." -ForegroundColor Cyan

# 1. Clean (Optional)
if (Test-Path "build") { 
    Write-Host "[CLEAN] Removing previous build..." -ForegroundColor Gray
    Remove-Item -Recurse -Force "build" 
}
New-Item -ItemType Directory -Force -Path "build" | Out-Null

# 2. Configure
Write-Host "[CMAKE] Configuring..." -ForegroundColor Yellow
cmake -B build -S . -DGEOMETRY_ENGINE_BUILD_TESTS=ON

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Configuration Failed." -ForegroundColor Red
    exit 1
}

# 3. Build
Write-Host "[COMPILE] compiling..." -ForegroundColor Yellow
cmake --build build --config Debug

# 4. Run
if ($LASTEXITCODE -eq 0) {
    Write-Host "[SUCCESS] Build Finished!" -ForegroundColor Green
    
    # Check the standard CMake output location (Visual Studio defaults to Source Structure)
    $exePath = "build\Frontend\Debug\Editor.exe"
    
    if (Test-Path $exePath) {
        Write-Host "[RUN] Launching Editor..." -ForegroundColor Cyan
        & $exePath
    } else {
        # Fallback check
        $altPath = "build\bin\Debug\Editor.exe"
        if (Test-Path $altPath) {
             Write-Host "[RUN] Launching Editor (Bin)..." -ForegroundColor Cyan
             & $altPath
        } else {
             Write-Host "[ERROR] Editor.exe found at neither:" -ForegroundColor Red
             Write-Host "   1. $exePath" -ForegroundColor Gray
             Write-Host "   2. $altPath" -ForegroundColor Gray
        }
    }
} else {
    Write-Host "[FAIL] Build Failed." -ForegroundColor Red
}