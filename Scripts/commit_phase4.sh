#!/usr/bin/env bash
# ==============================================================================
# Phase 4: Pre-Alpha Act 1 Five-Zone Continuity Polish (4 Commits)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "🚀 Starting Phase 4 (Act 1 Five-Zone Continuity) Commits..."

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

# Commit 4.1: Zone Completion Query Predicates
echo "📦 [1/4] Committing Zone Completion Predicate Declarations..."
git add \
    Source/Edrath/Public/World/Act1PreAlphaSliceDirector.h
git commit -m "feat(slice): declare zone completion query predicates in AAct1PreAlphaSliceDirector" || echo "Nothing to commit for 4.1."

# Commit 4.2: Possibility Dissolve Shader Synchronization
echo "📦 [2/4] Committing Possibility Dissolve Shader Synchronization..."
git add \
    Shaders/PossibilityDissolve.usf
git commit -m "feat(slice): synchronize Zone 2 Gorge Bridge Possibility Dissolve shader parameters" || echo "Nothing to commit for 4.2."

# Commit 4.3: Five-Zone Progression State Machine
echo "📦 [3/4] Committing Five-Zone Progression State Machine..."
git add \
    Source/Edrath/Private/World/Act1PreAlphaSliceDirector.cpp
git commit -m "feat(slice): refine five-zone progression state machine in AAct1PreAlphaSliceDirector" || echo "Nothing to commit for 4.3."

# Commit 4.4: Five-Zone Continuity Automation Test
echo "📦 [4/4] Committing Five-Zone Continuity Automation Test..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp \
    Scripts/commit_phase4.sh
git commit -m "test(slice): add Act 1 five-zone end-to-end continuity automation test" || echo "Nothing to commit for 4.4."

echo ""
echo "=============================================================================="
echo "✅ Phase 4 successfully committed (4 commits)!"
git log -n 4 --oneline
echo "=============================================================================="
