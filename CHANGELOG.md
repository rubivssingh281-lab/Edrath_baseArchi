# Changelog

All notable changes to **Edrath: The World That Forgot Tomorrow** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Assembly of Pre-Alpha Act 1 playable vertical slice (*The Man Who Was Never Born*).
- Nanite asset ingestion for Veyl gothic cathedral ruins and Hollowgreen root structures.
- Sentry native C++ crash reporter integration in `Plugins/Sentry`.

## [0.4.0] - 2026-11-10

### Added
- **Dynamic Dual-Perspective Reticle & Inspection Prompt HUD**:
  - Implemented `UEdrathReticleWidget` (`Source/Edrath/Public/UI/EdrathReticleWidget.h`, `Source/Edrath/Private/UI/EdrathReticleWidget.cpp`) providing seamless switching between Isometric ground cursor/telegraphing and Over-the-Shoulder Third-Person crosshair.
  - Added forensic examination focus detection and distance-scaled interact prompts (`[E] Inspect`).
- **Echo Combat Action & 120-Frame Timeline Buffer HUD**:
  - Implemented `UEchoCombatBufferWidget` (`Source/Edrath/Public/UI/EchoCombatBufferWidget.h`, `Source/Edrath/Private/UI/EchoCombatBufferWidget.cpp`) visualizing the 120-frame circular combat recording buffer.
  - Added recent combat action montage history, Echo Strike projection delay slider (`TriggerEchoStrike(1.2f)`), cooldown wheel, and regional resonance theme coloring (Veyl, Hollowgreen, Nhal, etc.).
- **Forensic Contradiction Deduction Board**:
  - Implemented `UForensicContradictionWidget` (`Source/Edrath/Public/UI/ForensicContradictionWidget.h`, `Source/Edrath/Private/UI/ForensicContradictionWidget.cpp`) enabling clue cross-referencing and contradiction resolution.
  - Registered default deduction rules for Act 1 Crypt priest revelation and Ilyra executioner blade mystery.
- **Master HUD Architecture & Mobile Touch Controls**:
  - Implemented `UEdrathTouchControlsWidget` (`Source/Edrath/Public/UI/EdrathTouchControlsWidget.h`, `Source/Edrath/Private/UI/EdrathTouchControlsWidget.cpp`) for on-screen touch actions on mobile platforms.
  - Expanded `AEdrathHUD` to orchestrate all UI widgets, input mode switching, and viewport layers.
  - Added comprehensive `FEdrathFullUIAndHUDArchitectureVerificationTest` in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp`.

## [0.3.0] - 2026-11-06

### Added
- **Mobile-First Engine Pipeline**:
  - Created `Config/Android/AndroidEngine.ini` stripping heavy PC features (Lumen, Nanite, VSM) in favor of CSM and ASTC texture streaming for mobile TBDR GPUs.
  - Configured `Config/DefaultGame.ini` with Android packaging metadata (ARM64, Vulkan, ASTC, SensorLandscape, SDK 34).
  - Added mobile touch control bindings in `Config/DefaultInput.ini` with virtual joystick integration.
- **Hardware Profile for RTX 5070 (8GB VRAM) & Ryzen 250 (16GB RAM)**:
  - Tuned `Config/DefaultEngine.ini` with VRAM safety caps (`r.Streaming.PoolSize=2560`, `r.Shadow.Virtual.MaxPhysicalPages=1536`).
  - Added CPU worker throttle (`r.ShaderCompiler.MaxShaderJobs=6`, `MaxShaderMemory=1536`) to protect 16GB system RAM from thrashing.
  - Capped Derived Data Cache (`MaxCacheSizeGB=32`) to protect 512GB SSD storage.
  - Added custom `[Windows_Laptop_RTX5070 DeviceProfile]` in `Config/DefaultDeviceProfiles.ini`.
- **Cross-Platform C++ Architecture**:
  - Implemented `UEdrathPlatformSubsystem` (`Source/Edrath/Public/EdrathPlatformSubsystem.h`) for automatic runtime mobile vs. PC hardware detection, dynamic device tier scaling, and touch control activation.
- **512GB SSD Space Management**:
  - Added `Scripts/CleanBuildArtifacts.ps1` and `Scripts/CleanBuildArtifacts.sh` for one-click purging of transient cooked packages and shader caches.

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
