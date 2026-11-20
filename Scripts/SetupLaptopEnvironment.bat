@echo off
setlocal
title Edrath - Laptop Environment Setup

echo ==========================================================
echo    Edrath: The World That Forgot Tomorrow
echo    Laptop Setup Launcher
echo ==========================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0SetupLaptopEnvironment.ps1"

if %ERRORLEVEL% NEQ 0 (
    echo [WARNING] Setup script finished with status code %ERRORLEVEL%.
) else (
    echo [SUCCESS] Environment is ready.
)

pause
