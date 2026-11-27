# Edrath: The World That Forgot Tomorrow
# Unreal Engine 5.5+ Structural, Reflection, IWYU & Lifecycle Integrity Validator

param (
    [string]$ProjectRoot = (Resolve-Path "$PSScriptRoot\..").Path
)

$ErrorActionPreference = "Stop"
$Failures = 0

function Report-Error {
    param([string]$Message)
    Write-Host "[FAIL] $Message" -ForegroundColor Red
    $script:Failures++
}

function Report-Ok {
    param([string]$Message)
    Write-Host "[OK]   $Message" -ForegroundColor Green
}

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "   Edrath Architectural & UE 5.5+ Integrity Check         " -ForegroundColor Yellow
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. Core Project Descriptors
foreach ($RequiredFile in @("Edrath.uproject", "Source/Edrath/Edrath.Build.cs", ".gitattributes")) {
    $FullPath = Join-Path $ProjectRoot $RequiredFile
    if (-not (Test-Path $FullPath)) {
        Report-Error "Missing required project file: $RequiredFile"
    } else {
        Report-Ok "Found required file: $RequiredFile"
    }
}

# 2. UE5 Reflection & generated.h Ordering + IWYU Guards
$PublicRoot = Join-Path $ProjectRoot "Source/Edrath/Public"
$PrivateRoot = Join-Path $ProjectRoot "Source/Edrath/Private"
$ModuleRoot = Join-Path $ProjectRoot "Source/Edrath"

$Headers = Get-ChildItem -Path $ModuleRoot -Recurse -Filter "*.h"
foreach ($Header in $Headers) {
    $Content = Get-Content -Path $Header.FullName
    $RawText = $Content -join "`n"

    # Reflection macro check
    if ($RawText -match '\b(UENUM|USTRUCT|UCLASS|UPROPERTY|UFUNCTION)\b') {
        $GenIncludes = $Content | Select-String -Pattern '#include\s+"[A-Za-z0-9_]+\.generated\.h"'
        if ($GenIncludes.Count -eq 0) {
            Report-Error "Missing *.generated.h in reflection header: $($Header.FullName)"
        } elseif ($GenIncludes.Count -gt 1) {
            Report-Error "Multiple *.generated.h includes in: $($Header.FullName)"
        } else {
            $GenLine = $GenIncludes[0].LineNumber
            $AllIncludes = $Content | Select-String -Pattern '^\s*#include\s+'
            foreach ($Inc in $AllIncludes) {
                if ($Inc.LineNumber -gt $GenLine) {
                    Report-Error "Include appears after *.generated.h in $($Header.FullName):$($Inc.LineNumber) -> $($Inc.Line)"
                }
            }
        }
    }

    # IWYU check for NativeGameplayTags.h
    if ($RawText -match 'UE_DECLARE_GAMEPLAY_TAG_EXTERN' -and $RawText -notmatch '#include\s+"NativeGameplayTags\.h"') {
        Report-Error "Missing #include `"NativeGameplayTags.h`" in $($Header.FullName)"
    }

    # IWYU check for Engine/HitResult.h in public UHT headers referencing FHitResult
    if ($Header.FullName.StartsWith($PublicRoot) -and $RawText -match '\bFHitResult\b' -and $RawText -notmatch '#include\s+"Engine/HitResult\.h"') {
        Report-Error "Missing #include `"Engine/HitResult.h`" for FHitResult in $($Header.FullName)"
    }
}
Report-Ok "Validated $($Headers.Count) C++ header files for UHT reflection and IWYU ordering."

# 3. Verify all local project subdirectory includes resolve on disk
$AllCppAndH = Get-ChildItem -Path $ModuleRoot -Recurse -Include *.h,*.cpp
$ProjectSubdirs = @("BlackClock/", "Camera/", "Character/", "Core/", "Narrative/", "Tests/", "UI/", "World/")
$EngineWhitelist = @("Camera/CameraComponent.h", "Camera/CameraActor.h", "Camera/PlayerCameraManager.h")
$UnresolvedCount = 0
foreach ($File in $AllCppAndH) {
    $Lines = Get-Content -Path $File.FullName
    foreach ($Line in $Lines) {
        if ($Line -match '^\s*#include\s+"([^"]+)"') {
            $IncPath = $Matches[1]
            if ($EngineWhitelist -contains $IncPath) {
                continue
            }
            foreach ($Prefix in $ProjectSubdirs) {
                if ($IncPath.StartsWith($Prefix)) {
                    $Exists = (Test-Path (Join-Path $PublicRoot $IncPath)) -or (Test-Path (Join-Path $PrivateRoot $IncPath))
                    if (-not $Exists) {
                        Report-Error "Unresolved project include '$IncPath' in $($File.FullName)"
                        $UnresolvedCount++
                    }
                }
            }
        }
    }
}
if ($UnresolvedCount -eq 0) {
    Report-Ok "All project-relative #include paths resolved on disk."
}

