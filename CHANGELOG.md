# Changelog

All notable changes to **Edrath: The World That Forgot Tomorrow** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Nanite asset ingestion for Veyl gothic cathedral ruins and Hollowgreen root structures.

## [0.12.0] - 2026-11-23

### Added
- **Velocity-Modulated Locomotion Audio & Refractory Filtering**:
  - Enhanced `UEdrathAudioSubsystem` with `CalculateGaitVolumeMultiplier` dynamically attenuating footstep volume from sneak (0.25x-0.45x) to sprint (1.20x-1.55x) and dodge roll (1.40x).
  - Added timestamp refractory gate (85ms per foot bone) suppressing rapid footstep spam to eliminate audio bus clipping and phase cancellation during high-velocity dodge rolls.
  - Added tactical dodge roll slide Foley routing with high-friction drag sound tags (`Footstep_Slide_Stone`, `Footstep_Slide_Mud`, `Footstep_Slide_Snow`, `Footstep_Slide_Ash`).
- **Dynamic Regional Weather & Environmental Hazard Subsystem**:
  - Implemented `UEdrathWeatherSubsystem` (`Source/Edrath/Public/World/EdrathWeatherSubsystem.h`, `Source/Edrath/Private/World/EdrathWeatherSubsystem.cpp`) managing canonical atmospheric conditions across all 6 Acts (`ClearChronalSky`, `AshenChronalStorm`, `VeylPossibilityFog`, `HollowgreenSporeRain`, `NhalGeothermalSteam`, `CrownBlizzard`, `UnwrittenChronalSurge`).
  - Environmental hazard simulation: dynamic visibility distance, chronal instability drain, frost glaze intensity, and movement penalties.
  - Thermal Throttling Coupling: adapts Niagara particle spawn rates between Nominal (1.0x) and Critical (0.25x) under thermal mitigation via `UEdrathHardwareOptimizationSubsystem`.
- **Forensic Clue Matrix & Save State Serialization**:
  - Implemented `UEdrathForensicSubsystem` (`Source/Edrath/Public/Narrative/EdrathForensicSubsystem.h`, `Source/Edrath/Private/Narrative/EdrathForensicSubsystem.cpp`) tracking clue lifecycle stages (`Undiscovered`, `Discovered`, `AnalyzedInLedger`, `LinkedToContradiction`, `SynthesizedBreakthrough`).
  - Serializes clue discovery masks and breakthrough tags directly into `UEdrathSaveGame`.
- **Automated Verification Suites (3 New Suites)**:
  - Extended `FEdrathAudioSubsystemPerspectiveAndSurfaceTest` verifying gait volume scaling, refractory window filtering, and slide Foley.
  - Added `FEdrathWeatherAndEnvironmentalHazardTest` verifying regional weather hazards and thermal particle throttling.
  - Added `FEdrathForensicSubsystemClueMatrixTest` verifying clue registration, duplicate rejection, ledger analysis, breakthrough commitment, and save state serialization roundtrips.

## [0.11.0] - 2026-11-23

### Added
- **Interactive Reality-Altering Side Quest World Actors**:
  - Implemented `AWeddingAltarActor` (`Source/Edrath/Public/World/WeddingAltarActor.h`, `Source/Edrath/Private/World/WeddingAltarActor.cpp`) governing `Quest.WeddingNotHappened`: handles wedding party echoes, current living family state, and the tragic choice between timeline merging and echo severing.
  - Implemented `ABuriedFutureGraveActor` (`Source/Edrath/Public/World/BuriedFutureGraveActor.h`, `Source/Edrath/Private/World/BuriedFutureGraveActor.cpp`) governing `Quest.BuriedFuture`: unearthing the farmer's future corpse dated 30 years ahead and mutating family memory anchor continuity.
  - Implemented `AVillageVotingShrineActor` (`Source/Edrath/Public/World/VillageVotingShrineActor.h`, `Source/Edrath/Private/World/VillageVotingShrineActor.cpp`) governing `Quest.VillageVotesOnDead`: ballot urn inspection, exposing the corrupt mayor's rigged swap of his accuser.
  - Implemented `ADragonMemoryHoardActor` (`Source/Edrath/Public/World/DragonMemoryHoardActor.h`, `Source/Edrath/Private/World/DragonMemoryHoardActor.cpp`) governing `Quest.DragonNoFire`: crystallized memory hoard interaction, choosing between returning stolen memories to citizens and awakening the apocalyptic dragon.
  - Implemented `ALastHonestKingMonumentActor` (`Source/Edrath/Public/World/LastHonestKingMonumentActor.h`, `Source/Edrath/Private/World/LastHonestKingMonumentActor.cpp`) governing `Quest.LastHonestKing`: royal confession plaque inspection, revealing the erased perpetrator versus preserving peace.
