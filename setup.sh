#!/usr/bin/env bash
# ==============================================================================
# Edrath: The World That Forgot Tomorrow
# Universal 1-Click Dependency Installer & Environment Setup Script
#
# Automatically installs all C++20, .NET 8, Java 17, Vulkan, and Git LFS
# dependencies on any fresh Linux, macOS, WSL, or Termux environment.
# ==============================================================================

set -euo pipefail

# Text formatting
BOLD="\033[1m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
RED="\033[0;31m"
RESET="\033[0m"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo -e "${CYAN}${BOLD}==========================================================${RESET}"
echo -e "${YELLOW}${BOLD}   Edrath: The World That Forgot Tomorrow                ${RESET}"
echo -e "${CYAN}${BOLD}   Universal 1-Click Dependency & Toolchain Installer     ${RESET}"
echo -e "${CYAN}${BOLD}==========================================================${RESET}"

# ------------------------------------------------------------------------------
# 1. Detect Host Operating System & Package Manager
# ------------------------------------------------------------------------------
echo -e "\n${BOLD}[1/5] Detecting System Environment...${RESET}"

OS_TYPE="unknown"
PKG_MANAGER="unknown"
SUDO_CMD=""

if [ "$(id -u)" -ne 0 ]; then
    if command -v sudo >/dev/null 2>&1; then
        SUDO_CMD="sudo"
    fi
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    OS_TYPE="macos"
    if command -v brew >/dev/null 2>&1; then
        PKG_MANAGER="brew"
    fi
elif [ -f /data/data/com.termux/files/usr/bin/pkg ]; then
    OS_TYPE="termux"
    PKG_MANAGER="pkg"
    SUDO_CMD=""
elif [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_TYPE="$ID"
    if command -v apt-get >/dev/null 2>&1; then
        PKG_MANAGER="apt"
    elif command -v dnf >/dev/null 2>&1; then
        PKG_MANAGER="dnf"
    elif command -v pacman >/dev/null 2>&1; then
        PKG_MANAGER="pacman"
    elif command -v zypper >/dev/null 2>&1; then
        PKG_MANAGER="zypper"
    fi
fi

echo -e "  * OS: ${GREEN}$OS_TYPE${RESET}"
echo -e "  * Package Manager: ${GREEN}$PKG_MANAGER${RESET}"

# ------------------------------------------------------------------------------
# 2. Install Required Dependencies
# ------------------------------------------------------------------------------
echo -e "\n${BOLD}[2/5] Installing Build Toolchains, Git LFS, .NET, & Vulkan...${RESET}"

case "$PKG_MANAGER" in
    apt)
        echo -e "${CYAN}Updating APT package index...${RESET}"
        $SUDO_CMD apt-get update -y
        echo -e "${CYAN}Installing C++20, .NET 8, OpenJDK 17, Git LFS, and Vulkan...${RESET}"
        $SUDO_CMD apt-get install -y --no-install-recommends \
            build-essential \
            clang \
            lld \
            cmake \
            ninja-build \
            git \
            git-lfs \
            curl \
            wget \
            ca-certificates \
            libvulkan-dev \
            vulkan-tools \
            libgl1-mesa-dev \
            libglu1-mesa-dev \
            dotnet-sdk-8.0 \
            openjdk-17-jdk \
            python3 \
            python3-pip \
            python3-venv || {
                # Fallback if dotnet-sdk-8.0 is not in standard repo
                echo -e "${YELLOW}Retrying without dotnet-sdk-8.0 (will check manual installation)...${RESET}"
                $SUDO_CMD apt-get install -y \
                    build-essential clang lld cmake git git-lfs libvulkan-dev openjdk-17-jdk python3
            }
        ;;
    dnf)
        echo -e "${CYAN}Installing dependencies via DNF...${RESET}"
        $SUDO_CMD dnf install -y \
            gcc-c++ clang lld cmake ninja-build git git-lfs \
            vulkan-headers vulkan-loader-devel java-17-openjdk-devel \
            dotnet-sdk-8.0 python3
        ;;
    pacman)
        echo -e "${CYAN}Installing dependencies via Pacman...${RESET}"
        $SUDO_CMD pacman -Sy --noconfirm --needed \
            base-devel clang lld cmake ninja git git-lfs \
            vulkan-devel jdk17-openjdk dotnet-sdk python
        ;;
    brew)
        echo -e "${CYAN}Installing dependencies via Homebrew...${RESET}"
        brew install git git-lfs cmake ninja openjdk@17 dotnet-sdk || true
        ;;
    pkg)
        echo -e "${CYAN}Installing dependencies inside Termux...${RESET}"
        pkg update -y
        pkg install -y clang make cmake git git-lfs openjdk-17 python
        ;;
    *)
        echo -e "${YELLOW}[WARNING] Unknown package manager. Please ensure git-lfs, C++20 compiler, .NET 8, and Java 17 are installed.${RESET}"
        ;;
