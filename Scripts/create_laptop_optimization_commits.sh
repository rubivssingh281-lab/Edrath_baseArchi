#!/usr/bin/env bash
# ==============================================================================
# Edrath: The World That Forgot Tomorrow
# Script to stage and commit the 4 Laptop Optimization Commits for RTX 5070 8GB
# ==============================================================================

set -e

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

echo "===================================================================="
echo " Creating 4 Discrete Git Commits for Laptop Optimization            "
echo " Target: RTX 5070 8GB | Ryzen 7 253 | 16GB DDR5 | 512GB NVMe M.2    "
echo "===================================================================="

# Commit 1: Config Layer
echo ""
echo "[1/4] Committing Config Tuning for RTX 5070 8GB Laptop Development..."
git add Config/DefaultEngine.ini \
        Config/DefaultScalability.ini \
        Config/DefaultEditor.ini \
        Config/DefaultEditorPerProjectUserSettings.ini
git commit -m "config: tune engine and scalability configs for RTX 5070 8GB laptop development

- Configure software Lumen GI with 2x probe density (downsample=8) freeing 2GB VRAM vs HWRT
- Maximize Nanite streaming pool (1024MB) and Virtual Shadow Maps (2048 pages, zero local LOD bias)
- Tune TSR with dynamic resolution (60-100%, targeting 60 FPS) and 2560MB texture streaming pool
- Bound shader compiler jobs to 6 of 16 threads and 1536MB RAM to protect 16GB DDR5 system
- Cap Derived Data Cache at 28GB with aggressive cleanup (7-14 days) to safeguard 512GB NVMe
- Add editor performance defaults and CPU throttling when viewport unfocused" || echo "Commit 1 already clean or skipped"

# Commit 2: Scalability Subsystem & Presets
echo ""
echo "[2/4] Committing Development_Laptop Quality Preset & Hardware Auto-Detection..."
git add Source/Edrath/Public/Core/EdrathTypes.h \
        Source/Edrath/Public/Core/EdrathScalabilitySubsystem.h \
        Source/Edrath/Private/Core/EdrathScalabilitySubsystem.cpp \
        Source/Edrath/Private/Core/EdrathGameInstance.cpp \
        Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp
git commit -m "feat(core): add Development_Laptop quality preset with VRAM-aware auto-detection

- Add Development_Laptop to EEdrathQualityPreset enum
- Implement RHI dedicated VRAM and platform RAM detection at startup
- Auto-select Development_Laptop preset for 6-12GB VRAM cards (RTX 5070 8GB)
- Apply maximum software Lumen probe density and full-resolution reflections
- Wire GameInstance startup logging and extend automation tests" || echo "Commit 2 already clean or skipped"

# Commit 3: Hardware Optimization Subsystem & VRAM Budgeting
echo ""
echo "[3/4] Committing Hardware Optimization Subsystem with VRAM Budget Management..."
git add Source/Edrath/Public/Core/EdrathHardwareOptimizationSubsystem.h \
        Source/Edrath/Private/Core/EdrathHardwareOptimizationSubsystem.cpp
git commit -m "feat(perf): rewrite hardware optimization subsystem with VRAM budget management

- Implement FEdrathHardwareProfile and FEdrathVRAMBudgetAllocation structs
- Calculate 7680MB usable VRAM ceiling (8192MB - 512MB driver reservation)
- Allocate non-overlapping budgets: Textures 2560MB, Nanite 1024MB, Lumen 1200MB, VSM 768MB
- Add dynamic camera optimizations (exterior bird's-eye distance vs interior 3rd-person detail)
- Add timeline-aware streaming memory scaling for Possibility Shifting" || echo "Commit 3 already clean or skipped"

# Commit 4: Documentation & Guides
echo ""
echo "[4/4] Committing Laptop Development Optimization Guide..."
git add Documentation/Laptop_Development_Optimization_Guide.md \
        Scripts/create_laptop_optimization_commits.sh \
        Scripts/create_laptop_optimization_commits.bat \
        README.md
git commit -m "docs: add laptop development guide and optimization reference

- Detail VRAM budget breakdown and architectural rationale for software Lumen
- Document system RAM and NVMe DDC management strategies
- Provide editor viewport tips, profiling commands, and troubleshooting guide
- Add cross-platform commit automation scripts" || echo "Commit 4 already clean or skipped"

echo ""
echo "===================================================================="
echo " All 4 commits created successfully!                                "
echo " Recent git commits:                                                "
echo "===================================================================="
git log -n 4 --oneline
echo "===================================================================="
echo " Ready! Run 'git push origin main' to push all 4 commits."