- **Dual-Perspective Acoustic Subsystem & Black Clock Psychoacoustics**:
  - Implemented `UEdrathAudioSubsystem` (`Source/Edrath/Public/Core/EdrathAudioSubsystem.h`, `Source/Edrath/Private/Core/EdrathAudioSubsystem.cpp`) managing dynamic environmental acoustic profiles (`ExteriorOpenField`, `InteriorClaustrophobicCrypt`, `TimelineDistortion`, `ConsciousArchive`, `DeepSubterranean`) responding dynamically to camera perspective transitions and timeline distortion state.
  - Spatialized Black Clock Sternum Ticking: dynamic heart-rate tempo acceleration from 60 BPM (resting) up to 200 BPM with pitch warping and distortion disturbances under critical instability.
  - Surface-Aware Footstep Routing: physical material audio routing from `UEdrathTerrainSubsystem` with left/right foot acoustic humanization across all `EEdrathSurfaceType` terrains.
- **Automated Verification Suite Expansion (2 New Suites)**:
  - Added `FEdrathSideQuestsWorldActorsVerificationTest` verifying state transitions and narrative resolutions for all 5 side quest physical world actors.
  - Added `FEdrathAudioSubsystemPerspectiveAndSurfaceTest` verifying acoustic filtering, Black Clock tempo scaling, and surface footstep routing.

## [0.10.0] - 2026-11-20

### Added
- **Act 4 Subterranean Metropolis of Nhal Orchestration**:
  - Implemented `AAct4NhalDirector` (`Source/Edrath/Public/World/Act4NhalDirector.h`, `Source/Edrath/Private/World/Act4NhalDirector.cpp`) managing faction mediation between Restorationists and Revanchists, future-dead companion echo manifestations, dead child prophecy, ally betrayal defusal, and Twist 3 ledger revelation.
- **Act 5 The Crown of Winter & Conscious Archive**:
  - Implemented `AAct5CrownOfWinterDirector` (`Source/Edrath/Public/World/Act5CrownOfWinterDirector.h`, `Source/Edrath/Private/World/Act5CrownOfWinterDirector.cpp`) orchestrating mountain traversal, time-dilated chamber exterior aging simulation (`SimulateInteriorTimeDilation`), Conscious Archive infiltration, dialogue where the machine asks to die (`Story.Act5.ArchiveRequest`), and ontological stance commitment.
- **Act 6 The Unwritten Sea & The First Archivist Climax Boss**:
  - Implemented `AAct6UnwrittenSeaDirector` (`Source/Edrath/Public/World/Act6UnwrittenSeaDirector.h`, `Source/Edrath/Private/World/Act6UnwrittenSeaDirector.cpp`) coordinating sky-ocean navigation, dynamic island materialization of unchosen paths via `AUnwrittenIslandSpawner`, former selves confrontation, and evaluation of the 4 Endings.
  - Implemented `AFirstArchivistBossCharacter` (`Source/Edrath/Public/Character/FirstArchivistBossCharacter.h`, `Source/Edrath/Private/Character/FirstArchivistBossCharacter.cpp`) featuring 3-phase combat, crystalline time-archive armor, timeline shield shatter mechanics via Black Clock Echo strikes, and possibility collapse novae.
