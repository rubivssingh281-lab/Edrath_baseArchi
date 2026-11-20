#!/usr/bin/env bash
# Edrath: The World That Forgot Tomorrow
# Disk Space Reclamation Script for 512GB SSD on Linux/macOS

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "=========================================================="
echo "   Edrath: SSD Disk Space Optimizer                      "
echo "   Reclaiming space on 512GB development drive...         "
echo "=========================================================="

PATHS_TO_CLEAN=(
    "$PROJECT_ROOT/Saved/Logs"
    "$PROJECT_ROOT/Saved/Crashes"
    "$PROJECT_ROOT/Saved/ShaderCache"
    "$PROJECT_ROOT/Saved/Cooked"
)

if [[ "${1:-}" == "--deep" ]]; then
    echo "[WARNING] Deep Clean requested: Including Intermediate and Binaries..."
    PATHS_TO_CLEAN+=(
        "$PROJECT_ROOT/Intermediate"
        "$PROJECT_ROOT/Binaries"
    )
fi

for TARGET_PATH in "${PATHS_TO_CLEAN[@]}"; do
    if [ -d "$TARGET_PATH" ]; then
        SIZE=$(du -sh "$TARGET_PATH" 2>/dev/null | cut -f1 || echo "unknown")
        rm -rf "$TARGET_PATH"
        echo "[OK] Cleaned $TARGET_PATH ($SIZE)"
    fi
done

echo "=========================================================="
echo "   Cleanup completed successfully.                        "
echo "=========================================================="
