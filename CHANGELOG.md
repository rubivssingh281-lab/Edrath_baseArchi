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
