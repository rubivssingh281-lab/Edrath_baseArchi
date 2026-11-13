#!/usr/bin/env bash
# ==============================================================================
# Edrath: The World That Forgot Tomorrow
# Master Multi-Phase Polish Commit Orchestrator (24 Commits Local)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "=============================================================================="
echo "🌟 Executing Master 6-Phase Polish Commit Orchestrator (24 Commits)"
echo "=============================================================================="

# Ensure user credentials
if ! git config user.name >/dev/null 2>&1; then
    git config user.name "rubivssingh281"
fi
if ! git config user.email >/dev/null 2>&1; then
    git config user.email "rubivssingh281@gmail.com"
fi

# Execute Phase 1: Dual-Perspective Camera & Transition Polish (4 Commits)
echo ""
echo "▶ Running Phase 1 Commits..."
bash "${SCRIPT_DIR}/commit_phase1.sh"

# Execute Phase 2: Echo Combat & Timeline Sub-Frame Interpolation (4 Commits)
echo ""
echo "▶ Running Phase 2 Commits..."
bash "${SCRIPT_DIR}/commit_phase2.sh"

# Execute Phase 3: Forensic Evidence & Contradiction Deduction Engine (4 Commits)
echo ""
echo "▶ Running Phase 3 Commits..."
bash "${SCRIPT_DIR}/commit_phase3.sh"

# Execute Phase 4: Pre-Alpha Act 1 Five-Zone Continuity Polish (4 Commits)
echo ""
echo "▶ Running Phase 4 Commits..."
bash "${SCRIPT_DIR}/commit_phase4.sh"

# Execute Phase 5: 8GB VRAM Guardian & Dynamic Sentinel (4 Commits)
echo ""
echo "▶ Running Phase 5 Commits..."
bash "${SCRIPT_DIR}/commit_phase5.sh"

# Execute Phase 6: Comprehensive Automation & Invariant Validation (4 Commits)
echo ""
echo "▶ Running Phase 6 Commits..."
bash "${SCRIPT_DIR}/commit_phase6.sh"

echo ""
echo "=============================================================================="
echo "🎉 ALL 6 PHASES SUCCESSFULLY COMMITTED (Total: 24 local commits)!"
echo "Run 'git log -n 24 --oneline' to view all new commits."
echo "When ready to push to remote: git push origin main"
echo "=============================================================================="
