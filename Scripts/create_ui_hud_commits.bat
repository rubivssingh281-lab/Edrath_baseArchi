@echo off
REM ==============================================================================
REM Edrath: The World That Forgot Tomorrow
REM Windows Batch Script to stage and commit the 4 UI & HUD Architecture Commits
REM ==============================================================================

setlocal EnableDelayedExpansion

pushd "%~dp0\.."

echo ====================================================================
echo  Creating 4 Discrete Git Commits for UI ^& HUD Architecture
echo  Edrath: The World That Forgot Tomorrow
echo ====================================================================

REM Commit 1: Reticle & Inspection Prompt HUD
echo.
echo [1/4] Committing Dynamic Dual-Perspective Reticle ^& Inspection Prompt HUD...
git add Source\Edrath\Public\Core\EdrathTypes.h ^
        Source\Edrath\Public\UI\EdrathReticleWidget.h ^
        Source\Edrath\Private\UI\EdrathReticleWidget.cpp
git commit -m "feat(ui): add dynamic dual-perspective reticle and inspection prompt HUD" -m "- Implement UEdrathReticleWidget with dual-mode crosshair and cursor projection" -m "- Add isometric circular targeting reticle with ability radius telegraphing" -m "- Add over-the-shoulder third-person reticle with distance-scaled focus detection" -m "- Implement contextual inspection prompts for handwritten notes, portraits, and corpses" -m "- Add EEdrathReticleMode and deduction data structures to EdrathTypes.h"

REM Commit 2: Echo Combat Buffer HUD
echo.
echo [2/4] Committing Echo Combat 120-Frame Timeline Buffer HUD...
git add Source\Edrath\Public\UI\EchoCombatBufferWidget.h ^
        Source\Edrath\Private\UI\EchoCombatBufferWidget.cpp
git commit -m "feat(ui): implement Echo Combat 120-frame timeline buffer and cooldown gauge" -m "- Implement UEchoCombatBufferWidget visualizing circular combat action buffer" -m "- Display real-time buffer fullness ratio and 120-frame (2.0s) timeline duration" -m "- Track recent combat montage action history and strike damage values" -m "- Add Echo Strike projection delay slider (-1.2s default) and cooldown wheel" -m "- Implement regional resonance spectral theme coloring (Veyl, Hollowgreen, Nhal)"

REM Commit 3: Forensic Contradiction Deduction Board
echo.
echo [3/4] Committing Forensic Contradiction Deduction Board ^& Evidence Linking...
git add Source\Edrath\Public\UI\ForensicContradictionWidget.h ^
        Source\Edrath\Private\UI\ForensicContradictionWidget.cpp
git commit -m "feat(ui): implement Forensic Contradiction deduction board and evidence linking" -m "- Implement UForensicContradictionWidget for detective deduction gameplay" -m "- Support two-slot clue cross-referencing between witness statements and physical evidence" -m "- Register default deduction rules for Act 1 Crypt priest and Ilyra murder clues" -m "- Wire hypothesis evaluation to trigger Breakthrough narrative events" -m "- Automatically notify Narrative Subsystem to break Act 1 7-day settlement loop"

REM Commit 4: Master HUD Integration, Mobile Touch Controls & Automation Tests
echo.
echo [4/4] Committing Master HUD Integration, Mobile Touch Controls, Tests ^& Docs...
git add Source\Edrath\Public\UI\EdrathTouchControlsWidget.h ^
        Source\Edrath\Private\UI\EdrathTouchControlsWidget.cpp ^
        Source\Edrath\Public\UI\EdrathHUD.h ^
        Source\Edrath\Private\UI\EdrathHUD.cpp ^
        Source\Edrath\Private\Tests\EdrathPreAlphaAutomationTest.cpp ^
        CHANGELOG.md ^
        README.md ^
        Scripts\create_ui_hud_commits.sh ^
        Scripts\create_ui_hud_commits.bat
git commit -m "feat(ui): integrate master EdrathHUD, mobile touch controls, automation tests, and docs" -m "- Implement UEdrathTouchControlsWidget with virtual action buttons for mobile TBDR GPUs" -m "- Integrate Reticle, Echo Buffer, Contradiction Board, and Touch overlays into AEdrathHUD" -m "- Wire input mode switching and focus locking between gameplay and inspection panels" -m "- Add FEdrathFullUIAndHUDArchitectureVerificationTest automation suite" -m "- Update CHANGELOG.md to 0.4.0 and document 4-commit milestone in README.md"

echo.
echo ====================================================================
echo  All 4 UI ^& HUD Architecture Commits Created Successfully!
echo  Run 'git log -4 --oneline' to view the commits.
echo  Run 'git push origin main' to push upstream.
echo ====================================================================

popd