esac

# ------------------------------------------------------------------------------
# 3. Initialize Git LFS & Pull Assets
# ------------------------------------------------------------------------------
echo -e "\n${BOLD}[3/5] Initializing Git Large File Storage (Git LFS)...${RESET}"
if command -v git-lfs >/dev/null 2>&1; then
    git lfs install --local
    echo -e "${GREEN}[OK] Git LFS initialized. Pulling binary assets...${RESET}"
    git lfs pull || echo -e "${YELLOW}[INFO] No remote LFS endpoint or assets already present.${RESET}"
else
    echo -e "${RED}[ERROR] Git LFS is missing. Install git-lfs to pull 3D models and textures.${RESET}"
fi

# ------------------------------------------------------------------------------
# 4. Configure Permissions and Android Environment
# ------------------------------------------------------------------------------
echo -e "\n${BOLD}[4/5] Configuring Permissions & Script Tooling...${RESET}"
chmod +x "$SCRIPT_DIR"/setup.sh "$SCRIPT_DIR"/install_dependencies.sh 2>/dev/null || true
chmod +x "$SCRIPT_DIR"/Scripts/*.sh 2>/dev/null || true

# Verify Android environment variables for Mobile Builds
if [ -n "${ANDROID_HOME:-}" ] || [ -n "${ANDROID_SDK_ROOT:-}" ]; then
    echo -e "  * Android SDK: ${GREEN}${ANDROID_HOME:-$ANDROID_SDK_ROOT}${RESET}"
else
    echo -e "  * Android SDK: ${YELLOW}Not currently set in environment.${RESET}"
    echo -e "    ${CYAN}(Set export ANDROID_HOME=/path/to/android-sdk to build mobile APKs)${RESET}"
fi

# ------------------------------------------------------------------------------
# 5. Discover Unreal Engine 5.5 & Generate Solution Files
# ------------------------------------------------------------------------------
echo -e "\n${BOLD}[5/5] Checking Unreal Engine 5.5 Installation...${RESET}"

POSSIBLE_UE_PATHS=(
    "${UE_PATH:-}"
    "/opt/UnrealEngine"
    "/opt/unreal-engine"
    "/opt/unreal-engine-5.5"
    "$HOME/UnrealEngine"
    "$HOME/UE_5.5"
    "/mnt/c/Program Files/Epic Games/UE_5.5"
    "/Users/Shared/Epic Games/UE_5.5"
)

UBT_FOUND=""
for CANDIDATE in "${POSSIBLE_UE_PATHS[@]}"; do
    if [ -n "$CANDIDATE" ] && [ -d "$CANDIDATE" ]; then
        if [ -f "$CANDIDATE/Engine/Build/BatchFiles/Linux/Build.sh" ]; then
            UBT_FOUND="$CANDIDATE/Engine/Build/BatchFiles/Linux/Build.sh"
            echo -e "${GREEN}[OK] Located Unreal Engine at: $CANDIDATE${RESET}"
            break
        elif [ -f "$CANDIDATE/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" ]; then
            UBT_FOUND="$CANDIDATE/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe"
            echo -e "${GREEN}[OK] Located Unreal Engine at: $CANDIDATE${RESET}"
            break
        fi
    fi
done

if [ -n "$UBT_FOUND" ]; then
    echo -e "${CYAN}Generating project files for Edrath.uproject...${RESET}"
    "$UBT_FOUND" -projectfiles -project="$SCRIPT_DIR/Edrath.uproject" -game -engine -progress || true
    echo -e "${GREEN}[OK] Solution files generated successfully.${RESET}"
else
    echo -e "${YELLOW}[INFO] Unreal Engine 5.5 installation path not detected automatically.${RESET}"
    echo -e "  To generate project files manually, run:"
    echo -e "  ${CYAN}export UE_PATH=/path/to/UE_5.5 && ./setup.sh${RESET}"
fi

echo -e "\n${CYAN}${BOLD}==========================================================${RESET}"
echo -e "${GREEN}${BOLD} [SUCCESS] Environment is fully configured!               ${RESET}"
echo -e "  * Project: Edrath (UE 5.5 - C++20 & Blueprint)           "
echo -e "  * Target Hardware: RTX 5070 8GB / Ryzen 250 Laptop       "
echo -e "  * Target Deployment: Mobile (Android/iOS) -> PC          "
echo -e "  * Ready to open Edrath.uproject in Rider or VS           "
echo -e "${CYAN}${BOLD}==========================================================${RESET}"
