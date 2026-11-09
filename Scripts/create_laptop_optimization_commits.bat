@echo off
REM ==============================================================================
REM Edrath: The World That Forgot Tomorrow
REM Windows Batch script to stage and commit the 4 Laptop Optimization Commits
REM Target: RTX 5070 8GB | Ryzen 7 253 | 16GB DDR5 | 512GB NVMe M.2
REM ==============================================================================

cd /d "%~dp0\.."

echo ====================================================================
echo  Creating 4 Discrete Git Commits for Laptop Optimization            
echo  Target: RTX 5070 8GB ^| Ryzen 7 253 ^| 16GB DDR5 ^| 512GB NVMe M.2    
echo ====================================================================

echo.
echo [1/4] Committing Config Tuning for RTX 5070 8GB Laptop Development...
git add Config/DefaultEngine.ini Config/DefaultScalability.ini Config/DefaultEditor.ini Config/DefaultEditorPerProjectUserSettings.ini
git commit -m "config: tune engine and scalability configs for RTX 5070 8GB laptop development"

echo.
echo [2/4] Committing Development_Laptop Quality Preset ^& Hardware Auto-Detection...
git add Source/Edrath/Public/Core/EdrathTypes.h Source/Edrath/Public/Core/EdrathScalabilitySubsystem.h Source/Edrath/Private/Core/EdrathScalabilitySubsystem.cpp Source/Edrath/Private/Core/EdrathGameInstance.cpp Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp
git commit -m "feat(core): add Development_Laptop quality preset with VRAM-aware auto-detection"

echo.
echo [3/4] Committing Hardware Optimization Subsystem with VRAM Budget Management...
git add Source/Edrath/Public/Core/EdrathHardwareOptimizationSubsystem.h Source/Edrath/Private/Core/EdrathHardwareOptimizationSubsystem.cpp
git commit -m "feat(perf): rewrite hardware optimization subsystem with VRAM budget management"

echo.
echo [4/4] Committing Laptop Development Optimization Guide...
git add Documentation/Laptop_Development_Optimization_Guide.md Scripts/create_laptop_optimization_commits.sh Scripts/create_laptop_optimization_commits.bat README.md
git commit -m "docs: add laptop development guide and optimization reference"

echo.
echo ====================================================================
echo  All 4 commits created successfully!
echo ====================================================================
git log -n 4 --oneline
echo ====================================================================
echo  Ready! Run 'git push origin main' to push all 4 commits.