- **Autonomous Companion AI Controller**:
  - Implemented `AEdrathCompanionAIController` (`Source/Edrath/Public/Character/EdrathCompanionAIController.h`, `Source/Edrath/Private/Character/EdrathCompanionAIController.cpp`) adapting companion follow behaviors, combat support anticipation from Senn's 0.8s foresight warnings, and suspicion escalation leading to defection under excessive Black Clock usage.
- **Automated Verification Suite Expansion (4 New Suites)**:
  - Added `FEdrathAct4NhalSubterraneanMetropolisTest` verifying faction mediation, dead child prophecy, companion betrayal resolution, and Twist 3 ledger discovery.
  - Added `FEdrathAct5CrownOfWinterConsciousArchiveTest` verifying time dilation math, Conscious Archive infiltration, dialogue request, and ontological commitment.
  - Added `FEdrathAct6UnwrittenSeaAndFirstArchivistTest` verifying sky ocean traversal, former selves confrontation, multi-phase First Archivist boss battle, and ending resolution.
  - Added `FEdrathCompanionAutonomousAIControllerTest` verifying companion follow mode, foresight reaction, and Black Clock suspicion escalation.

## [0.9.0] - 2026-11-17

### Added
- **Rendering Pipeline, VRAM Budgets & Thermal Adaptation**:
  - Implemented dynamic thermal mitigation in `UEdrathHardwareOptimizationSubsystem` (`Nominal`, `Fair`, `Serious`, `Critical`) adjusting frame limits (`t.MaxFPS`), dynamic resolution scaling, and shadow distance to preserve mobile/laptop thermal envelopes.
  - Implemented `RecoverEmergencyVRAM()` routine shedding texture mips over budget (`r.Streaming.PurgeOverbudgetMips 1`), clamping directional virtual shadow map resolution bias, and calling `GEngine->TrimMemory()`.
  - Configured Unreal Engine 5.5 Pipeline State Object (PSO) precaching (`r.PSOPrecache=1`, `r.PSOPrecache.Validation=1`) in `Config/DefaultEngine.ini` to eliminate runtime transition hitching.
  - Configured asynchronous loading threads (`s.AsyncLoadingThreadEnabled=1`, `s.EventDrivenLoaderEnabled=1`) and aggressive garbage collection clustering (`gc.TimeBetweenPurgingPendingKillObjects=30`).
  - Added Vulkan pipeline cache and mobile memory compaction overrides in `Config/Android/AndroidEngine.ini`.
- **Act 3 Hollowgreen Physical Memory Realms & World Mutation Anchors**:
  - Implemented `AMemoryAnchorActor` (`Source/Edrath/Public/Narrative/MemoryAnchorActor.h`, `Source/Edrath/Private/Narrative/MemoryAnchorActor.cpp`) supporting physical interactive memory anchors in Hollowgreen memory sub-levels that trigger `UEdrathNarrativeSubsystem::RecordForestMutation()`.
  - Implemented `AAct3HollowgreenDirector` (`Source/Edrath/Public/World/Act3HollowgreenDirector.h`, `Source/Edrath/Private/World/Act3HollowgreenDirector.cpp`) orchestrating Act III (*The Forest of Your Other Lives*): discovery of the three alternate life echoes (`PlaguedHealer_Act3`, `TyrantConqueror_Act3`, `SimpleFarmer_Act3`), modular outfit preset synchronization, and the dramatic confrontation with Ilyra unlocking the loop attempt truth (`Story.Act3.LoopAttemptsRevealed`).