# 4. Placeholder / TODO Audit
$CodeFiles = Get-ChildItem -Path $ModuleRoot,(Join-Path $ProjectRoot "Shaders"),(Join-Path $ProjectRoot "Config") -Recurse -File
$PlaceholderHits = $CodeFiles | Select-String -Pattern '\b(TODO|FIXME|PLACEHOLDER|NOT IMPLEMENTED)\b'
if ($PlaceholderHits.Count -gt 0) {
    foreach ($Hit in $PlaceholderHits) {
        Report-Error "Placeholder/TODO marker found at $($Hit.Path):$($Hit.LineNumber)"
    }
} else {
    Report-Ok "Zero TODO/FIXME/PLACEHOLDER markers found in Source/, Shaders/, or Config/."
}

# 5. External Subsystem Delegate Cleanup Audit
$SubsystemSubscribeFiles = Get-ChildItem -Path $PrivateRoot -Recurse -Filter "*.cpp" | ForEach-Object {
    $Text = Get-Content $_.FullName -Raw
    if ($Text -match '(GetNarrativeSubsystem|GetGameInstance|Subsystem|DualCameraComponent|OnClicked).*Add(Unique)?Dynamic' -and $Text -notmatch 'RemoveDynamic') {
        $_.FullName
    }
}
if ($SubsystemSubscribeFiles.Count -gt 0) {
    foreach ($F in $SubsystemSubscribeFiles) {
        Report-Error "External delegate binding without RemoveDynamic cleanup in: $F"
    }
} else {
    Report-Ok "All external subsystem/widget/UI delegate subscriptions have matching RemoveDynamic cleanup."
}

# 6. LF Line Endings Check (matching .gitattributes *.h/*.cpp/*.cs/*.ini/*.usf eol=lf)
$CrlfViolations = @()
foreach ($F in $CodeFiles) {
    $Bytes = [System.IO.File]::ReadAllBytes($F.FullName)
    for ($i = 0; $i -lt $Bytes.Length - 1; $i++) {
        if ($Bytes[$i] -eq 13 -and $Bytes[$i + 1] -eq 10) {
            $CrlfViolations += $F.FullName
            break
        }
    }
}
if ($CrlfViolations.Count -gt 0) {
    foreach ($V in $CrlfViolations) {
        Report-Error "CRLF line endings found (expected LF per .gitattributes): $V"
    }
} else {
    Report-Ok "All Source/, Shaders/, and Config/ files conform to LF line endings."
}

# 7. Duplicate UHT Reflected Symbol Check
$ReflectedSymbols = @{}
$DuplicateCount = 0
foreach ($Header in $Headers) {
    $RawText = (Get-Content -Path $Header.FullName) -join "`n"
    $Matches = [regex]::Matches($RawText, '(?s)\b(?:UENUM|USTRUCT|UCLASS)\s*\([^\)]*\)\s*(?:enum\s+class|struct|class)\s+(?:EDRATH_API\s+)?([A-Za-z_][A-Za-z0-9_]*)')
    foreach ($M in $Matches) {
        $SymbolName = $M.Groups[1].Value
        if ($ReflectedSymbols.ContainsKey($SymbolName)) {
            Report-Error "Duplicate UHT symbol '$SymbolName' in $($Header.FullName) (already declared in $($ReflectedSymbols[$SymbolName]))"
            $DuplicateCount++
        } else {
            $ReflectedSymbols[$SymbolName] = $Header.FullName
        }
    }
}
if ($DuplicateCount -eq 0) {
    Report-Ok "Verified $($ReflectedSymbols.Count) unique UHT-reflected symbols with zero collisions."
}

# 8. Git LFS Tracking Policy Check
$GitAttrPath = Join-Path $ProjectRoot ".gitattributes"
$GitAttrRaw = Get-Content -Path $GitAttrPath -Raw
foreach ($Rule in @('\*\.uasset.*lockable', '\*\.umap.*lockable', '\*\.fbx filter=lfs', '\*\.wav filter=lfs')) {
    if ($GitAttrRaw -notmatch $Rule) {
        Report-Error "Missing Git LFS rule matching '$Rule' in .gitattributes"
    }
}
Report-Ok "Git LFS lockable asset tracking rules verified."

Write-Host "`n==========================================================" -ForegroundColor Cyan
if ($Failures -eq 0) {
    Write-Host "   INTEGRITY CHECK PASSED (0 ERRORS)                     " -ForegroundColor Green
    exit 0
} else {
    Write-Host "   INTEGRITY CHECK FAILED ($Failures ERRORS)              " -ForegroundColor Red
    exit 1
}
