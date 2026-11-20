#!/usr/bin/env bash
# Edrath: The World That Forgot Tomorrow
# Cross-platform environment setup for Linux / macOS laptops

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=========================================================="
echo "   Edrath: The World That Forgot Tomorrow                 "
echo "   Laptop Setup & Integrity Check                         "
echo "=========================================================="

# Delegate to the universal dependency installer
if [ -f "$PROJECT_ROOT/setup.sh" ]; then
    bash "$PROJECT_ROOT/setup.sh" "$@"
else
    echo "[1/3] Checking Git Large File Storage (Git LFS)..."
    if command -v git-lfs >/dev/null 2>&1; then
        echo "[OK] Git LFS is installed."
        git lfs install --local
        git lfs pull
    else
        echo "[WARNING] Git LFS not detected. Please install git-lfs for binary asset support."
    fi

    echo "[2/3] Checking project files..."
    if [ -f "$PROJECT_ROOT/Edrath.uproject" ]; then
        echo "[OK] Located Edrath.uproject"
    fi

    echo "[3/3] Setting execution permissions on scripts..."
    chmod +x "$SCRIPT_DIR"/*.sh 2>/dev/null || true

    echo "=========================================================="
    echo " Setup complete! Ready to open Edrath in Unreal Engine 5.5."
    echo "=========================================================="
fi