- **Comprehensive Automation Test Suite Expansion**:
  - Added `FEdrathAct3HollowgreenMemoryRealmTest` in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp` validating memory fruit harvesting, memory anchor alteration, forest mutation tracking, alternate life echo discovery, and Ilyra loop confrontation.
  - Added `FEdrathEnginePerformanceOptimizationTest` in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp` verifying emergency VRAM recovery thresholds, thermal mitigation state machine transitions, and PSO precache detection.

## [0.8.0] - 2026-11-15

### Added
- **Act 2 Kingdom of Veyl Royal Court & Belief Building Dissolve**:
  - Enhanced `ABeliefBuildingActor` (`Source/Edrath/Public/World/BeliefBuildingActor.h`, `Source/Edrath/Private/World/BeliefBuildingActor.cpp`) with continuous dynamic material dissolve parameter modulation (`DissolveAlpha`, `EdgeWidth`, `EdgeColor`) and collision threshold fading driven by collective citizen belief weight.
  - Implemented `AAct2VeylCourtDirector` (`Source/Edrath/Public/World/Act2VeylCourtDirector.h`, `Source/Edrath/Private/World/Act2VeylCourtDirector.cpp`) orchestrating Act II (*The Kingdom of Borrowed Faces*): Queen Serath royal court bargain, the three identical-face murder investigations (`Beggar`, `General`, `UnbornChild`), and Prince Aemon's erased memory truth revelation.
  - Added `FEdrathAct2VeylCourtInvestigationTest` in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp`.
- **Asynchronous Telemetry & Sentry Crash Diagnostics Architecture**:
  - Implemented `UEdrathTelemetrySubsystem` (`Source/Edrath/Public/Core/EdrathTelemetrySubsystem.h`, `Source/Edrath/Private/Core/EdrathTelemetrySubsystem.cpp`) supporting non-blocking queueing of narrative choices, memory mutations, regional clock instability heatmaps, ending distribution selections, and Sentry-compatible crash breadcrumb ring buffering.
  - Added `[Sentry.SentrySettings]` configuration block in `Config/DefaultEngine.ini`.
  - Added `FEdrathTelemetryAndCrashReportingTest` in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp`.

## [0.7.0] - 2026-11-15

### Added
- **Camera Movement & Terrain Slope Pitch Adaptation Polish**:
  - Implemented configurable camera translation and rotation lag in `UEdrathDualCameraComponent` (`bEnableCameraLag = true`, `CameraLagSpeed = 10.0f`, `bEnableCameraRotationLag = true`, `CameraRotationLagSpeed = 12.0f`) to prevent jarring snaps across bumpy terrain.
  - Implemented forward-predictive terrain slope pitch adaptation (`bEnableTerrainSlopeAdaptation = true`, `TerrainPitchCompensationScale = 0.35f`, `MaxTerrainPitchDeltaDegrees = 12.0f`) to keep the character centered and path ahead visible on steep inclines and declines.
  - Implemented quantized snap orbit yaw controls (`SnapExteriorYaw`) for 45-degree tactical increments.
  - Implemented dynamic camera impulse dampening (`PlayCameraImpulse`, `UpdateCameraImpulse`) for heavy attacks and landings.
- **Character Body Modular Rig Architecture Polish**:
  - Upgraded `UKaelModularRigComponent` (`Source/Edrath/Public/Character/KaelModularRigComponent.h`, `Source/Edrath/Private/Character/KaelModularRigComponent.cpp`) with full modular skeletal mesh slots (`Head`, `Torso`, `Arms`, `Legs`, `Cloak`) dynamically bound to the master body mesh via `SetLeaderPoseComponent`.
  - Added static mesh equipment socket management for `hand_r_weapon`, `hand_l_relic`, and `spine_03_holster` adhering to `Documentation/3D_Modeling_and_Art_Pipeline_Guide.md`.
  - Implemented `FKaelModularOutfitDefinition` presets for switching Kael's wardrobe across Life-Echo archetypes (`ForgottenSoldier_Act1`, `RoyalAssassin_Act2`, etc.).
  - Wired `UKaelModularRigComponent` directly into `AKaelCharacter` lifecycle and constructor.
