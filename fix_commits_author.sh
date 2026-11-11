#!/usr/bin/env bash
# ==============================================================================
# Re-author the 3 Commits to rubivssingh281-lab for GitHub Streak Attribution
# ==============================================================================
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$REPO_DIR"

# Reset back to the parent commit
git reset --hard d46e6f043323661c47aa9aba944532060933fb33

# Set author and committer explicitly to the account tracked by streak-stats: rubivssingh281-lab
export GIT_AUTHOR_NAME="rubivssingh281-lab"
export GIT_AUTHOR_EMAIL="rubivssingh281-lab@users.noreply.github.com"
export GIT_COMMITTER_NAME="rubivssingh281-lab"
export GIT_COMMITTER_EMAIL="rubivssingh281-lab@users.noreply.github.com"

echo "=== Applying 3 commits attributed to rubivssingh281-lab ==="

# ------------------------------------------------------------------------------
# COMMIT 1: 2026-11-04 19:12:40 +0530
# ------------------------------------------------------------------------------
cat <<'EOF' > CONTRIBUTING.md
# Contributing to Edrath: The World That Forgot Tomorrow

Thank you for contributing to **Edrath: The World That Forgot Tomorrow** — an open-world dark fantasy, mystery, and psychological thriller Action RPG built in **Unreal Engine 5 (UE 5.5+)** using C++20 and Blueprints.

## 1. Unreal Engine 5 C++ Coding Standards

We follow Epic Games' official Unreal Engine coding guidelines:
- **Prefix Conventions**:
  - `A` for Actors (e.g., `AKaelCharacter`, `APossibilityShiftActor`).
  - `U` for UObjects and Subsystems (e.g., `UBlackClockSubsystem`, `UEchoCombatComponent`).
  - `F` for plain structs (e.g., `FForensicEvidenceRecord`, `FRealityDistortionState`).
  - `T` for templates (e.g., `TSubclassOf<AActor>`).
  - `E` for enums (e.g., `ETimelineState`, `ECameraPerspectiveMode`).
  - `I` for interfaces (e.g., `IInteractableMemoryInterface`).
- **Standard**: C++20 features enabled via `CppStandard = CppStandardVersion.Cpp20` in `Edrath.Build.cs`.
- **Memory Safety**: Use Unreal smart pointers (`TWeakObjectPtr`, `TSharedPtr`) and garbage collection macros (`UPROPERTY()`) to avoid dangling pointers.

## 2. Blueprint & Asset Guidelines

- **Blueprint Logic**: Limit heavy mathematical loops and continuous raycasting in Blueprints; migrate compute-intensive systems to C++ subsystems.
- **Git LFS File Locking**:
  - Always lock binary assets before editing:
    ```bash
    git lfs lock Content/Maps/PreAlpha_Act1.umap
    ```
  - Unlock after pushing changes:
    ```bash
    git lfs unlock Content/Maps/PreAlpha_Act1.umap
    ```

## 3. Branching & Commit Workflow

- **Branch Naming**:
  - `feat/<subsystem>`: New gameplay mechanics, shaders, or UI modules.
  - `fix/<defect>`: Bug fixes or crash dump resolutions.
  - `docs/<topic>`: Documentation, Nanite budgets, or architectural specs.
  - `chore/<task>`: Build tooling, device profiles, or dependency updates.
- **Commit Messages**: Follow Conventional Commits format (`feat:`, `fix:`, `docs:`, `chore:`).
EOF

export GIT_AUTHOR_DATE="2026-11-04 19:12:40 +0530"
export GIT_COMMITTER_DATE="2026-11-04 19:12:40 +0530"
git add CONTRIBUTING.md
git commit -m "docs(contributing): add UE5 C++20 coding standards, Blueprint guidelines, and Git LFS workflow"
echo "[1/3] Created: docs(contributing)"

# ------------------------------------------------------------------------------
# COMMIT 2: 2026-11-04 19:25:15 +0530
# ------------------------------------------------------------------------------
cat <<'EOF' > .editorconfig
# Unreal Engine 5 & C++20 EditorConfig for Edrath
root = true

[*]
charset = utf-8
end_of_line = lf
insert_final_newline = true
trim_trailing_whitespace = true

[*.{cpp,h,inl,cs}]
indent_style = tab
indent_size = 4
tab_width = 4

