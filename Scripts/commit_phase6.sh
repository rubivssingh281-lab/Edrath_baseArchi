#!/usr/bin/env bash
# ==============================================================================
# Phase 6: Comprehensive Automation & Invariant Validation (4 Commits)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "🚀 Starting Phase 6 (Comprehensive Automation & Invariant Validation) Commits..."

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

# Commit 6.1: Changelog Documentation
echo "📦 [1/4] Committing Changelog Documentation..."
git add \
    CHANGELOG.md
git commit -m "docs(changelog): document 6-phase master polish features and architectural upgrades in CHANGELOG.md" || echo "Nothing to commit for 6.1."

# Commit 6.2: README Architecture & Execution Guide
echo "📦 [2/4] Committing README Architecture Matrix..."
git add \
    README.md
git commit -m "docs(architecture): add 6-phase master polish summary matrix and execution guide to README.md" || echo "Nothing to commit for 6.2."

# Commit 6.3: Automation Test Suite Consolidation
echo "📦 [3/4] Committing Multi-Phase Automation Verification Suite..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp
git commit -m "test(automation): consolidate end-to-end multi-phase automated invariant validation suite" || echo "Nothing to commit for 6.3."

# Commit 6.4: Phase 6 and Master Orchestration Scripts
echo "📦 [4/4] Committing Commit Orchestrators..."
git add \
    Scripts/commit_phase6.sh \
    Scripts/commit_all_phases.sh
git commit -m "chore(scripts): create Phase 6 commit script and master commit_all_phases orchestrator" || echo "Nothing to commit for 6.4."

echo ""
echo "=============================================================================="
echo "✅ Phase 6 successfully committed (4 commits)!"
git log -n 4 --oneline
echo "=============================================================================="
