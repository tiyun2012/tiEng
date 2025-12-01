# ui_dev.ps1 - Watches UI files and rebuilds only sandbox
param([switch]$Watch = $false)

$ErrorActionPreference = "Stop"

$uiFiles = @(
    "UI/UILayouts.h",
    "UI/Components/*.h",
    "UI/Core/*.h",
    "WindowSetup.h"
)
$lastBuildTime = $null

function Build-Sandbox {
    Write-Host "[BUILD] Building UI Sandbox..." -ForegroundColor Cyan
    cmake --build build --target UISandbox --config Debug --parallel
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[SUCCESS] Sandbox built!" -ForegroundColor Green
        $script:lastBuildTime = Get-Date
        return $true
    } else {
        Write-Host "[FAILED] Build failed" -ForegroundColor Red
        return $false
    }
}

function Launch-Sandbox {
    $exePath = "build/bin/Debug/UISandbox.exe"
    if (-not (Test-Path $exePath)) {
        $exePath = "build/Frontend/Debug/UISandbox.exe"
    }
    
    if (Test-Path $exePath) {
        Write-Host "[LAUNCH] Starting Sandbox..." -ForegroundColor Yellow
        Start-Process $exePath -NoNewWindow
    } else {
        Write-Host "[ERROR] Sandbox executable not found" -ForegroundColor Red
    }
}

# Initial build
if (Build-Sandbox) {
    Launch-Sandbox
    
    if ($Watch) {
        Write-Host "[WATCH] Monitoring UI files for changes..." -ForegroundColor Cyan
        Write-Host "        Press Ctrl+C to stop" -ForegroundColor Gray
        
        try {
            while ($true) {
                $filesChanged = $false
                foreach ($file in $uiFiles) {
                    if (Test-Path $file) {
                        $fileTime = (Get-Item $file).LastWriteTime
                        if ($null -eq $lastBuildTime -or $fileTime -gt $lastBuildTime) {
                            $filesChanged = $true
                            Write-Host "[CHANGE] $file modified" -ForegroundColor Yellow
                            break
                        }
                    }
                }
                
                if ($filesChanged) {
                    Write-Host "[REBUILD] Changes detected, rebuilding..." -ForegroundColor Cyan
                    if (Build-Sandbox) {
                        Launch-Sandbox
                    }
                }
                
                Start-Sleep -Seconds 1
            }
        } finally {
            Write-Host "[STOP] File watcher stopped" -ForegroundColor Gray
        }
    }
}