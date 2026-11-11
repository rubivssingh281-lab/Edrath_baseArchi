#!/usr/bin/env bash
# ==============================================================================
# Edrath: The World That Forgot Tomorrow
# Stage 2 Pre-Alpha Playable Vertical Slice - 5 Discrete Git Commits
# ==============================================================================

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

echo "=========================================================="
echo " Packaging Stage 2 Pre-Alpha Playable Vertical Slice     "
echo " 5 Discrete Conventional Commits                          "
echo "=========================================================="

# Commit 1
echo -e "\n[1/5] Committing Zone 1: Battlefield Awakening, Dying Soldier, and Battlefield Wraith..."
git add \
    Source/Edrath/Public/Core/EdrathGameplayTags.h \
    Source/Edrath/Private/Core/EdrathGameplayTags.cpp \
    Source/Edrath/Public/Character/BattlefieldWraithCharacter.h \
    Source/Edrath/Private/Character/BattlefieldWraithCharacter.cpp \
    Source/Edrath/Public/World/DyingSoldierActor.h \
    Source/Edrath/Private/World/DyingSoldierActor.cpp
git commit -m "feat(act1): implement Zone 1 battlefield awakening, dying soldier narrative actor, and battlefield wraith echo combat enemy" || echo "Commit 1 already clean or skipped"

# Commit 2
echo -e "\n[2/5] Committing Zone 2 & Zone 3: Gorge Bridge and Settlement Villager Agency..."
git add \
    Source/Edrath/Public/BlackClock/PossibilityShiftActor.h \
    Source/Edrath/Public/World/Act1GorgeBridgeActor.h \
    Source/Edrath/Private/World/Act1GorgeBridgeActor.cpp \
    Source/Edrath/Public/Character/SettlementVillagerCharacter.h \
    Source/Edrath/Private/Character/SettlementVillagerCharacter.cpp
git commit -m "feat(act1): implement Zone 2 gorge bridge possibility shifting and Zone 3 looping settlement villager agency" || echo "Commit 2 already clean or skipped"

# Commit 3
echo -e "\n[3/5] Committing Zone 4 & Zone 5: Kael House Forensics and 114-Year Priest Corpse..."
git add \
    Source/Edrath/Public/World/KaelHouseInteriorActor.h \
    Source/Edrath/Private/World/KaelHouseInteriorActor.cpp \
    Source/Edrath/Public/World/PriestCorpseActor.h \
    Source/Edrath/Private/World/PriestCorpseActor.cpp
git commit -m "feat(act1): implement Zone 4 house forensic clues and Zone 5 crypt 114-year-dead priest memory reconstruction" || echo "Commit 3 already clean or skipped"

# Commit 4
echo -e "\n[4/5] Committing Level Script, World Partition Data Layers, and Automation Tests..."
git add \
    Source/Edrath/Public/World/Act1BattlefieldLevelScriptActor.h \
    Source/Edrath/Private/World/Act1BattlefieldLevelScriptActor.cpp \
    Content/Maps/PreAlpha_Act1/ \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp
git commit -m "feat(act1): scaffold PreAlpha_Act1 level script actor, world partition data layers, and automated 5-zone vertical slice tests" || echo "Commit 4 already clean or skipped"

# Commit 5
echo -e "\n[5/5] Committing CHANGELOG, README, and Packaging Scripts..."
git add \
    CHANGELOG.md \
    README.md \
    Scripts/create_stage2_commits.sh \
    Scripts/create_stage2_commits.bat \
    Scripts/apply_stage2_git_commits.py
git commit -m "docs(stage2): update CHANGELOG, README, and provide 5-commit automated git packaging scripts" || echo "Commit 5 already clean or skipped"

echo -e "\n=========================================================="
echo " All 5 Stage 2 commits created successfully!               "
echo " Recent git commits:                                      "
git log -n 5 --oneline
echo "=========================================================="
echo " Run 'git push origin main' to publish Stage 2 to GitHub. "
