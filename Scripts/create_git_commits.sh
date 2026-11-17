#!/usr/bin/env bash
# ==============================================================================
# Edrath: The World That Forgot Tomorrow
# 3-Commit Automated Git Packaging Script
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$PROJECT_ROOT"

echo "=========================================================="
echo "   Edrath: Automated 3-Commit Packaging Pipeline          "
echo "=========================================================="

# Ensure Git is available
if ! command -v git >/dev/null 2>&1; then
    echo "[ERROR] Git is not installed or not in PATH."
    exit 1
fi

# ------------------------------------------------------------------------------
# Commit 1: Performance, VRAM Budgets & PSO Precaching
# ------------------------------------------------------------------------------
echo -e "\n[1/3] Packaging Commit 1: Performance & Rendering Optimization..."
git add \
    Config/DefaultEngine.ini \
    Config/Android/AndroidEngine.ini \
    Source/Edrath/Public/Core/EdrathHardwareOptimizationSubsystem.h \
    Source/Edrath/Private/Core/EdrathHardwareOptimizationSubsystem.cpp

git commit -m "perf(engine): optimize rendering pipeline, VRAM budgets, and PSO caching for mobile and laptop targets" \
    -m "- Implement dynamic thermal mitigation (Nominal, Fair, Serious, Critical) adjusting frame rate caps and shadow resolution
- Implement RecoverEmergencyVRAM() purging overbudget mips and virtual shadow map allocations
- Configure UE5.5 Pipeline State Object (PSO) precaching (r.PSOPrecache=1) to prevent transition hitching
- Tune asynchronous loading threads (s.AsyncLoadingThreadEnabled=1) and aggressive GC clustering (gc.TimeBetweenPurgingPendingKillObjects=30)
- Add Vulkan pipeline caching and mobile memory compaction overrides in AndroidEngine.ini" || echo "[INFO] Commit 1 already applied or nothing to commit."

# ------------------------------------------------------------------------------
# Commit 2: Act 3 Hollowgreen Memory Realms & World Mutation Anchors
# ------------------------------------------------------------------------------
echo -e "\n[2/3] Packaging Commit 2: Act 3 Hollowgreen Architecture..."
git add \
    Source/Edrath/Public/Narrative/MemoryAnchorActor.h \
    Source/Edrath/Private/Narrative/MemoryAnchorActor.cpp \
    Source/Edrath/Public/World/Act3HollowgreenDirector.h \
    Source/Edrath/Private/World/Act3HollowgreenDirector.cpp

git commit -m "feat(world): implement Act 3 Hollowgreen physical memory realms, fruit harvest streaming, and world mutation anchors" \
    -m "- Implement AMemoryAnchorActor providing interactive memory anchors that trigger UEdrathNarrativeSubsystem::RecordForestMutation()
- Implement AAct3HollowgreenDirector orchestrating Act III (The Forest of Your Other Lives)
- Track exploration across the three alternate life echoes (Plagued Healer, Tyrant Conqueror, Simple Farmer)
- Synchronize Kael's modular rig outfit presets with active memory echoes
- Implement Ilyra confrontation state machine unlocking loop attempt truth (Story.Act3.LoopAttemptsRevealed)" || echo "[INFO] Commit 2 already applied or nothing to commit."

# ------------------------------------------------------------------------------
# Commit 3: Automation Tests & Documentation
# ------------------------------------------------------------------------------
echo -e "\n[3/3] Packaging Commit 3: Automation Tests & Documentation..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp \
    CHANGELOG.md \
    README.md \
    Scripts/create_git_commits.sh

git commit -m "test(automation): add Act 3 memory realm and hardware optimization validation test suites with v0.9.0 documentation" \
    -m "- Add FEdrathAct3HollowgreenMemoryRealmTest validating memory fruit harvesting, anchor mutations, and Ilyra loop confrontation
- Add FEdrathEnginePerformanceOptimizationTest verifying emergency VRAM recovery thresholds, thermal mitigation, and PSO precaching
- Document version 0.9.0 release in CHANGELOG.md
- Update README.md with 8-Phase polish architecture and test matrix
- Add automated git commit script Scripts/create_git_commits.sh" || echo "[INFO] Commit 3 already applied or nothing to commit."

echo -e "\n=========================================================="
echo "   All 3 commits created successfully!"
echo "=========================================================="
git log -n 3 --oneline
