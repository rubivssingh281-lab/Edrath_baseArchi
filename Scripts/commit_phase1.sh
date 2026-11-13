#!/usr/bin/env bash
# ==============================================================================
# Phase 1: Dual-Perspective Camera & Transition Polish (4 Commits)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "🚀 Starting Phase 1 (Dual-Perspective Camera & Transition Polish) Commits..."

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

# Commit 1.1: Anti-Clipping Sphere Collision Sweep & Transition Alpha Tracking
echo "📦 [1/4] Committing Anti-Clipping Probe & Transition Tracking..."
git add Source/Edrath/Public/Camera/EdrathDualCameraComponent.h
git commit -m "feat(camera): add dynamic anti-clipping sphere collision sweep and transition alpha tracking in UEdrathDualCameraComponent" || echo "Nothing to commit for 1.1."

# Commit 1.2: Doorway Orientation Quaternion Slerp
echo "📦 [2/4] Committing Doorway Orientation Slerp..."
git add Source/Edrath/Private/Camera/EdrathDualCameraComponent.cpp
git commit -m "feat(camera): implement doorway orientation quaternion slerp alignment on interior threshold entry" || echo "Nothing to commit for 1.2."

# Commit 1.3: Dithered Roof Culling Fade
echo "📦 [3/4] Committing Smooth Dithered Roof Culling..."
git add \
    Source/Edrath/Public/Camera/EdrathInteriorTransitionVolume.h \
    Source/Edrath/Private/Camera/EdrathInteriorTransitionVolume.cpp
git commit -m "feat(camera): implement smooth dithered roof culling fade in AEdrathInteriorTransitionVolume" || echo "Nothing to commit for 1.3."

# Commit 1.4: Automation Verification Tests
echo "📦 [4/4] Committing Camera & Roof Fade Automation Test..."
git add \
    Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp \
    Scripts/commit_phase1.sh
git commit -m "test(camera): add dual-perspective transition and roof fade automation verification test" || echo "Nothing to commit for 1.4."

echo ""
echo "=============================================================================="
echo "✅ Phase 1 successfully committed (4 commits)!"
git log -n 4 --oneline
echo "=============================================================================="