[*.{usf,ush}]
indent_style = space
indent_size = 4
tab_width = 4

[*.{json,uproject,uplugin}]
indent_style = space
indent_size = 2

[*.{ini,cfg}]
indent_style = space
indent_size = 4

[*.{md,yml,yaml}]
indent_style = space
indent_size = 2
trim_trailing_whitespace = false
EOF

mkdir -p .github

cat <<'EOF' > .github/pull_request_template.md
## Description
Provide a concise overview of the changes, architectural subsystems touched, and motivation behind this PR.

## Type of Change
- [ ] Core C++ Subsystem (`Source/EdrathCore`)
- [ ] HLSL Reality Shader / Virtual Directory (`Shaders/`)
- [ ] Content Architecture / Collision Channels (`Config/`, `Content/`)
- [ ] Bug Fix / Crash Dump Resolution
- [ ] Documentation / Pipeline Guides

## Unreal Engine Verification Checklist
- [ ] Project compiles cleanly in Visual Studio 2022 / JetBrains Rider with zero warnings.
- [ ] Verified under target laptop scalability profile (`Windows_Laptop_Balanced`).
- [ ] Any binary `.uasset` / `.umap` modifications tracked under Git LFS and unlocked.
- [ ] No circular Blueprint dependencies introduced.
EOF

export GIT_AUTHOR_DATE="2026-11-04 19:25:15 +0530"
export GIT_COMMITTER_DATE="2026-11-04 19:25:15 +0530"
git add .editorconfig .github/pull_request_template.md
git commit -m "chore(tooling): add Unreal Engine 5 .editorconfig and GitHub Pull Request template"
echo "[2/3] Created: chore(tooling)"

# ------------------------------------------------------------------------------
# COMMIT 3: 2026-11-04 19:38:50 +0530
# ------------------------------------------------------------------------------
cat <<'EOF' > CHANGELOG.md
# Changelog

All notable changes to **Edrath: The World That Forgot Tomorrow** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Assembly of Pre-Alpha Act 1 playable vertical slice (*The Man Who Was Never Born*).
- Nanite asset ingestion for Veyl gothic cathedral ruins and Hollowgreen root structures.
- Sentry native C++ crash reporter integration in `Plugins/Sentry`.

## [0.2.0] - 2026-11-04

### Added
- **Laptop Hardware Readiness & Device Profiles**:
  - Added dedicated scalability profiles (`Windows_Laptop_Battery`, `Windows_Laptop_Balanced`, `Windows_Laptop_Performance`) in `Config/DefaultDeviceProfiles.ini`.
  - Enabled UE 5.5 `ModelingToolsEditorMode`, `GeometryScripting`, and `ControlRig` inside `Edrath.uproject`.
- **Custom HLSL Shaders**:
  - Virtual shader path mounting (`/Project/Shaders`) in `Source/Edrath/EdrathModule.cpp`.
  - Added reality shaders: `PossibilityDissolve.usf`, `MemoryEchoGhost.usf`, and `PackedRGBACustomizer.usf`.
- **DevOps & Tooling**:
  - 1-Click environment setup scripts for Windows, macOS, and Linux in `Scripts/SetupLaptopEnvironment.*`.
  - Comprehensive 3D Modeling, Nanite polycount budgets, and skeletal socket pipeline guide.

## [0.1.0] - 2026-10-31

### Added
- **Core Architecture Base**:
  - Initialized `Source/EdrathCore` foundational C++ subsystems.
  - Implemented Black Clock reality-shifting managers and dynamic dual-perspective camera controller (isometric exterior to third-person interior).
  - Forensic Evidence Ledger and Memory Echo investigation state machines.
EOF

export GIT_AUTHOR_DATE="2026-11-04 19:38:50 +0530"
export GIT_COMMITTER_DATE="2026-11-04 19:38:50 +0530"
git add CHANGELOG.md
git commit -m "docs(changelog): initialize CHANGELOG.md tracking engine milestones and architecture versions"
echo "[3/3] Created: docs(changelog)"

# Clean up runner script
rm -- "$0"

echo ""
echo "=== All 3 commits re-authored to rubivssingh281-lab! ==="
git log -3 --pretty=format:"%h - %an <%ae> (%ad): %s" --date=iso
echo ""
echo "Ready to push! Run: git push -f origin main"
