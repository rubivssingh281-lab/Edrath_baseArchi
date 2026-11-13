#!/usr/bin/env bash
# ==============================================================================
# Phase 2: Echo Combat & Timeline Sub-Frame Interpolation (4 Commits)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "🚀 Starting Phase 2 (Echo Combat & Sub-Frame Interpolation) Commits..."

if [[ ! -d ".git" ]]; then
    echo "Error: Not a git repository!" >&2
    exit 1
fi

if ! git config user.name >/dev/null 2>&1; then
    git config user.name "rubivssingh281"
fi
if ! git config user.email >/dev/null 2>&1; then
    git config user.email "rubivssingh281@gmail.com"
fi

# Commit 2.1: Black Clock Sub-Frame Interpolation Engine
echo "📦 [1/4] Committing Sub-Frame Interpolation Engine..."
git add \
    Source/Edrath/Public/BlackClock/BlackClockComponent.h \
    Source/Edrath/Private/BlackClock/BlackClockComponent.cpp
git commit -m "feat(blackclock): implement sub-frame linear and quaternion Hermite interpolation in UBlackClockComponent" || echo "Nothing to commit for 2.1."

# Commit 2.2: Echo Combat Ghost Strike Physics Sweep
echo "📦 [2/4] Committing Ghost Strike Physics Sweep Detection..."
git add \
    Source/Edrath/Public/BlackClock/EchoCombatComponent.h
git commit -m "feat(combat): declare physics sweep multi-hit detection and delegates for echo ghost strikes" || echo "Nothing to commit for 2.2."

# Commit 2.3: Echo Combat Parry Stability Reward & Collision Sweep Execution
echo "📦 [3/4] Committing Parry Stability Reward & Sweep Execution..."
git add \
    Source/Edrath/Private/BlackClock/EchoCombatComponent.cpp
git commit -m "feat(combat): implement ghost strike collision sweep execution and parry stability restoration" || echo "Nothing to commit for 2.3."

# Commit 2.4: Echo Combat Automation Test Suite
echo "📦 [4/4] Committing Sub-Frame Interpolation & Parry Automation Test..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp \
    Scripts/commit_phase2.sh
git commit -m "test(combat): add echo combat sub-frame interpolation and parry stability automation test" || echo "Nothing to commit for 2.4."

echo ""
echo "=============================================================================="
echo "✅ Phase 2 successfully committed (4 commits)!"
git log -n 4 --oneline
echo "=============================================================================="
