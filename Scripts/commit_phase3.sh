#!/usr/bin/env bash
# ==============================================================================
# Phase 3: Forensic Evidence & Contradiction Deduction Engine (4 Commits)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "🚀 Starting Phase 3 (Forensic Evidence & Contradiction Deduction) Commits..."

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

# Commit 3.1: Contradiction Widget Visual FX Delegates & Slot Management
echo "📦 [1/4] Committing Contradiction Deduction Declarations..."
git add \
    Source/Edrath/Public/UI/ForensicContradictionWidget.h
git commit -m "feat(deduction): declare chromatic visual feedback delegates and slot clearing in UForensicContradictionWidget" || echo "Nothing to commit for 3.1."

# Commit 3.2: Contradiction Resolution Logic & Visual FX Dispatch
echo "📦 [2/4] Committing Contradiction Resolution Implementation..."
git add \
    Source/Edrath/Private/UI/ForensicContradictionWidget.cpp
git commit -m "feat(deduction): implement visual FX dispatch and individual clue slot clearing in UForensicContradictionWidget" || echo "Nothing to commit for 3.2."

# Commit 3.3: Memory Echo Ghost Shader Tuning
echo "📦 [3/4] Committing Memory Echo Ghost Shader Tuning..."
git add \
    Shaders/MemoryEchoGhost.usf
git commit -m "feat(shaders): enhance temporal memory ghost glitch parameters for forensic contradiction breakthroughs" || echo "Nothing to commit for 3.3."

# Commit 3.4: Contradiction Deduction Automation Test Suite
echo "📦 [4/4] Committing Contradiction Deduction Automation Test..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp \
    Scripts/commit_phase3.sh
git commit -m "test(deduction): add forensic contradiction deduction and settlement loop break automation test" || echo "Nothing to commit for 3.4."

echo ""
echo "=============================================================================="
echo "✅ Phase 3 successfully committed (4 commits)!"
git log -n 4 --oneline
echo "=============================================================================="
