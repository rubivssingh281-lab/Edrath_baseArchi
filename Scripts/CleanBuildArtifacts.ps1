# Edrath: The World That Forgot Tomorrow
# Disk Space Reclamation Script for 512GB Laptop SSD
# Cleans transient build artifacts, cooked platform packages, and logs.

param (
    [switch]$DeepClean = $false
)

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "   Edrath: SSD Disk Space Optimizer                      " -ForegroundColor Yellow
Write-Host "   Reclaiming space on 512GB development drive...         " -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

$ProjectRoot = (Resolve-Path "$PSScriptRoot\..").Path
$PathsToClean = @(
    (Join-Path $ProjectRoot "Saved\Logs"),
    (Join-Path $ProjectRoot "Saved\Crashes"),
    (Join-Path $ProjectRoot "Saved\ShaderCache"),
    (Join-Path $ProjectRoot "Saved\Cooked")
)

if ($DeepClean) {
    Write-Warning "Deep Clean requested: Adding Intermediate and Binaries folders to cleanup..."
    $PathsToClean += (Join-Path $ProjectRoot "Intermediate")
    $PathsToClean += (Join-Path $ProjectRoot "Binaries\Win64")
}

$TotalReclaimedBytes = 0

foreach ($Path in $PathsToClean) {
    if (Test-Path $Path) {
        try {
            $SizeBytes = (Get-ChildItem -Path $Path -Recurse -File -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
            if ($null -ne $SizeBytes) {
                $TotalReclaimedBytes += $SizeBytes
                $SizeMB = [math]::Round($SizeBytes / 1MB, 2)
                Write-Host "Removing $Path ($SizeMB MB)..." -ForegroundColor Gray
            }
            Remove-Item -Path $Path -Recurse -Force -ErrorAction SilentlyContinue
            Write-Host "[OK] Cleaned: $Path" -ForegroundColor Green
        } catch {
            Write-Warning "Could not delete $Path (files may be in use by Unreal Engine)."
        }
    }
}

$TotalMB = [math]::Round($TotalReclaimedBytes / 1MB, 2)
$TotalGB = [math]::Round($TotalReclaimedBytes / 1GB, 2)

Write-Host "`n==========================================================" -ForegroundColor Cyan
Write-Host "   Total Disk Space Reclaimed: $TotalMB MB ($TotalGB GB)" -ForegroundColor Green
Write-Host "==========================================================" -ForegroundColor Cyan
