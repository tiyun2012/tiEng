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
                Start-Process $pathEditor 
                exit 0
            } else { Write-Host "Error: Editor not found." -ForegroundColor Red; Pause }
        }
        "2" { 
            if (Test-Path $pathSandbox) { 
                Write-Host "Launching Sandbox..." -ForegroundColor Yellow
                Start-Process $pathSandbox 
                exit 0
            } else { Write-Host "Error: Sandbox not found." -ForegroundColor Red; Pause }
        }
        "Q" { exit 0 }
        "q" { exit 0 }
        default { Write-Host "Invalid selection." -ForegroundColor Red; Start-Sleep -Seconds 1 }
    }
}