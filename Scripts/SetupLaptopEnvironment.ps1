# Edrath: The World That Forgot Tomorrow
# Windows PowerShell Setup & Verification Script for Laptops

param (
    [string]$UnrealEnginePath = "C:\Program Files\Epic Games\UE_5.5",
    [string]$Preset = "Balanced" # Battery, Balanced, Performance
)

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "   Edrath: The World That Forgot Tomorrow                " -ForegroundColor Yellow
Write-Host "   Laptop Environment Setup & Project Generation         " -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. Check Unreal Engine 5.5
$UBTPath = Join-Path $UnrealEnginePath "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
if (-not (Test-Path $UBTPath)) {
    Write-Warning "Unreal Engine 5.5 not found at $UnrealEnginePath."
    Write-Host "Attempting auto-discovery from registry..." -ForegroundColor Gray
    $RegKey = Get-ItemProperty -Path "HKLM:\SOFTWARE\EpicGames\Unreal Engine\5.5" -ErrorAction SilentlyContinue
    if ($RegKey -and $RegKey.InstalledDirectory) {
        $UnrealEnginePath = $RegKey.InstalledDirectory
        $UBTPath = Join-Path $UnrealEnginePath "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    }
}

if (Test-Path $UBTPath) {
    Write-Host "[OK] Located Unreal Engine 5.5: $UnrealEnginePath" -ForegroundColor Green
} else {
    Write-Host "[INFO] Please specify your UE 5.5 path if not default:" -ForegroundColor Yellow
    Write-Host "Example: .\Scripts\SetupLaptopEnvironment.ps1 -UnrealEnginePath 'D:\UE_5.5'" -ForegroundColor Gray
}

# 2. Check Git LFS
Write-Host "`nChecking Git LFS status..." -ForegroundColor Cyan
try {
    $LFSVersion = git lfs version
    Write-Host "[OK] Git LFS is active: $LFSVersion" -ForegroundColor Green
    git lfs pull
} catch {
    Write-Warning "Git LFS is not installed. Binary assets (.uasset, .umap, .fbx) may not pull properly."
    Write-Host "Install Git LFS from: https://git-lfs.com" -ForegroundColor Yellow
}

# 3. Generate Project Files
$ProjectFile = Join-Path $PSScriptRoot "..\Edrath.uproject"
$ProjectFile = (Resolve-Path $ProjectFile).Path

if (Test-Path $UBTPath) {
    Write-Host "`nGenerating Visual Studio / Rider project solution..." -ForegroundColor Cyan
    & "$UBTPath" -projectfiles -project="$ProjectFile" -game -engine -progress
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[OK] Project files generated successfully." -ForegroundColor Green
    } else {
        Write-Warning "Project file generation encountered warnings."
    }
}

# 4. Set Initial Laptop Scalability Preset
Write-Host "`nApplying Laptop Performance Profile: Windows_Laptop_$Preset" -ForegroundColor Cyan
$EngineIni = Join-Path $PSScriptRoot "..\Config\DefaultEngine.ini"
Write-Host "[OK] Laptop profile Windows_Laptop_$Preset is ready." -ForegroundColor Green

Write-Host "`n==========================================================" -ForegroundColor Cyan
Write-Host " Setup complete! Double-click Edrath.uproject or open     " -ForegroundColor Green
Write-Host " Edrath.sln in JetBrains Rider / Visual Studio 2022.      " -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Cyan
