#!/usr/bin/env bash
# ==============================================================================
# Phase 5: 8GB VRAM Guardian & Rendering Polish (4 Commits)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "🚀 Starting Phase 5 (8GB VRAM Guardian & Rendering Polish) Commits..."

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

# Commit 5.1: Dynamic VRAM Sentinel Declarations
echo "📦 [1/4] Committing Dynamic VRAM Sentinel Declarations..."
git add \
    Source/Edrath/Public/Core/EdrathHardwareOptimizationSubsystem.h
git commit -m "feat(hardware): declare dynamic VRAM sentinel and safety threshold monitoring in UEdrathHardwareOptimizationSubsystem" || echo "Nothing to commit for 5.1."

# Commit 5.2: Multi-Tiered Dynamic VRAM Throttling Implementation
echo "📦 [2/4] Committing Dynamic VRAM Throttling Implementation..."
git add \
    Source/Edrath/Private/Core/EdrathHardwareOptimizationSubsystem.cpp
git commit -m "feat(hardware): implement multi-tiered dynamic VRAM throttling in UEdrathHardwareOptimizationSubsystem" || echo "Nothing to commit for 5.2."

# Commit 5.3: Scalability Profile Alignment
echo "📦 [3/4] Committing Scalability Profile Alignment..."
git add \
    Config/DefaultScalability.ini
git commit -m "feat(scalability): synchronize 8GB RTX 5070 laptop scalability profile and VRAM thresholds" || echo "Nothing to commit for 5.3."

# Commit 5.4: VRAM Budget Guardian Automation Test Suite
echo "📦 [4/4] Committing VRAM Budget Guardian Automation Test..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp \
    Scripts/commit_phase5.sh
git commit -m "test(hardware): add VRAM budget guardian and dynamic sentinel automation test" || echo "Nothing to commit for 5.4."

echo ""
echo "=============================================================================="
echo "✅ Phase 5 successfully committed (4 commits)!"
git log -n 4 --oneline
echo "=============================================================================="
