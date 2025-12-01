<#
    GEOMETRY ENGINE BUILD SCRIPT
    Usage:
      .\build.ps1          -> Incremental build (Fast)
      .\build.ps1 -Clean   -> Full wipe and rebuild (Slow)
#>

param (
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

# --- 1. CLEANUP (Optional) ---
if ($Clean) {
    if (Test-Path "build") { 
        Write-Host "[CLEAN] Removing previous build artifact..." -ForegroundColor Gray
        Remove-Item -Recurse -Force "build" 
    }
}

# Ensure build directory exists
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Force -Path "build" | Out-Null
}

# --- 2. CONFIGURE (CMake) ---
Write-Host "[CMAKE] Configuring Project..." -ForegroundColor Yellow
# We explicitly set the install prefix/output bin to keep things tidy
cmake -B build -S . -DGEOMETRY_ENGINE_BUILD_TESTS=ON

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Configuration Failed." -ForegroundColor Red
    exit 1
}

# --- 3. COMPILE ---
Write-Host "[COMPILE] Building Targets..." -ForegroundColor Yellow
# --parallel uses all CPU cores for faster builds
cmake --build build --config Debug --parallel 

if ($LASTEXITCODE -ne 0) {
    Write-Host "[FAIL] Build Failed." -ForegroundColor Red
    exit 1
}

# --- 4. LAUNCHER MENU ---
Write-Host "[SUCCESS] Build Finished!" -ForegroundColor Green

# Define paths (Adjust these if you haven't set CMAKE_RUNTIME_OUTPUT_DIRECTORY yet)
# Assumption: You are using the standard bin/ output or Frontend/ folder.
$pathEditor  = "build\bin\Debug\Editor.exe"
$pathSandbox = "build\bin\Debug\UISandbox.exe"

# Fallback check for default CMake behavior if bin/ doesn't exist
if (-not (Test-Path $pathEditor)) { $pathEditor = "build\Frontend\Debug\Editor.exe" }
if (-not (Test-Path $pathSandbox)) { $pathSandbox = "build\Frontend\Debug\UISandbox.exe" }

function Show-Menu {
    Clear-Host
    Write-Host "=========================================" -ForegroundColor Cyan
    Write-Host "       GEOMETRY ENGINE - LAUNCHER        " -ForegroundColor Cyan
    Write-Host "=========================================" -ForegroundColor Cyan
    Write-Host ""
    
    # Check if files exist to color code the menu
    if (Test-Path $pathEditor) {
        Write-Host "  [1] Editor   " -NoNewline -ForegroundColor Green
        Write-Host "(Full Engine Backend + UI)" -ForegroundColor White
    } else {
        Write-Host "  [1] Editor   (Not Found)" -ForegroundColor Red
    }

    if (Test-Path $pathSandbox) {
        Write-Host "  [2] Sandbox  " -NoNewline -ForegroundColor Yellow
        Write-Host "(Rapid UI Iteration)" -ForegroundColor White
    } else {
        Write-Host "  [2] Sandbox  (Not Found)" -ForegroundColor Red
    }
    
    Write-Host "  [Q] Quit" -ForegroundColor Gray
    Write-Host ""
}

# Loop until quit
while ($true) {
    Show-Menu
    $choice = Read-Host "  Select Option"

    switch ($choice) {
        "1" { 
            if (Test-Path $pathEditor) { 
                Write-Host "Launching Editor..." -ForegroundColor Green
                & $pathEditor 
                exit 0
            } else { Write-Host "Error: Editor not found." -ForegroundColor Red; Pause }
        }
        "2" { 
            if (Test-Path $pathSandbox) { 
                Write-Host "Launching Sandbox..." -ForegroundColor Yellow
                & $pathSandbox 
                exit 0
            } else { Write-Host "Error: Sandbox not found." -ForegroundColor Red; Pause }
        }
        "Q" { exit 0 }
        "q" { exit 0 }
        default { Write-Host "Invalid selection." -ForegroundColor Red; Start-Sleep -Seconds 1 }
    }
}