- **Map Terrain & Foot IK Grounding Architecture**:
  - Implemented `UEdrathTerrainSubsystem` (`Source/Edrath/Public/World/EdrathTerrainSubsystem.h`, `Source/Edrath/Private/World/EdrathTerrainSubsystem.cpp`) providing surface classification (`EEdrathSurfaceType`: Stone, Dirt/Mud, Ashen Sand, Corpses, Snow, Water, Wood), slope calculation, surface-dependent friction scaling, and footstep audio/VFX routing.
  - Implemented `UKaelTerrainAdaptationComponent` (`Source/Edrath/Public/Character/KaelTerrainAdaptationComponent.h`, `Source/Edrath/Private/Character/KaelTerrainAdaptationComponent.cpp`) performing dual-foot raycasting to calculate smooth pelvis drops and foot orientation rotations for natural grounding on uneven hills, steps, and rubble.
  - Added `PhysicsCore` dependency to `Source/Edrath/Edrath.Build.cs`.
- **Comprehensive Automation Test Suite Extension**:
  - Added `FEdrathCameraTerrainElevationAndMotionTest` verifying camera lag, quantized snap yaw, and terrain adaptation.
  - Added `FEdrathModularRigAndTerrainIKTest` validating modular rig leader pose initialization, archetype swapping, equipment mounting, foot IK offsets, and physical material friction/audio routing.

## [0.6.0] - 2026-11-13

### Added
- **Phase 1: Dual-Perspective Camera & Transition Polish**:
  - Implemented dynamic collision sweep probe (`ProbeSphereRadius = 18.0f`) on `ECC_Camera` inside `UEdrathDualCameraComponent` to eliminate clipping into ceilings and door frames.
  - Implemented smooth doorway threshold alignment via spherical quaternion interpolation (`FQuat::Slerp`) and cubic ease-in-out alpha blending.
  - Implemented smooth dithered opacity fade (`RoofFadeDuration = 0.45f`, `RoofDitherOpacity`) in `AEdrathInteriorTransitionVolume`, replacing binary mesh popping.
- **Phase 2: Echo Combat & Timeline Sub-Frame Interpolation**:
  - Implemented continuous sub-frame linear (`FMath::Lerp`) and orientation (`FQuat::Slerp`) Hermite interpolation in `UBlackClockComponent::InterpolateFrameAtTime` across the 120-frame combat history buffer.
  - Implemented multi-hit sphere sweep physics collision detection (`PerformGhostStrikePhysicsSweep`) dealing echo temporal damage in `UEdrathEchoCombatComponent`.
  - Added parry window stability reward callback restoring Black Clock temporal energy upon successful defensive deflects.
- **Phase 3: Forensic Evidence & Contradiction Deduction Engine**:
  - Implemented dynamic visual chromatic aberration dispatch (`OnBreakthroughVisualFX`) and clue slot management (`ClearSlotA`, `ClearSlotB`) in `UForensicContradictionWidget`.
  - Enhanced `Shaders/MemoryEchoGhost.usf` with dynamic chromatic aberration glitch spikes for narrative breakthrough moments.
- **Phase 4: Pre-Alpha Act 1 Five-Zone Continuity Polish**:
  - Implemented discrete zone completion query predicates (`IsZone1Cleared` through `IsZone5Cleared`, `IsPreAlphaCompleted`) in `AAct1PreAlphaSliceDirector`.
  - Synchronized Zone 2 Collapsed Gorge Bridge reality swap with `Shaders/PossibilityDissolve.usf`.
- **Phase 5: 8GB VRAM Guardian & Rendering Polish**:
  - Implemented multi-tiered dynamic VRAM sentinel in `UEdrathHardwareOptimizationSubsystem`: Level 1 Caution at 80% and Level 2 Emergency Throttling at 90% (7.2 GB ceiling).
  - Throttles texture streaming pool (`r.Streaming.PoolSize`), Nanite pool, and VSM physical pages to protect the RTX 5070 8GB laptop hardware envelope.
