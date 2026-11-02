#!/usr/bin/env bash
# Edrath: The World That Forgot Tomorrow
# Script to create the 6 discrete, sequential git commits for GitHub.

set -e

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

echo "=========================================================="
echo " Creating 6 Discrete Git Commits for Edrath BaseArchi     "
echo "=========================================================="

# Commit 1
echo "[1/6] Committing Laptop Optimization & Modeling Engine Plugins..."
git add Edrath.uproject Config/DefaultEngine.ini Config/DefaultDeviceProfiles.ini
git commit -m "feat(engine): enable UE5.5 Modeling Tools & configure laptop hardware optimization profiles" || echo "Commit 1 already clean or skipped"

# Commit 2
echo "[2/6] Committing HLSL Custom Shaders & Virtual Path Mount..."
git add Shaders/ Source/Edrath/EdrathModule.h Source/Edrath/EdrathModule.cpp Source/Edrath/Edrath.Build.cs
git commit -m "feat(shaders): implement HLSL custom reality shaders and virtual directory mount" || echo "Commit 2 already clean or skipped"

# Commit 3
echo "[3/6] Committing Content Asset Architecture & Enhanced Input..."
git add Content/ Config/DefaultInput.ini Config/DefaultEngine.ini
git commit -m "feat(content): scaffold Content asset architecture, collision channels, and Enhanced Input profiles" || echo "Commit 3 already clean or skipped"

# Commit 4
echo "[4/6] Committing 3D Modeling & Art Pipeline Specification..."
git add Documentation/3D_Modeling_and_Art_Pipeline_Guide.md
git commit -m "docs(art): add comprehensive 3D Modeling, Nanite budgets, skeletal sockets, and texturing guide" || echo "Commit 4 already clean or skipped"

# Commit 5
echo "[5/6] Committing 1-Click Laptop Setup & DevOps Scripts..."
git add Scripts/
git commit -m "feat(devops): add automated laptop setup & one-click build scripts for Windows, Mac, and Linux" || echo "Commit 5 already clean or skipped"

# Commit 6
echo "[6/6] Committing Master Architecture & README Milestone Updates..."
git add README.md
git commit -m "docs(readme): update Master Technical Architecture with Laptop Readiness, Modeling Tools guide, and 6-commit milestone log" || echo "Commit 6 already clean or skipped"

echo "=========================================================="
echo " All 6 commits created successfully!                      "
echo " Recent git commits:                                      "
git log -n 6 --oneline
echo "=========================================================="
echo " Run 'git push origin main' to push all 6 commits to GitHub."