- **Phase 6: Comprehensive Automation & Invariant Validation**:
  - Added full automation test suites in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp`:
    - `FEdrathDualCameraAntiClippingAndRoofFadeTest`
    - `FEdrathEchoCombatSubFrameInterpolationTest`
    - `FEdrathForensicContradictionDeductionTest`
    - `FEdrathAct1FiveZoneContinuityTest`
    - `FEdrathHardwareVRAMBudgetGuardianTest`

## [0.5.0] - 2026-11-11

### Added
- **Stage 2 Pre-Alpha Playable Vertical Slice (*The Man Who Was Never Born*)**:
  - **Zone 1: Ancient Battlefield Awakening**:
    - Implemented `ABattlefieldWraithCharacter` (`Source/Edrath/Public/Character/BattlefieldWraithCharacter.h`, `Source/Edrath/Private/Character/BattlefieldWraithCharacter.cpp`) featuring dual timeline phase shield and vulnerability to Kael's 120-frame Echo Strikes (`TriggerEchoStrike(1.2f)`).
    - Implemented `ADyingSoldierActor` (`Source/Edrath/Public/World/DyingSoldierActor.h`, `Source/Edrath/Private/World/DyingSoldierActor.cpp`) broadcasting the iconic dialogue (*"Do not let my daughter be born"*) and registering initial forensic evidence.
  - **Zone 2: Collapsed Gorge Bridge**:
    - Implemented `AAct1GorgeBridgeActor` (`Source/Edrath/Public/World/Act1GorgeBridgeActor.h`, `Source/Edrath/Private/World/Act1GorgeBridgeActor.cpp`) with proximity chasm detection and Possibility Shift restoration from shattered ruins to an intact monolithic stone archway.
  - **Zone 3: 7-Day Looping Settlement**:
    - Implemented `ASettlementVillagerCharacter` (`Source/Edrath/Public/Character/SettlementVillagerCharacter.h`, `Source/Edrath/Private/Character/SettlementVillagerCharacter.cpp`) reacting dynamically to `OnSettlementLoopReset` with contextual amnesia dialogue branches across Days 1–6, Day 7 dread, and broken loop liberation.
  - **Zone 4: Kael's Settlement House (Interior Inspection)**:
    - Implemented `AKaelHouseInteriorActor` (`Source/Edrath/Public/World/KaelHouseInteriorActor.h`, `Source/Edrath/Private/World/KaelHouseInteriorActor.cpp`) handling real-time exterior roof hiding, seamless OTS third-person camera transitions, and forensic desk handwriting and scratched woman portrait clues.
  - **Zone 5: Local Crypt & The 114-Year-Dead Priest**:
    - Implemented `APriestCorpseActor` (`Source/Edrath/Public/World/PriestCorpseActor.h`, `Source/Edrath/Private/World/PriestCorpseActor.cpp`) combining skeletal sarcophagus forensic inspection with `AMemoryReconstructionAnchor` to shatter the 7-day settlement loop and expose the priest's memory echo.
  - **Master Level Script & World Partition Orchestration**:
    - Implemented `AAct1BattlefieldLevelScriptActor` (`Source/Edrath/Public/World/Act1BattlefieldLevelScriptActor.h`, `Source/Edrath/Private/World/Act1BattlefieldLevelScriptActor.cpp`) binding all 5 zones, director, and completion state.
    - Added level manifest (`Content/Maps/PreAlpha_Act1/L_PreAlpha_Act1_Manifest.json`) and World Partition Data Layers (`DataLayer_PresentTimeline.json`, `DataLayer_RejectedTimeline.json`, `DataLayer_MemoryEcho.json`).
  - **Automated Verification Test**:
    - Added `FEdrathStage2ComprehensiveVerticalSlicePlaythroughTest` in `Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp` validating all 5 zones end-to-end.

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
