# Edrath: The World That Forgot Tomorrow — Master Technical Architecture & GitHub Copilot Implementation Bible

**Edrath: The World That Forgot Tomorrow** is an open-world dark fantasy, mystery, and psychological thriller Action RPG built in [Unreal Engine 5](https://www.unrealengine.com/unreal-engine-5) using C++20 and Blueprints. Every kingdom remembers a different version of history, every NPC remembers a different version of the protagonist (**Kael Veyr**), and the camera dynamically transitions between an **angled bird's-eye isometric view outdoors** and an **immersive third-person over-the-shoulder perspective inside buildings, crypts, and dungeons**.

This document is structured as a strict, deterministic specification for **GitHub Copilot** (Agent Mode / Edits / Chat) and human engineers. It enforces a two-stage engineering workflow:
1. **Stage 1 — Core Architectural Base (`Source/EdrathCore`)**: Build all foundational C++ subsystems, dual-perspective camera controllers, skeletal rigging sockets, Black Clock reality-shifting managers, and narrative state machines first with zero broken dependencies.
2. **Stage 2 — Working Pre-Alpha Playable Vertical Slice (`Content/Maps/PreAlpha_Act1`)**: Assemble and test a complete, playable Pre-Alpha build covering Act I (*The Man Who Was Never Born*), including the corpse-strewn battlefield, the 7-day looping settlement, exterior-to-interior camera transitions, Echo Combat, Possibility Shifting, and the 114-year-dead priest memory investigation.

## 1. Engine Selection & Visual Fidelity Justification: Unreal Engine 5 (C++20 + Blueprints)

To render clean, smooth character figures, seamless skeletal morphs, atmospheric dark-fantasy lighting, and zero-loading-screen reality shifts across an 80-hour open world, this project standardizes on **[Unreal Engine 5](https://www.unrealengine.com/unreal-engine-5) (UE 5.5+ / 5.8 C++20 & Blueprint Hybrid Architecture)**.

| Engine Technology | Specific Role in *Edrath: The World That Forgot Tomorrow* | Technical Configuration |
| :--- | :--- | :--- |
| **[Lumen Global Illumination & Reflections](https://dev.epicgames.com/documentation/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine)** | Renders real-time dynamic bounce lighting when Kael shifts between the Present Timeline and Rejected Futures, or transitions from sunlit exterior bird's-eye views into pitch-dark interior crypts. | Hardware Ray Tracing enabled where supported; Software Ray Tracing fallback with Detail Tracing (`r.Lumen.ScreenProbeGather.ScreenTraces 1`). |
| **[Nanite Virtualized Micropolygon Geometry](https://www.unrealengine.com/blog/understanding-nanite---unreal-engine-5-s-new-virtualized-geometry-system)** | Renders multi-million-polygon gothic cathedrals in Veyl, organic root structures in Hollowgreen, and carved subterranean caverns in Nhal with smooth silhouette edges and zero LOD pop-in. | Enabled on all static architectural meshes, terrain cliffs, and ruin kits (`bEnableNanite = true`). |
| **[World Partition & Data Layers](https://altheragames.com/en/blog/ue5-world-partition-guide)** | Powers **Possibility Shifting** by keeping both the *Present Timeline* and *Rejected Timeline* loaded in memory within the same spatial grid cell, toggling collision and shader opacity in real time. | 128m x 128m runtime grid cell size; **One File Per Actor (OFPA)** enabled to prevent Git merge conflicts. |
| **[Virtual Shadow Maps (VSM)](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5.0-release-notes?application_version=5.0)** | Delivers crisp contact-hardening shadows on Kael's character model in close-up interior third-person views while allowing per-actor shadow suppression for the shadowless inhabitants of *The Ashen Expanse*. | DirectX 12 (`DefaultGraphicsRHI=DefaultGraphicsRHI_DX12`), `r.Shadow.Virtual.Enable 1`. |
| **TSR (Temporal Super Resolution) & Control Rig** | Produces clean, alias-free character silhouettes and smooth procedural IK foot placement on uneven rubble, spiral stairs, and floating ship decks. | TSR Anti-Aliasing with full-body Control Rig foot-locking and spine-aiming IK. |

## 2. GitHub Student Developer Pack Toolchain & DevOps Pipeline

Every tool below is available at zero cost through the **[GitHub Student Developer Pack](https://education.github.com/pack)** and is integrated directly into the *Edrath* production pipeline:

| Student Pack Benefit | Role in *Edrath* Development | Exact Project Integration |
| :--- | :--- | :--- |
| **[JetBrains Rider (Free Student License)](https://www.jetbrains.com/lp/rider-unreal/)** | Primary C++20 & Blueprint IDE for Unreal Engine. Opens `.uproject` directly, indexes UE reflection macros (`UCLASS`, `UPROPERTY`, `UFUNCTION`), inspects Blueprint references inline, and runs Live Coding. | Install the **[UnrealLink / RiderLink Plugin](https://plugins.jetbrains.com/plugin/14989-unreallink)** inside `Plugins/Developer/RiderLink`. Enable auto-generation of `.generated.h` includes. |
| **[GitHub Copilot Pro (Free for Students)](https://github.com/achoarnold/discount-for-student-dev)** | AI pair-programmer inside JetBrains Rider and VS Code. Reads `.github/copilot-instructions.md` and this `README.md` to generate UE5 C++ classes, shaders, and automated tests in strict architectural order. | Enable Copilot Agent Mode in JetBrains Rider (`Settings -> Tools -> GitHub Copilot`). Pin `README.md` to the workspace context. |
| **[GitHub Pro & Git Large File Storage (Git LFS)](https://git-lfs.com/)** | Unlimited private repositories and binary asset pointer tracking for `.uasset`, `.umap`, `.fbx`, and `.png` files alongside UE5 One File Per Actor (OFPA). | Configure `.gitattributes` to lock `.uasset` and `.umap` files (`lockable`) so binary assets never corrupt. |
| **[GitKraken Pro Client & Boards](https://github.blog/news-insights/welcome-algolia-gitkrakenglo-heroku-jetbrains/)** | Visual Git commit graph, LFS file locking dashboard, and milestone Kanban boards synced directly with GitHub Issues for the Base Architecture and Pre-Alpha milestones. | Connect GitKraken Pro to the private GitHub repository and manage LFS locks before editing `.umap` files. |
| **[Sentry for Unreal Engine (Free Student Plan)](https://docs.sentry.io/platforms/unreal/)** | Automated native C++ crash dump capture (`sentry-unreal` SDK), call-stack symbolication, and session replay leading up to Pre-Alpha crashes. | Add the **[Sentry Unreal Engine SDK](https://github.com/getsentry/sentry-unreal)** to `Plugins/Sentry` and enable `Sentry Crash Reporter` in `DefaultEngine.ini`. |
| **[MongoDB Atlas ($50 Credit & Free Tier)](https://mimalla.com/en/student-benefits)** | Cloud telemetry store for recording Pre-Alpha playtester choices across the 6 Acts, witness memory mutations, regional clock instability heatmaps, and the 4 Endings. | Connect via an asynchronous HTTP REST subsystem (`UEdrathTelemetrySubsystem`) during Pre-Alpha playtests. |
| **[Heroku ($13/mo for 24 Months) & Docker Desktop Pro](https://mimalla.com/en/student-benefits)** | Hosts the lightweight containerized Telemetry & Save-State Validation API that aggregates player world-state hashes and Ending D global distribution choices. | Deploy a Dockerized FastAPI/Node telemetry receiver connected to MongoDB Atlas. |
| **[1Password (1 Year Free Developer Tools)](https://mimalla.com/en/student-benefits)** | Secures Sentry DSN keys, MongoDB Atlas connection strings, and build-signing certificates via the 1Password CLI (`op run`). | Store secrets in `.env.vault` referenced only at CI/CD build time, never committed to Git. |
| **[Termius Pro SSH Client](https://github.com/Elele-Group/free-for-students)** | Manages remote SSH sessions to build servers and cloud telemetry containers across desktop and mobile. | Configure SSH keys synced via 1Password SSH Agent. |

### Mandatory `.gitattributes` Configuration for Unreal Engine 5 + Git LFS

Create this exact `.gitattributes` file in the repository root before committing any Unreal Engine assets:

```gitattributes
# Unreal Engine Binary Assets (Managed via Git LFS with File Locking)
*.uasset filter=lfs diff=lfs merge=lfs -text lockable
*.umap filter=lfs diff=lfs merge=lfs -text lockable

# Raw 3D Art, Rigging, Audio, and Texture Sources
*.fbx filter=lfs diff=lfs merge=lfs -text
*.obj filter=lfs diff=lfs merge=lfs -text
*.blend filter=lfs diff=lfs merge=lfs -text
*.png filter=lfs diff=lfs merge=lfs -text
*.tga filter=lfs diff=lfs merge=lfs -text
*.exr filter=lfs diff=lfs merge=lfs -text
*.hdr filter=lfs diff=lfs merge=lfs -text
*.wav filter=lfs diff=lfs merge=lfs -text
*.ogg filter=lfs diff=lfs merge=lfs -text
*.mp4 filter=lfs diff=lfs merge=lfs -text

# Source Code & Configs (Standard Line Endings)
*.h text eol=lf
*.cpp text eol=lf
*.cs text eol=lf
*.ini text eol=lf
*.usf text eol=lf
*.ush text eol=lf
*.md text eol=lf
```

### Mandatory `.github/copilot-instructions.md` Configuration

Create `.github/copilot-instructions.md` with the following rules so GitHub Copilot never deviates from the project architecture:

```markdown
# GitHub Copilot Instructions for Edrath: The World That Forgot Tomorrow
1. ENGINE & STANDARD: Target Unreal Engine 5.5+ using C++20 (`CppStandardVersion.Cpp20`). Expose all designer-tunable properties with `UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|...")` and `UFUNCTION(BlueprintCallable, Category="Edrath|...")`.
2. ARCHITECTURAL ORDER: Never generate level scripts, quests, or UI widgets until the Phase 1 Core Base classes (`AEdrathCharacterBase`, `UEdrathDualCameraComponent`, `UBlackClockComponent`, `UEdrathEchoCombatComponent`, `UKaelModularRigComponent`, `UEdrathNarrativeSubsystem`, `AEdrathInteriorTransitionVolume`) compile cleanly.
3. DUAL CAMERA PERSPECTIVE: Exterior exploration MUST use `EEdrathCameraMode::ExteriorBirdsEye` (Pitch `-38.0f`, Yaw `45.0f`, TargetArmLength `1150.0f`, FOV `28.0f`, `bOrientRotationToMovement = true`). Entering any building, crypt, or interior volume (`AEdrathInteriorTransitionVolume`) MUST smoothly blend via `FMath::FInterpTo` to `EEdrathCameraMode::InteriorThirdPerson` (Pitch `-12.0f`, TargetArmLength `240.0f`, SocketOffset `(0.0f, 65.0f, 45.0f)`, FOV `75.0f`, `bUseControllerRotationYaw = true`) while hiding exterior roof meshes.
4. THE BLACK CLOCK: All timeline mutations, Memory Reconstructions, Possibility Shifts, and Echo Combat actions must route through `UBlackClockComponent` and increment regional instability (`RegionalInstabilityMap`) inside `UEdrathNarrativeSubsystem`.
5. NO PLACEHOLDERS: Write complete, compilable C++ headers (`.h`) and implementations (`.cpp`) with all required `#include` directives and `ModuleRules` dependencies (`Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `Niagara`, `UMG`, `GameplayTags`).
```

## 3. Complete Story, World Rules, Acts, Twists & Quest Specifications

GitHub Copilot must represent every narrative concept below as concrete gameplay state variables, `FGameplayTag` identifiers, and world mutation rules.

### Core Premise & Protagonist Identity
* **The Defect in Reality**: In the continent of **Edrath**, tomorrow has stopped arriving properly. Events vanish from history before they occur: villages disappear from maps overnight, murdered kings are forgotten by their subjects, and mothers forget their children while looking directly at them.
* **Protagonist — Kael Veyr**: A nameless wanderer who awakens inside an ancient battlefield where both armies have been dead for hundreds of years. Embedded in Kael's sternum (`clavicle_center -> bone_chest_clock`) is a ticking **Black Clock**. While everyone else experiences linear time, Kael experiences the moments that time has rejected.
* **Core Promise**: Every kingdom remembers a different version of history, and every NPC remembers a different version of Kael (soldier, royal assassin, plague healer, tyrant conqueror, farmer, betrayer). The deeper Kael investigates, the less certain the player becomes that Edrath should be saved.

### The Six Regions of Edrath & Their Reality Laws

| Region ID & Name | Target Playtime | Reality Defect & Lore Rule | Engine & Gameplay Implementation Specification |
| :--- | :--- | :--- | :--- |
| **1. `Region.Veyl`** — *The Kingdom of Veyl (The City That Remembers)* | Hours 1–12 | A capital where every citizen remembers a different king, buildings exist only when people believe in them, and the royal family has ruled for generations despite having zero ancestors. | `ABeliefBuildingActor` queries `UEdrathNarrativeSubsystem::GetCitizenBeliefWeight(BuildingTag)`. When belief drops below threshold `0.5f`, the building disables physics collision and dissolves via `M_PossibilityDissolve`. |
| **2. `Region.Hollowgreen`** — *The Hollowgreen (The Forest That Dreams)* | Hours 10–25 | A bioluminescent forest where trees grow physical memory fruits. Kael enters memories as physical 3D sub-levels; altering a past memory mutates the present world. | `AMemoryFruitTreeActor` streams additive level instances (`ULevelStreamingDynamic`). Modifying an `AMemoryAnchorActor` inside the memory updates `FWorldMutationState` in the parent forest. |
| **3. `Region.AshenExpanse`** — *The Ashen Expanse (The Kingdom That Never Existed)* | Hours 20–38 | A desert of ruins from an erased civilization. Inhabitants cast zero shadows, and the oldest tombs contain the corpses of people currently alive in other regions. | `AAshenNPCCharacter` forces `GetMesh()->SetCastShadow(false)` and `bCastDynamicShadow = false`. `ATombSarcophagusActor` links via `FGuid` to living NPCs; disturbing a tomb mutates the living NPC's dialogue and memories. |
| **4. `Region.Nhal`** — *Nhal (The City Beneath Death)* | Hours 32–50 | A vertical subterranean metropolis populated by people who died in futures that never happened. They remember their deaths, families, and murderers—and some recognize Kael immediately. | Multi-tiered cave World Partition grid with faction reputation (`Faction.Nhal.Restorationists` vs `Faction.Nhal.Revanchists`). Spawns future-dead echoes of Kael's active living companions. |
| **5. `Region.CrownOfWinter`** — *The Crown of Winter (The Place Outside Time)* | Hours 45–62 | A frozen mountain realm where a single conversation inside a room lasts years outside, while only seconds pass indoors. Ancient inhabitants have watched Kael arrive thousands of times. | `ATimeDilationInteriorVolume` binds to `AEdrathInteriorTransitionVolume`. Spending `N` seconds in third-person interior mode advances exterior weather, snow accumulation, and NPC age states by `N * 365` simulated days. |
| **6. `Region.UnwrittenSea`** — *The Unwritten Sea (The Edge of Creation)* | Hours 58–80 | An ocean suspended above a bottomless sky where ships sail through abandoned possibilities and islands materialize from decisions the player never made. | `AUnwrittenIslandSpawner` reads unchosen dialogue/quest branches from `UEdrathNarrativeSubsystem::UnchosenPossibilityTags` and dynamically spawns islands representing the roads not taken. |

### The Six Acts — Complete Narrative & Systems Mapping (70–80 Hours Total)

| Act & Title | Playtime Window | Plot Sequence & Investigation Mechanics | Act-Ending Twist & State Flag |
| :--- | :--- | :--- | :--- |
| **Act I — *The Man Who Was Never Born*** | Hours 1–10 | Kael wakes on a battlefield beside thousands of corpses. A dying soldier calls him by an unfamiliar name and begs him not to let his daughter be born. Kael reaches a nearby settlement where nobody remembers the previous 7 days, yet villagers insist he has lived there for years and show him his house, his handwriting, and a portrait beside a woman with a scratched-out face. The settlement priest invites Kael into the local crypt to explain—where Kael finds the priest's corpse, dead for 114 years. | **Twist**: The priest has been speaking from a memory, not the present. The settlement is trapped in a repeating 7-day loop (`bSettlementLoopActive = true`), and Kael is the only person who can leave it (`Story.Act1.CryptTwistRevealed`). |
| **Act II — *The Kingdom of Borrowed Faces*** | Hours 10–24 | In the capital of Veyl, **Queen Serath** recognizes Kael as the assassin who murdered her son. She offers a bargain: investigate a sequence of impossible murders where every victim shares the exact same face—a beggar, a general, and an unborn child. Kael interviews witnesses, reconstructs memories, and uncovers an ancient court conspiracy. | **Twist**: The prince was never murdered; Queen Serath ordered the kingdom to forget his existence because he discovered the truth about her throne. When found, the prince refuses to return and reveals Queen Serath is not the villain Kael thinks she is (`Story.Act2.PrinceFound`). |
| **Act III — *The Forest of Your Other Lives*** | Hours 22–39 | In Hollowgreen, Kael explores physical memory realms of his own radically different lives: a healer who saved a plague-stricken continent, a tyrant who burned every kingdom, and a simple farmer with a forgotten family. Kael meets **Ilyra**, who claims to be his wife, remembers their arguments, and remembers killing him. | **Twist**: Kael's alternate lives are not random parallel universes; each was a sequential attempt to stop the same catastrophe, and every attempt failed for a different reason. Ilyra killed him to stop him from repeating his worst mistake (`Story.Act3.LoopAttemptsRevealed`). |
| **Act IV — *The Dead Who Remember Tomorrow*** | Hours 38–53 | Beneath Edrath in Nhal, the dead remember unrealized futures. Factions war over restoration vs. vengeance against the living. Kael meets future-dead versions of his active companions, a dead child reveals how Kael's journey ends, and a former ally turns hostile because her future memory shows Kael betraying her. | **Twist**: The catastrophe is not erasing the past—it is deleting possible futures before the world can choose them. Every time Kael uses the Black Clock to alter a memory, he permanently prunes possible futures (`ClockUsagePrunedFuturesCount++`). |
| **Act V — *The God Who Asked to Die*** | Hours 51–66 | In the Crown of Winter, Kael meets the world's creator: not a dragon or king, but a **Conscious Archive** holding every living person's memories. Built to stop an ancient civilization from discovering that consciousness can exist without a fixed timeline, the Archive prunes futures to prevent total reality collapse as civilization grows more complex. | **Twist**: The Archive has been shrinking reality to prevent an even worse ontological collapse, and it asks Kael to kill it. Killing it unleashes every possible future at once; sparing it lets reality shrink until nothing remains (`Story.Act5.ArchiveRequest`). |
| **Act VI — *The World That Forgot Tomorrow*** | Hours 64–80 | Sailing the Unwritten Sea, Kael confronts his former selves, revisits mutated regions, and weighs three incompatible philosophies (**Preservation**, **Liberation**, **Acceptance**) alongside a hidden fourth path unlocked by evidence. Companions reveal whether they were protecting or manipulating him. | **Twist**: The final revelation uncovers who Kael was before the opening battlefield scene—the architect of the Archive's failsafe itself (`Story.Act6.TrueIdentityRevealed`). |

### The Six Core Twists & Foreshadowing Architecture
Every twist must be foreshadowed via inspectable clues tracked in `UEdrathNarrativeSubsystem::CollectedClues`:
1. **Twist 1 — Your memories are evidence, not truth**: Memory reconstructions play from Kael's camera perspective, leading the player to assume they are Kael's past. Subtle audio/visual discrepancies (reflection mismatches in mirrors, voice pitch offsets, handedness differences) foreshadow that Kael is experiencing *other people's* memories absorbed by the Black Clock.
2. **Twist 2 — The companion who saves you has been killing you**: **Ilyra** rescues Kael from ambushes throughout Acts I–III, but forensic clues at murder scenes (executioner's blade notch patterns, specific alchemical poison residues) match Ilyra's inventory—she has been executing witnesses who remember catastrophic versions of Kael.
3. **Twist 3 — The dead aren't the victims you assumed**: In Nhal, ledger records in the Archive reveal that several erased citizens were warmongers or patient-zero carriers whose existence would have prevented billions of current citizens from ever being born. Restoring a Nhal NPC via the Black Clock despawns a corresponding living family in Veyl.
4. **Twist 4 — The final boss may be your best outcome**: The antagonist version of Kael (*The First Archivist*) fought in the endgame erected the reality-pruning wards to keep Edrath from dissolving into non-linear chaos; defeating him strips those safeguards in real time.
5. **Twist 5 — Your choices have been changing the evidence**: Every side quest and memory edit mutates `EvidenceRegistry`. Saving a witness overwrites their testimony; restoring a ruined building replaces the physical evidence inside it.
6. **Twist 6 — The last mystery isn't who created the world**: The climax evaluates whether a preserved, pruned world is genuinely alive, governed by the player's accumulated evidence and moral choices.

### Companions & Dynamic Autonomous Agency
Companions are autonomous agents (`AEdrathCompanionCharacter`) governed by `FCompanionRelationshipState` (`Loyalty`, `Suspicion`, `KnowledgeUnlocked`, `PersonalGoalProgress`). They remember every Black Clock activation, can withhold clues, develop inter-companion rivalries, abandon the party, or become bosses:
* **Ilyra — *The Woman Who Remembers Killing You***: Former royal executioner who believes certain truths must stay buried. Weapon: Executioner's Greatsword. Quest outcomes: `Reconciliation`, `Imprisonment`, or `LethalConfrontation`.
* **Orren — *The Historian With No Past***: Scholar with total amnesia regarding his childhood who recalls histories of non-existent civilizations. Assists in Memory Reconstruction puzzles, but his personal quest reveals his scholarship actively overwrites historical evidence as he records it.
* **Senn — *The Boy From Tomorrow***: Young wanderer who remembers unlived futures. Appears innocent, provides cryptic combat warnings (`0.8s` incoming attack foresight), and possesses intimate knowledge of the Act VI final choice before Kael arrives there.
* **Maevra — *The Enemy Who Joins You***: Royal court alchemist who hunts Kael in Act II as an elite boss/pursuer before joining the party solely to use Kael's Black Clock to restore a spouse she accidentally erased from history—claiming a forty-year marriage that no longer exists.

### Side Quests With Reality-Altering Consequences
Each side quest is implemented as a multi-outcome state machine (`UEdrathQuestDefinition`) with no trivial "correct" answer:
1. **`Quest.WeddingNotHappened` (*The Wedding That Hasn't Happened*)**: A village prepares a wedding for two strangers who both remember forty years of marriage to each other, despite never having met in the present timeline. Choices: merge their timelines (erasing their current families) or sever the memory echo.
2. **`Quest.BuriedFuture` (*The Man Who Buried His Future*)**: A farmer unearths a grave on his land containing his own corpse dated 30 years in the future. Exhuming the body immediately mutates his wife and children's memories of whether he is a loving father or a violent stranger.
3. **`Quest.VillageVotesOnDead` (*The Village That Votes on Its Dead*)**: Once a year, villagers vote to resurrect one deceased citizen, which causes one living villager to vanish retroactively from history. Kael must investigate a rigged vote where the mayor's accuser was swapped out of existence.
4. **`Quest.DragonNoFire` (*A Dragon With No Fire*)**: An ancient dragon in the mountain pass has forgotten how to breathe fire or destroy; its hoard consists of crystallized memories stolen from people across Edrath. Returning the memories restores the citizens' minds—and restores the dragon's apocalyptic nature.
5. **`Quest.LastHonestKing` (*The Last Honest King*)**: A ruler publicly confesses to war crimes he never committed because the true perpetrator was erased from history, leaving the crimes unanchored. Exposing the truth clears the king's name but reignites a devastating civil war.
6. **`Quest.HouseOneMoreRoom` (*The House With One More Room*)**: An abandoned house spawns a new procedural interior room (`AHouseWithOneMoreRoomActor`) every time an NPC recalls its forgotten owner. Navigating the expanding third-person interior maze leads to a final locked room holding the atrocity the owner paid to erase.

### The Four Endings (`EEdrathEndingType`)
* **Ending A — *The Preserved World* (`EEdrathEndingType::PreservedWorld`)**: Kael maintains the Conscious Archive. Reality remains stable and visually serene, but anomalous futures continue to be erased and companions remember Kael through peaceful, sanitized falsehoods.
* **Ending B — *The Unwritten World* (`EEdrathEndingType::UnwrittenWorld`)**: Kael destroys the Archive. Countless pruned futures flood back simultaneously; erased citizens from Nhal return while certain living NPCs vanish into alternate timelines. Total freedom is achieved at the cost of ontological unpredictability.
* **Ending C — *The Mortal World* (`EEdrathEndingType::MortalWorld`)**: Kael dismantles the Archive's selection mechanism gradually (`Acceptance`). Time becomes finite and irreversible again—the Black Clock stops ticking, the dead stay dead, and tomorrow becomes genuinely unknown.
* **Ending D — *The Uncomfortable Truth* (`EEdrathEndingType::UncomfortableTruth`)**: Unlocked only if `UEdrathNarrativeSubsystem::HasUnlockedFourthPath()` returns `true` (requiring all 6 core twist clues and reconciled witness contradictions). Kael distributes the Archive's reality-choosing power across all inhabitants of Edrath, forcing a flawed humanity—capable of selfishness and conflict—to govern reality rather than a machine.

## 4. Dual-Perspective Camera & Transition Architecture (Bird's-Eye Exterior ↔ Third-Person Interior)

A core requirement of *Edrath* is seamless dual-perspective gameplay:
* **Outdoors (Overworld, Battlefields, Streets of Veyl, Hollowgreen Trails, Unwritten Sea Decks)**: Angled **Bird's-Eye / Isometric Perspective** (`Pitch: -38.0°`, `Yaw: 45.0°`, `TargetArmLength: 1150.0f`, `FOV: 28.0°` telephoto isometric projection) giving tactical visibility over surrounding enemies, terrain splines, and expanding Possibility Shift shockwaves.
* **Indoors (Settlement Houses, Crypts, Veyl Palaces, The House With One More Room, Crown of Winter Chambers)**: Immersive **Over-the-Shoulder Third-Person Perspective** (`Pitch: -12.0°` with full mouse/stick look `-65.0°` to `+55.0°`, `TargetArmLength: 240.0f`, `SocketOffset: (0.0f, 65.0f, 45.0f)`, `FOV: 75.0f`) allowing intimate forensic inspection of portraits, handwriting, corpses, and claustrophobic interior combat.

| Camera Parameter | `ExteriorBirdsEye` Mode (Outdoors) | `InteriorThirdPerson` Mode (Inside Buildings/Crypts) | Transition Blend Behavior |
| :--- | :--- | :--- | :--- |
| **SpringArm Length (`TargetArmLength`)** | `1150.0f` cm (Zoomable `850.0f` – `1450.0f`) | `240.0f` cm (Dynamic collision probe `120.0f` – `260.0f`) | Cubic ease-in-out interpolation (`InterpSpeed = 4.5f` over `0.65s`). |
| **Camera Pitch & Yaw Control** | Fixed `-38.0°` Pitch, `45.0°` Yaw (or 90° snap rotation via Q/E). | Free 3D Controller Yaw & Pitch (`-65.0°` to `+55.0°`) driven by Mouse/Right Stick. | Smooth quaternion slerp (`FQuat::Slerp`) aligning camera forward with doorway entry vector. |
| **Field of View (`FOV`)** | `28.0°` (Flattened isometric-style telephoto perspective with minimal distortion). | `75.0°` (Natural human-eye over-the-shoulder perspective for interior depth). | Interpolated via `UCameraComponent::SetFieldOfView` every tick during transition. |
| **SpringArm Socket Offset** | `FVector(0.0f, 0.0f, 0.0f)` (Centered on Kael's pelvis/sternum). | `FVector(0.0f, 65.0f, 45.0f)` (Over right shoulder, framing Kael and the ticking chest clock). | Smooth `FMath::VInterpTo` vector blend. |
| **Character Movement Orientation** | `bOrientRotationToMovement = true`, `bUseControllerRotationYaw = false`. | `bOrientRotationToMovement = false` (strafe-enabled in combat/aim) or contextual free-look. | Switched atomically when transition alpha crosses `0.5f`. |
| **Roof & Ceiling Geometry** | Exterior roofs visible; occlusion raycast dithers trees/walls blocking Kael. | Exterior roof hides (`SetActorHiddenInGame(true)`); interior ceilings & chandeliers render in full 3D. | Triggered by `AEdrathInteriorTransitionVolume::OnOverlapBegin` / `OnOverlapEnd`. |

## 5. Complete Project Repository & Directory Structure

GitHub Copilot must generate and organize all files strictly inside this directory layout:

```text
Edrath/
├── .gitattributes
├── .gitignore
├── .github/
│   ├── copilot-instructions.md
│   └── workflows/
│       └── ue5-syntax-and-test-check.yml
├── Edrath.uproject
├── Config/
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   ├── DefaultInput.ini
│   └── DefaultScalability.ini
├── Source/
│   ├── Edrath.Target.cs
│   ├── EdrathEditor.Target.cs
│   └── Edrath/
│       ├── Edrath.Build.cs
│       ├── EdrathModule.h
│       ├── EdrathModule.cpp
│       ├── Public/
│       │   ├── Core/
│       │   │   ├── EdrathTypes.h
│       │   │   ├── EdrathGameplayTags.h
│       │   │   └── EdrathGameInstance.h
│       │   ├── Camera/
│       │   │   ├── EdrathDualCameraComponent.h
│       │   │   └── EdrathInteriorTransitionVolume.h
│       │   ├── Character/
│       │   │   ├── KaelCharacter.h
│       │   │   ├── KaelModularRigComponent.h
│       │   │   └── EdrathCompanionCharacter.h
│       │   ├── BlackClock/
│       │   │   ├── BlackClockComponent.h
│       │   │   ├── EchoCombatComponent.h
│       │   │   └── PossibilityShiftActor.h
│       │   ├── World/
│       │   │   ├── BeliefBuildingActor.h
│       │   │   ├── HouseWithOneMoreRoomActor.h
│       │   │   └── TimeLoopSettlementManager.h
│       │   ├── Narrative/
│       │   │   ├── EdrathNarrativeSubsystem.h
│       │   │   └── MemoryReconstructionAnchor.h
│       │   └── Tests/
│       │       └── EdrathPreAlphaAutomationTest.h
│       └── Private/
│           ├── Core/
│           │   ├── EdrathGameplayTags.cpp
│           │   └── EdrathGameInstance.cpp
│           ├── Camera/
│           │   ├── EdrathDualCameraComponent.cpp
│           │   └── EdrathInteriorTransitionVolume.cpp
│           ├── Character/
│           │   ├── KaelCharacter.cpp
│           │   ├── KaelModularRigComponent.cpp
│           │   └── EdrathCompanionCharacter.cpp
│           ├── BlackClock/
│           │   ├── BlackClockComponent.cpp
│           │   ├── EchoCombatComponent.cpp
│           │   └── PossibilityShiftActor.cpp
│           ├── World/
│           │   ├── BeliefBuildingActor.cpp
│           │   ├── HouseWithOneMoreRoomActor.cpp
│           │   └── TimeLoopSettlementManager.cpp
│           ├── Narrative/
│           │   ├── EdrathNarrativeSubsystem.cpp
│           │   └── MemoryReconstructionAnchor.cpp
│           └── Tests/
│               └── EdrathPreAlphaAutomationTest.cpp
├── Content/
│   ├── Characters/
│   │   ├── Kael_MasterRig/
│   │   └── ModularOutfits/
│   ├── Shaders/
│   │   ├── M_PossibilityDissolve.uasset
│   │   ├── M_MemoryEchoGhost.uasset
│   │   └── M_Customizer_PackedRGBA.uasset
│   └── Maps/
│       └── PreAlpha_Act1/
│           ├── L_PreAlpha_Act1_BattlefieldAndCrypt.umap
│           └── DataLayers/
└── README.md
```

## 6. Stage 1 — Core Architectural Base (Full Compilable C++ Specifications)

GitHub Copilot must generate the following Stage 1 foundational files **first** before building any level or quest assets. Every class below is complete and self-contained.

### 6.1 Module Build Configuration (`Source/Edrath/Edrath.Build.cs`)

```csharp
// File: Source/Edrath/Edrath.Build.cs
using UnrealBuildTool;

public class Edrath : ModuleRules
{
    public Edrath(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTags",
            "Niagara",
            "UMG",
            "AIModule",
            "NavigationSystem"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "RenderCore",
            "RHI"
        });
    }
}
```

### 6.2 Core Enumerations & Data Structures (`Source/Edrath/Public/Core/EdrathTypes.h`)

```cpp
// File: Source/Edrath/Public/Core/EdrathTypes.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EdrathTypes.generated.h"

/**
 * Defines whether the camera operates in Exterior Bird's-Eye Isometric mode
 * or Interior Over-the-Shoulder Third-Person mode.
 */
UENUM(BlueprintType)
enum class EEdrathCameraMode : uint8
{
    ExteriorBirdsEye       UMETA(DisplayName = "Exterior Bird's-Eye (Isometric 38 deg)"),
    InteriorThirdPerson    UMETA(DisplayName = "Interior Third-Person (Over-the-Shoulder)")
};

/**
 * Defines which timeline layer is currently active around Kael's Black Clock.
 */
UENUM(BlueprintType)
enum class ETimelineState : uint8
{
    PresentTimeline        UMETA(DisplayName = "Present Timeline (Default Reality)"),
    RejectedTimeline       UMETA(DisplayName = "Rejected Timeline (Possibility Shift)"),
    MemoryReconstruction   UMETA(DisplayName = "Memory Reconstruction Echo")
};

/**
 * Defines the Six Regions of Edrath.
 */
UENUM(BlueprintType)
enum class EEdrathRegion : uint8
{
    Veyl                   UMETA(DisplayName = "1. The Kingdom of Veyl"),
    Hollowgreen            UMETA(DisplayName = "2. The Hollowgreen"),
    AshenExpanse           UMETA(DisplayName = "3. The Ashen Expanse"),
    Nhal                   UMETA(DisplayName = "4. Nhal - The City Beneath Death"),
    CrownOfWinter          UMETA(DisplayName = "5. The Crown of Winter"),
    UnwrittenSea           UMETA(DisplayName = "6. The Unwritten Sea")
};

/**
 * Defines the Four Endings of Edrath.
 */
UENUM(BlueprintType)
enum class EEdrathEndingType : uint8
{
    None                   UMETA(DisplayName = "In Progress"),
    PreservedWorld         UMETA(DisplayName = "Ending A - The Preserved World (Preservation)"),
    UnwrittenWorld         UMETA(DisplayName = "Ending B - The Unwritten World (Liberation)"),
    MortalWorld            UMETA(DisplayName = "Ending C - The Mortal World (Acceptance)"),
    UncomfortableTruth     UMETA(DisplayName = "Ending D - The Uncomfortable Truth (Distributed Power)")
};

/**
 * Defines Kael's Life-Echo Wardrobe Archetypes across Acts I-VI.
 */
UENUM(BlueprintType)
enum class EKaelLifeEchoArchetype : uint8
{
    ForgottenSoldier_Act1  UMETA(DisplayName = "Forgotten Soldier (Act I Battlefield)"),
    RoyalAssassin_Act2     UMETA(DisplayName = "Royal Assassin (Act II Veyl)"),
    PlaguedHealer_Act3     UMETA(DisplayName = "Plagued Healer (Act III Hollowgreen Echo)"),
    TyrantConqueror_Act3   UMETA(DisplayName = "Tyrant Conqueror (Act III Hollowgreen Echo)"),
    SimpleFarmer_Act3      UMETA(DisplayName = "Simple Farmer (Act III Hollowgreen Echo)"),
    ShadowlessWraith_Act4  UMETA(DisplayName = "Shadowless Wraith (Act IV Nhal)"),
    TimeArchivist_Act5     UMETA(DisplayName = "Time Archivist (Act V Crown of Winter)")
};

/**
 * Stores a single recorded combat action frame for the Black Clock Echo Combat system.
 */
USTRUCT(BlueprintType)
struct FEchoActionFrame
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Edrath|EchoCombat")
    FVector WorldPosition = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Edrath|EchoCombat")
    FRotator WorldRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, Category = "Edrath|EchoCombat")
    FName ActionMontageSection = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category = "Edrath|EchoCombat")
    float BaseDamage = 25.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Edrath|EchoCombat")
    float TimestampSeconds = 0.0f;
};

/**
 * Stores a piece of forensic narrative evidence that can mutate when memories or timelines change.
 */
USTRUCT(BlueprintType)
struct FEdrathEvidenceRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Narrative")
    FGameplayTag EvidenceTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Narrative")
    FText CurrentDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Narrative")
    bool bIsForeshadowingTwistClue = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Narrative")
    int32 MutationVersion = 1;
};

/**
 * Stores autonomous state for each of the four major companions (Ilyra, Orren, Senn, Maevra).
 */
USTRUCT(BlueprintType)
struct FCompanionRelationshipState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Companions")
    FName CompanionID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Companions")
    float Loyalty = 50.0f; // Range: 0.0f (Hostile/Leaves Party) to 100.0f (Devoted)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Companions")
    float SuspicionOfKael = 10.0f; // Increases when Kael abuses the Black Clock

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Companions")
    bool bWithholdingCriticalTruth = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Companions")
    bool bHasTurnedHostileInNhal = false;
};
```

### 6.3 Dual-Perspective Camera Component (`Source/Edrath/Public/Camera/EdrathDualCameraComponent.h` & `.cpp`)

This component handles the seamless interpolation between the **Exterior Bird's-Eye Isometric** view and the **Interior Third-Person Over-the-Shoulder** view, including time dilation hooks for *The Crown of Winter*.

```cpp
// File: Source/Edrath/Public/Camera/EdrathDualCameraComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "EdrathDualCameraComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraPerspectiveChanged, EEdrathCameraMode, NewMode, AActor*, InteriorVolumeActor);

UCLASS(ClassGroup=(EdrathCamera), meta=(BlueprintSpawnableComponent))
class EDRATH_API UEdrathDualCameraComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEdrathDualCameraComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Initializes references to the owning character's SpringArm and Camera components. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|Camera")
    void InitializeCameraRig(USpringArmComponent* InSpringArm, UCameraComponent* InCamera, ACharacter* InOwnerCharacter);

    /** Transitions smoothly between Exterior Bird's-Eye and Interior Third-Person modes. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|Camera")
    void SetCameraPerspective(EEdrathCameraMode NewMode, AActor* TriggeringVolume = nullptr);

    /** Adjusts zoom distance when in Exterior Bird's-Eye mode. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|Camera")
    void AdjustExteriorZoom(float WheelAxisDelta);

    /** Rotates the Exterior Bird's-Eye camera yaw in 45-degree increments or smooth orbit. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|Camera")
    void RotateExteriorYaw(float YawDeltaDegrees);

    UFUNCTION(BlueprintPure, Category = "Edrath|Camera")
    EEdrathCameraMode GetCurrentCameraMode() const { return CurrentMode; }

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Camera")
    FOnCameraPerspectiveChanged OnCameraPerspectiveChanged;

protected:
    // --- Exterior Bird's-Eye Isometric Settings ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    float ExteriorTargetArmLength = 1150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    float ExteriorMinArmLength = 850.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    float ExteriorMaxArmLength = 1450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    float ExteriorPitchDegrees = -38.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    float ExteriorYawDegrees = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    float ExteriorFOV = 28.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Exterior")
    FVector ExteriorSocketOffset = FVector(0.0f, 0.0f, 0.0f);

    // --- Interior Third-Person Over-the-Shoulder Settings ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Interior")
    float InteriorTargetArmLength = 240.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Interior")
    float InteriorDefaultPitchDegrees = -12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Interior")
    float InteriorFOV = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Interior")
    FVector InteriorSocketOffset = FVector(0.0f, 65.0f, 45.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Camera|Transition")
    float BlendInterpSpeed = 4.5f;

private:
    UPROPERTY()
    TObjectPtr<USpringArmComponent> SpringArmComp;

    UPROPERTY()
    TObjectPtr<UCameraComponent> CameraComp;

    UPROPERTY()
    TObjectPtr<ACharacter> OwnerCharacter;

    EEdrathCameraMode CurrentMode = EEdrathCameraMode::ExteriorBirdsEye;
};
```

```cpp
// File: Source/Edrath/Private/Camera/EdrathDualCameraComponent.cpp
#include "Camera/EdrathDualCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

UEdrathDualCameraComponent::UEdrathDualCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UEdrathDualCameraComponent::InitializeCameraRig(USpringArmComponent* InSpringArm, UCameraComponent* InCamera, ACharacter* InOwnerCharacter)
{
    SpringArmComp = InSpringArm;
    CameraComp = InCamera;
    OwnerCharacter = InOwnerCharacter;

    if (SpringArmComp && CameraComp)
    {
        SpringArmComp->TargetArmLength = ExteriorTargetArmLength;
        SpringArmComp->SocketOffset = ExteriorSocketOffset;
        SpringArmComp->bUsePawnControlRotation = false;
        SpringArmComp->bDoCollisionTest = false; // Disabled in Bird's-Eye so roofs dither instead of snapping camera
        SpringArmComp->SetRelativeRotation(FRotator(ExteriorPitchDegrees, ExteriorYawDegrees, 0.0f));
        CameraComp->SetFieldOfView(ExteriorFOV);
    }
}

void UEdrathDualCameraComponent::SetCameraPerspective(EEdrathCameraMode NewMode, AActor* TriggeringVolume)
{
    if (CurrentMode == NewMode)
    {
        return;
    }

    CurrentMode = NewMode;

    if (OwnerCharacter && SpringArmComp)
    {
        UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
        if (CurrentMode == EEdrathCameraMode::InteriorThirdPerson)
        {
            // Enable close-up collision testing and controller-driven look inside buildings
            SpringArmComp->bUsePawnControlRotation = true;
            SpringArmComp->bDoCollisionTest = true;
            OwnerCharacter->bUseControllerRotationYaw = false;
            if (MoveComp)
            {
                MoveComp->bOrientRotationToMovement = true;
                MoveComp->MaxWalkSpeed = 420.0f; // Paced interior exploration speed
            }

            if (APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController()))
            {
                FRotator EntryRot = OwnerCharacter->GetActorRotation();
                EntryRot.Pitch = InteriorDefaultPitchDegrees;
                PC->SetControlRotation(EntryRot);
            }
        }
        else
        {
            // Restore Bird's-Eye Isometric tactical perspective outdoors
            SpringArmComp->bUsePawnControlRotation = false;
            SpringArmComp->bDoCollisionTest = false;
            OwnerCharacter->bUseControllerRotationYaw = false;
            if (MoveComp)
            {
                MoveComp->bOrientRotationToMovement = true;
                MoveComp->MaxWalkSpeed = 600.0f; // Open-world traversal speed
            }
        }
    }

    OnCameraPerspectiveChanged.Broadcast(CurrentMode, TriggeringVolume);
}

void UEdrathDualCameraComponent::AdjustExteriorZoom(float WheelAxisDelta)
{
    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        ExteriorTargetArmLength = FMath::Clamp(
            ExteriorTargetArmLength - (WheelAxisDelta * 75.0f),
            ExteriorMinArmLength,
            ExteriorMaxArmLength
        );
    }
}

void UEdrathDualCameraComponent::RotateExteriorYaw(float YawDeltaDegrees)
{
    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        ExteriorYawDegrees = FMath::Fmod(ExteriorYawDegrees + YawDeltaDegrees, 360.0f);
    }
}

void UEdrathDualCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!SpringArmComp || !CameraComp)
    {
        return;
    }

    const float DesiredArmLength = (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
        ? ExteriorTargetArmLength
        : InteriorTargetArmLength;

    const FVector DesiredSocketOffset = (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
        ? ExteriorSocketOffset
        : InteriorSocketOffset;

    const float DesiredFOV = (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
        ? ExteriorFOV
        : InteriorFOV;

    SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, DesiredArmLength, DeltaTime, BlendInterpSpeed);
    SpringArmComp->SocketOffset = FMath::VInterpTo(SpringArmComp->SocketOffset, DesiredSocketOffset, DeltaTime, BlendInterpSpeed);
    CameraComp->SetFieldOfView(FMath::FInterpTo(CameraComp->FieldOfView, DesiredFOV, DeltaTime, BlendInterpSpeed));

    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        const FRotator DesiredExteriorRot(ExteriorPitchDegrees, ExteriorYawDegrees, 0.0f);
        SpringArmComp->SetWorldRotation(FMath::RInterpTo(SpringArmComp->GetComponentRotation(), DesiredExteriorRot, DeltaTime, BlendInterpSpeed));
    }
}
```

### 6.4 Interior Building Transition Volume (`Source/Edrath/Public/Camera/EdrathInteriorTransitionVolume.h` & `.cpp`)

Place `AEdrathInteriorTransitionVolume` inside any building, house, palace, or crypt. When Kael steps inside:
1. It switches `UEdrathDualCameraComponent` from `ExteriorBirdsEye` to `InteriorThirdPerson`.
2. It hides exterior roof actors so lighting and ceilings behave cleanly indoors.
3. If `bIsCrownOfWinterTimeDilatedInterior` is true (Act V), it tracks interior seconds and fast-forwards exterior world time when Kael exits.

```cpp
// File: Source/Edrath/Public/Camera/EdrathInteriorTransitionVolume.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EdrathInteriorTransitionVolume.generated.h"

class UBoxComponent;

UCLASS()
class EDRATH_API AEdrathInteriorTransitionVolume : public AActor
{
    GENERATED_BODY()

public:
    AEdrathInteriorTransitionVolume();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Interior")
    TObjectPtr<UBoxComponent> InteriorBoundsBox;

    /** Roof or upper-floor exterior meshes that should hide when Kael is inside or near this building. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Interior")
    TArray<TObjectPtr<AActor>> RoofActorsToToggle;

    /** Enables Act V Crown of Winter rule: seconds inside equal years outside. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Interior")
    bool bIsCrownOfWinterTimeDilatedInterior = false;

    /** Multiplier of exterior simulated days elapsed per second spent inside this room. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Interior")
    float SimulatedDaysPerInteriorSecond = 180.0f;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnInteriorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnInteriorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    double InteriorEntryTimestampSeconds = 0.0;
};
```

```cpp
// File: Source/Edrath/Private/Camera/EdrathInteriorTransitionVolume.cpp
#include "Camera/EdrathInteriorTransitionVolume.h"
#include "Components/BoxComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AEdrathInteriorTransitionVolume::AEdrathInteriorTransitionVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    InteriorBoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBoundsBox"));
    RootComponent = InteriorBoundsBox;
    InteriorBoundsBox->SetBoxExtent(FVector(600.0f, 600.0f, 350.0f));
    InteriorBoundsBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AEdrathInteriorTransitionVolume::BeginPlay()
{
    Super::BeginPlay();
    InteriorBoundsBox->OnComponentBeginOverlap.AddDynamic(this, &AEdrathInteriorTransitionVolume::OnInteriorOverlapBegin);
    InteriorBoundsBox->OnComponentEndOverlap.AddDynamic(this, &AEdrathInteriorTransitionVolume::OnInteriorOverlapEnd);
}

void AEdrathInteriorTransitionVolume::OnInteriorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    if (UEdrathDualCameraComponent* DualCam = OtherActor->FindComponentByClass<UEdrathDualCameraComponent>())
    {
        DualCam->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson, this);
        InteriorEntryTimestampSeconds = GetWorld()->GetTimeSeconds();

        for (AActor* RoofActor : RoofActorsToToggle)
        {
            if (RoofActor)
            {
                RoofActor->SetActorHiddenInGame(false); // Inside 3D view uses interior ceiling visibility
            }
        }
    }
}

void AEdrathInteriorTransitionVolume::OnInteriorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    if (UEdrathDualCameraComponent* DualCam = OtherActor->FindComponentByClass<UEdrathDualCameraComponent>())
    {
        DualCam->SetCameraPerspective(EEdrathCameraMode::ExteriorBirdsEye, this);

        if (bIsCrownOfWinterTimeDilatedInterior && GetWorld() && GetWorld()->GetGameInstance())
        {
            const double SecondsSpentIndoors = FMath::Max(0.0, GetWorld()->GetTimeSeconds() - InteriorEntryTimestampSeconds);
            const int32 ElapsedExteriorDays = FMath::RoundToInt(SecondsSpentIndoors * SimulatedDaysPerInteriorSecond);
            if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                NarrativeSys->AdvanceExteriorTimeDays(ElapsedExteriorDays);
            }
        }
    }
}
```

### 6.5 Signature Mechanic: The Black Clock & Echo Combat (`Source/Edrath/Public/BlackClock/BlackClockComponent.h` & `.cpp`)

Implements the three core mechanics of the Black Clock:
1. **Possibility Shifting**: Reveals rejected futures (intact bridges, unlocked crypt doors, drained basins) while increasing regional instability.
2. **Memory Reconstruction**: Enters physical memory echoes to inspect witness contradictions (with Twist 1 tracking that recovered memories belong to others).
3. **Echo Combat**: Records a 120-frame circular buffer of Kael's attacks and spawns a timeline ghost executing a past strike (`1.2s` delay) at a clock stability cost.

```cpp
// File: Source/Edrath/Public/BlackClock/BlackClockComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "BlackClockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTimelineStateShifted, ETimelineState, NewState, FVector, Epicenter, float, WaveRadius);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEchoGhostSpawned, const FEchoActionFrame&, EchoFrame);

UCLASS(ClassGroup=(EdrathBlackClock), meta=(BlueprintSpawnableComponent))
class EDRATH_API UBlackClockComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBlackClockComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Toggles Possibility Shifting between PresentTimeline and RejectedTimeline. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|BlackClock")
    bool TogglePossibilityShift(EEdrathRegion CurrentRegion);

    /** Enters or exits Memory Reconstruction mode at a valid Memory Anchor. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|BlackClock")
    bool EnterMemoryReconstruction(FGameplayTag MemoryAnchorTag, FName OriginalMemoryOwnerNPC);

    /** Records an attack or movement frame into the circular buffer for Echo Combat. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|BlackClock|EchoCombat")
    void RecordCombatFrame(FName MontageSection, float BaseDamage);

    /** Manifests a ghost strike from SecondsAgo (default 1.2s) if clock stability permits. */
    UFUNCTION(BlueprintCallable, Category = "Edrath|BlackClock|EchoCombat")
    bool TriggerEchoStrike(float SecondsAgo, EEdrathRegion CurrentRegion, FEchoActionFrame& OutEchoFrame);

    UFUNCTION(BlueprintPure, Category = "Edrath|BlackClock")
    ETimelineState GetActiveTimelineState() const { return ActiveTimelineState; }

    UFUNCTION(BlueprintPure, Category = "Edrath|BlackClock")
    float GetClockStability() const { return CurrentClockStability; }

    UPROPERTY(BlueprintAssignable, Category = "Edrath|BlackClock")
    FOnTimelineStateShifted OnTimelineStateShifted;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|BlackClock")
    FOnEchoGhostSpawned OnEchoGhostSpawned;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|BlackClock")
    float MaxClockStability = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|BlackClock")
    float CurrentClockStability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|BlackClock")
    float StabilityRegenPerSecond = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|BlackClock")
    float PossibilityShiftStabilityCost = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|BlackClock")
    float EchoStrikeStabilityCost = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|BlackClock")
    float PossibilityWaveMaxRadius = 1800.0f;

private:
    ETimelineState ActiveTimelineState = ETimelineState::PresentTimeline;
    TArray<FEchoActionFrame> ActionHistoryBuffer;
    int32 MaxBufferFrames = 120;
};
```

```cpp
// File: Source/Edrath/Private/BlackClock/BlackClockComponent.cpp
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UBlackClockComponent::UBlackClockComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBlackClockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (ActiveTimelineState == ETimelineState::PresentTimeline)
    {
        CurrentClockStability = FMath::Min(MaxClockStability, CurrentClockStability + (StabilityRegenPerSecond * DeltaTime));
    }
    else if (ActiveTimelineState == ETimelineState::RejectedTimeline)
    {
        // Maintaining a Rejected Future slowly drains stability
        CurrentClockStability = FMath::Max(0.0f, CurrentClockStability - (2.5f * DeltaTime));
        if (CurrentClockStability <= 0.0f)
        {
            TogglePossibilityShift(EEdrathRegion::Veyl);
        }
    }
}

bool UBlackClockComponent::TogglePossibilityShift(EEdrathRegion CurrentRegion)
{
    AActor* Owner = GetOwner();
    if (!Owner || !GetWorld()) return false;

    if (ActiveTimelineState == ETimelineState::PresentTimeline)
    {
        if (CurrentClockStability < PossibilityShiftStabilityCost)
        {
            return false;
        }
        CurrentClockStability -= PossibilityShiftStabilityCost;
        ActiveTimelineState = ETimelineState::RejectedTimeline;

        // Every activation of the Black Clock prunes possible futures (Act IV Twist)
        if (UGameInstance* GI = GetWorld()->GetGameInstance())
        {
            if (UEdrathNarrativeSubsystem* NarrativeSys = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                NarrativeSys->RegisterBlackClockActivation(CurrentRegion, 5.0f);
            }
        }
    }
    else
    {
        ActiveTimelineState = ETimelineState::PresentTimeline;
    }

    // Update all PossibilityShiftActors within radius
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APossibilityShiftActor::StaticClass(), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        if (APossibilityShiftActor* ShiftActor = Cast<APossibilityShiftActor>(Actor))
        {
            const float Dist = FVector::Dist(Owner->GetActorLocation(), ShiftActor->GetActorLocation());
            if (Dist <= PossibilityWaveMaxRadius)
            {
                ShiftActor->ApplyTimelineState(ActiveTimelineState);
            }
        }
    }

    OnTimelineStateShifted.Broadcast(ActiveTimelineState, Owner->GetActorLocation(), PossibilityWaveMaxRadius);
    return true;
}

bool UBlackClockComponent::EnterMemoryReconstruction(FGameplayTag MemoryAnchorTag, FName OriginalMemoryOwnerNPC)
{
    if (!GetWorld() || !GetWorld()->GetGameInstance()) return false;

    ActiveTimelineState = ETimelineState::MemoryReconstruction;
    if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
    {
        // Stores Twist 1 metadata: memory appears from Kael's POV but belongs to OriginalMemoryOwnerNPC
        NarrativeSys->RecordRecoveredMemory(MemoryAnchorTag, OriginalMemoryOwnerNPC);
    }
    OnTimelineStateShifted.Broadcast(ActiveTimelineState, GetOwner()->GetActorLocation(), PossibilityWaveMaxRadius);
    return true;
}

void UBlackClockComponent::RecordCombatFrame(FName MontageSection, float BaseDamage)
{
    if (!GetOwner() || !GetWorld()) return;

    FEchoActionFrame Frame;
    Frame.WorldPosition = GetOwner()->GetActorLocation();
    Frame.WorldRotation = GetOwner()->GetActorRotation();
    Frame.ActionMontageSection = MontageSection;
    Frame.BaseDamage = BaseDamage;
    Frame.TimestampSeconds = GetWorld()->GetTimeSeconds();

    if (ActionHistoryBuffer.Num() >= MaxBufferFrames)
    {
        ActionHistoryBuffer.RemoveAt(0);
    }
    ActionHistoryBuffer.Add(Frame);
}

bool UBlackClockComponent::TriggerEchoStrike(float SecondsAgo, EEdrathRegion CurrentRegion, FEchoActionFrame& OutEchoFrame)
{
    if (CurrentClockStability < EchoStrikeStabilityCost || ActionHistoryBuffer.IsEmpty() || !GetWorld())
    {
        return false;
    }

    const float TargetTimestamp = GetWorld()->GetTimeSeconds() - SecondsAgo;
    int32 BestIndex = ActionHistoryBuffer.Num() - 1;
    float SmallestDiff = TNumericLimits<float>::Max();

    for (int32 i = 0; i < ActionHistoryBuffer.Num(); ++i)
    {
        const float Diff = FMath::Abs(ActionHistoryBuffer[i].TimestampSeconds - TargetTimestamp);
        if (Diff < SmallestDiff)
        {
            SmallestDiff = Diff;
            BestIndex = i;
        }
    }

    CurrentClockStability -= EchoStrikeStabilityCost;
    OutEchoFrame = ActionHistoryBuffer[BestIndex];

    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->RegisterBlackClockActivation(CurrentRegion, 2.0f);
        }
    }

    OnEchoGhostSpawned.Broadcast(OutEchoFrame);
    return true;
}
```

### 6.6 Dual-Timeline World Actor (`Source/Edrath/Public/BlackClock/PossibilityShiftActor.h` & `.cpp`)

Any bridge, crypt door, collapsed wall, or flooded basin that differs between the **Present Timeline** and a **Rejected Timeline** inherits from `APossibilityShiftActor`.

```cpp
// File: Source/Edrath/Public/BlackClock/PossibilityShiftActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/EdrathTypes.h"
#include "PossibilityShiftActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class EDRATH_API APossibilityShiftActor : public AActor
{
    GENERATED_BODY()

public:
    APossibilityShiftActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|PossibilityShift")
    TObjectPtr<USceneComponent> SceneRoot;

    /** Mesh active in the Present Timeline (e.g., collapsed bridge rubble or sealed stone crypt door). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|PossibilityShift")
    TObjectPtr<UStaticMeshComponent> PresentTimelineMesh;

    /** Mesh active in the Rejected Timeline (e.g., intact stone bridge or open crypt passage). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|PossibilityShift")
    TObjectPtr<UStaticMeshComponent> RejectedTimelineMesh;

    UFUNCTION(BlueprintCallable, Category = "Edrath|PossibilityShift")
    void ApplyTimelineState(ETimelineState NewState);

    UFUNCTION(BlueprintPure, Category = "Edrath|PossibilityShift")
    ETimelineState GetCurrentActorTimelineState() const { return CurrentState; }

protected:
    virtual void BeginPlay() override;

private:
    ETimelineState CurrentState = ETimelineState::PresentTimeline;
};
```

```cpp
// File: Source/Edrath/Private/BlackClock/PossibilityShiftActor.cpp
#include "BlackClock/PossibilityShiftActor.h"
#include "Components/StaticMeshComponent.h"

APossibilityShiftActor::APossibilityShiftActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    PresentTimelineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PresentTimelineMesh"));
    PresentTimelineMesh->SetupAttachment(SceneRoot);

    RejectedTimelineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RejectedTimelineMesh"));
    RejectedTimelineMesh->SetupAttachment(SceneRoot);
}

void APossibilityShiftActor::BeginPlay()
{
    Super::BeginPlay();
    ApplyTimelineState(ETimelineState::PresentTimeline);
}

void APossibilityShiftActor::ApplyTimelineState(ETimelineState NewState)
{
    CurrentState = NewState;
    const bool bShowRejected = (NewState == ETimelineState::RejectedTimeline);

    PresentTimelineMesh->SetVisibility(!bShowRejected, true);
    PresentTimelineMesh->SetCollisionEnabled(bShowRejected ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

    RejectedTimelineMesh->SetVisibility(bShowRejected, true);
    RejectedTimelineMesh->SetCollisionEnabled(bShowRejected ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}
```

### 6.7 Narrative, Twist, Belief & Ending Subsystem (`Source/Edrath/Public/Narrative/EdrathNarrativeSubsystem.h` & `.cpp`)

This `UGameInstanceSubsystem` is the single source of truth for the 7-day settlement loop (Act I), Veyl's belief-dependent buildings (Act II), Hollowgreen's past-life echoes (Act III), Nhal's pruned futures counter (Act IV), Crown of Winter's exterior time dilation (Act V), and the 4 Endings (Act VI).

```cpp
// File: Source/Edrath/Public/Narrative/EdrathNarrativeSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "EdrathNarrativeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettlementLoopReset, int32, TotalLoopResets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeliefWeightChanged, FGameplayTag, BuildingTag, float, NewBeliefWeight);

UCLASS()
class EDRATH_API UEdrathNarrativeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- Act I: 7-Day Settlement Loop & 114-Year-Dead Priest Twist ---
    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Act1")
    void AdvanceSettlementDay();

    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Act1")
    void BreakAct1SettlementLoop();

    UFUNCTION(BlueprintPure, Category = "Edrath|Narrative|Act1")
    int32 GetCurrentSettlementLoopDay() const { return CurrentSettlementDay; }

    UFUNCTION(BlueprintPure, Category = "Edrath|Narrative|Act1")
    bool IsSettlementLoopBroken() const { return bAct1LoopBroken; }

    // --- Act II: Veyl Citizen Belief Architecture ---
    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Act2")
    void SetCitizenBeliefWeight(FGameplayTag BuildingTag, float NewWeight);

    UFUNCTION(BlueprintPure, Category = "Edrath|Narrative|Act2")
    float GetCitizenBeliefWeight(FGameplayTag BuildingTag) const;

    // --- Twist 1 & Twist 5: Memory Ownership & Mutating Evidence ---
    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Evidence")
    void RecordRecoveredMemory(FGameplayTag MemoryTag, FName TrueOwnerNPC);

    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Evidence")
    void AddOrMutateEvidence(FGameplayTag EvidenceTag, FText NewDescription, bool bIsTwistClue);

    // --- Act IV & Twist 4: Pruned Futures & Regional Instability ---
    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Instability")
    void RegisterBlackClockActivation(EEdrathRegion Region, float InstabilityDelta);

    UFUNCTION(BlueprintPure, Category = "Edrath|Narrative|Instability")
    int32 GetPrunedFuturesCount() const { return ClockUsagePrunedFuturesCount; }

    // --- Act V: Crown of Winter Exterior Time Dilation ---
    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Act5")
    void AdvanceExteriorTimeDays(int32 SimulatedDays);

    UFUNCTION(BlueprintPure, Category = "Edrath|Narrative|Act5")
    int32 GetTotalSimulatedExteriorDays() const { return TotalExteriorDaysElapsed; }

    // --- Act VI: Four Endings Evaluation ---
    UFUNCTION(BlueprintPure, Category = "Edrath|Narrative|Endings")
    bool HasUnlockedFourthPath() const;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Narrative|Endings")
    EEdrathEndingType EvaluateFinalEnding(EEdrathEndingType PlayerSelectedPhilosophy) const;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Narrative")
    FOnSettlementLoopReset OnSettlementLoopReset;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Narrative")
    FOnBeliefWeightChanged OnBeliefWeightChanged;

private:
    int32 CurrentSettlementDay = 1; // Loops 1..7 until Priest Crypt twist is solved
    int32 SettlementLoopResetCount = 0;
    bool bAct1LoopBroken = false;

    int32 ClockUsagePrunedFuturesCount = 0;
    int32 TotalExteriorDaysElapsed = 0;

    TMap<FGameplayTag, float> BuildingBeliefWeights;
    TMap<FGameplayTag, FName> MemoryTrueOwnerRegistry;
    TMap<FGameplayTag, FEdrathEvidenceRecord> EvidenceRegistry;
    TMap<EEdrathRegion, float> RegionalInstabilityMap;
    TMap<FName, FCompanionRelationshipState> CompanionStates;
};
```

```cpp
// File: Source/Edrath/Private/Narrative/EdrathNarrativeSubsystem.cpp
#include "Narrative/EdrathNarrativeSubsystem.h"

void UEdrathNarrativeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Initialize the 4 core companions from the story bible
    const TArray<FName> CoreCompanions = { TEXT("Ilyra"), TEXT("Orren"), TEXT("Senn"), TEXT("Maevra") };
    for (const FName& Name : CoreCompanions)
    {
        FCompanionRelationshipState State;
        State.CompanionID = Name;
        State.Loyalty = (Name == TEXT("Maevra")) ? 25.0f : 55.0f;
        CompanionStates.Add(Name, State);
    }
}

void UEdrathNarrativeSubsystem::AdvanceSettlementDay()
{
    if (bAct1LoopBroken)
    {
        CurrentSettlementDay++;
        return;
    }

    CurrentSettlementDay++;
    if (CurrentSettlementDay > 7)
    {
        CurrentSettlementDay = 1;
        SettlementLoopResetCount++;
        OnSettlementLoopReset.Broadcast(SettlementLoopResetCount);
    }
}

void UEdrathNarrativeSubsystem::BreakAct1SettlementLoop()
{
    bAct1LoopBroken = true;
}

void UEdrathNarrativeSubsystem::SetCitizenBeliefWeight(FGameplayTag BuildingTag, float NewWeight)
{
    const float Clamped = FMath::Clamp(NewWeight, 0.0f, 1.0f);
    BuildingBeliefWeights.Add(BuildingTag, Clamped);
    OnBeliefWeightChanged.Broadcast(BuildingTag, Clamped);
}

float UEdrathNarrativeSubsystem::GetCitizenBeliefWeight(FGameplayTag BuildingTag) const
{
    if (const float* Found = BuildingBeliefWeights.Find(BuildingTag))
    {
        return *Found;
    }
    return 1.0f;
}

void UEdrathNarrativeSubsystem::RecordRecoveredMemory(FGameplayTag MemoryTag, FName TrueOwnerNPC)
{
    MemoryTrueOwnerRegistry.Add(MemoryTag, TrueOwnerNPC);
}

void UEdrathNarrativeSubsystem::AddOrMutateEvidence(FGameplayTag EvidenceTag, FText NewDescription, bool bIsTwistClue)
{
    if (FEdrathEvidenceRecord* Existing = EvidenceRegistry.Find(EvidenceTag))
    {
        Existing->CurrentDescription = NewDescription;
        Existing->MutationVersion++;
        Existing->bIsForeshadowingTwistClue = bIsTwistClue;
    }
    else
    {
        FEdrathEvidenceRecord NewRecord;
        NewRecord.EvidenceTag = EvidenceTag;
        NewRecord.CurrentDescription = NewDescription;
        NewRecord.bIsForeshadowingTwistClue = bIsTwistClue;
        NewRecord.MutationVersion = 1;
        EvidenceRegistry.Add(EvidenceTag, NewRecord);
    }
}

void UEdrathNarrativeSubsystem::RegisterBlackClockActivation(EEdrathRegion Region, float InstabilityDelta)
{
    ClockUsagePrunedFuturesCount++;
    const float Current = RegionalInstabilityMap.FindRef(Region);
    RegionalInstabilityMap.Add(Region, FMath::Clamp(Current + InstabilityDelta, 0.0f, 100.0f));
}

void UEdrathNarrativeSubsystem::AdvanceExteriorTimeDays(int32 SimulatedDays)
{
    TotalExteriorDaysElapsed += FMath::Max(0, SimulatedDays);
}

bool UEdrathNarrativeSubsystem::HasUnlockedFourthPath() const
{
    int32 TwistCluesFound = 0;
    for (const TPair<FGameplayTag, FEdrathEvidenceRecord>& Pair : EvidenceRegistry)
    {
        if (Pair.Value.bIsForeshadowingTwistClue)
        {
            TwistCluesFound++;
        }
    }
    // Requires all 6 major twist foreshadowing clues to unlock Ending D (The Uncomfortable Truth)
    return TwistCluesFound >= 6;
}

EEdrathEndingType UEdrathNarrativeSubsystem::EvaluateFinalEnding(EEdrathEndingType PlayerSelectedPhilosophy) const
{
    if (PlayerSelectedPhilosophy == EEdrathEndingType::UncomfortableTruth && !HasUnlockedFourthPath())
    {
        return EEdrathEndingType::MortalWorld;
    }
    return PlayerSelectedPhilosophy;
}
```

### 6.8 Protagonist Controller (`Source/Edrath/Public/Character/KaelCharacter.h` & `.cpp`)

Combines Kael's movement, the Sternum Black Clock socket (`bone_chest_clock`), the Dual-Perspective Camera component (`UEdrathDualCameraComponent`), the Black Clock component (`UBlackClockComponent`), and life-echo modular equipment slots.

```cpp
// File: Source/Edrath/Public/Character/KaelCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "KaelCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UEdrathDualCameraComponent;
class UBlackClockComponent;

UCLASS()
class EDRATH_API AKaelCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AKaelCharacter();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Kael")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Kael")
    TObjectPtr<UCameraComponent> FollowCamera;

    /** The ticking Black Clock embedded directly in Kael's sternum socket (bone_chest_clock). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Kael")
    TObjectPtr<UStaticMeshComponent> SternumBlackClockMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Kael")
    TObjectPtr<UEdrathDualCameraComponent> DualCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Kael")
    TObjectPtr<UBlackClockComponent> BlackClockComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Kael")
    EEdrathRegion CurrentRegion = EEdrathRegion::Veyl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Kael")
    EKaelLifeEchoArchetype EquippedLifeEchoArchetype = EKaelLifeEchoArchetype::ForgottenSoldier_Act1;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Kael")
    void PerformPrimaryAttack();

    UFUNCTION(BlueprintCallable, Category = "Edrath|Kael")
    bool ActivateBlackClockShift();
};
```

```cpp
// File: Source/Edrath/Private/Character/KaelCharacter.cpp
#include "Character/KaelCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "BlackClock/BlackClockComponent.h"

AKaelCharacter::AKaelCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    SternumBlackClockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SternumBlackClockMesh"));
    SternumBlackClockMesh->SetupAttachment(GetMesh(), TEXT("bone_chest_clock"));

    DualCameraComponent = CreateDefaultSubobject<UEdrathDualCameraComponent>(TEXT("DualCameraComponent"));
    BlackClockComponent = CreateDefaultSubobject<UBlackClockComponent>(TEXT("BlackClockComponent"));
}

void AKaelCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (DualCameraComponent)
    {
        DualCameraComponent->InitializeCameraRig(CameraBoom, FollowCamera, this);
    }
}

void AKaelCharacter::PerformPrimaryAttack()
{
    if (BlackClockComponent)
    {
        BlackClockComponent->RecordCombatFrame(TEXT("Slash_Combo_01"), 35.0f);
    }
}

bool AKaelCharacter::ActivateBlackClockShift()
{
    return BlackClockComponent ? BlackClockComponent->TogglePossibilityShift(CurrentRegion) : false;
}
```

### 6.9 Dynamic World Actors: Belief Architecture & The House With One More Room (`BeliefBuildingActor.h` & `HouseWithOneMoreRoomActor.h`)

```cpp
// File: Source/Edrath/Public/World/BeliefBuildingActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BeliefBuildingActor.generated.h"

class UStaticMeshComponent;

/**
 * Implements The Kingdom of Veyl's rule: buildings exist physically only when citizens believe in them.
 */
UCLASS()
class EDRATH_API ABeliefBuildingActor : public AActor
{
    GENERATED_BODY()

public:
    ABeliefBuildingActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Veyl")
    TObjectPtr<UStaticMeshComponent> BuildingMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Veyl")
    FGameplayTag BuildingBeliefTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Veyl")
    float ExistenceBeliefThreshold = 0.5f;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Veyl")
    void EvaluateBeliefExistence(FGameplayTag ChangedTag, float NewBeliefWeight);

protected:
    virtual void BeginPlay() override;
};
```

```cpp
// File: Source/Edrath/Private/World/BeliefBuildingActor.cpp
#include "World/BeliefBuildingActor.h"
#include "Components/StaticMeshComponent.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

ABeliefBuildingActor::ABeliefBuildingActor()
{
    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    RootComponent = BuildingMesh;
}

void ABeliefBuildingActor::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->OnBeliefWeightChanged.AddDynamic(this, &ABeliefBuildingActor::EvaluateBeliefExistence);
            EvaluateBeliefExistence(BuildingBeliefTag, NarrativeSys->GetCitizenBeliefWeight(BuildingBeliefTag));
        }
    }
}

void ABeliefBuildingActor::EvaluateBeliefExistence(FGameplayTag ChangedTag, float NewBeliefWeight)
{
    if (ChangedTag == BuildingBeliefTag)
    {
        const bool bExistsInReality = (NewBeliefWeight >= ExistenceBeliefThreshold);
        BuildingMesh->SetVisibility(bExistsInReality, true);
        BuildingMesh->SetCollisionEnabled(bExistsInReality ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    }
}
```

```cpp
// File: Source/Edrath/Public/World/HouseWithOneMoreRoomActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HouseWithOneMoreRoomActor.generated.h"

/**
 * Implements Side Quest 6 ("The House With One More Room"): spawns a new physical interior room
 * at the next modular doorway socket every time an NPC remembers the house's forgotten owner.
 */
UCLASS()
class EDRATH_API AHouseWithOneMoreRoomActor : public AActor
{
    GENERATED_BODY()

public:
    AHouseWithOneMoreRoomActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|SideQuests")
    TArray<TSubclassOf<AActor>> ProceduralRoomClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|SideQuests")
    FVector RoomExtensionOffset = FVector(800.0f, 0.0f, 0.0f);

    UFUNCTION(BlueprintCallable, Category = "Edrath|SideQuests")
    AActor* OnOwnerRememberedSpawnNextRoom();

    UFUNCTION(BlueprintPure, Category = "Edrath|SideQuests")
    int32 GetSpawnedRoomCount() const { return SpawnedRooms.Num(); }

private:
    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedRooms;
};
```

```cpp
// File: Source/Edrath/Private/World/HouseWithOneMoreRoomActor.cpp
#include "World/HouseWithOneMoreRoomActor.h"
#include "Engine/World.h"

AHouseWithOneMoreRoomActor::AHouseWithOneMoreRoomActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

AActor* AHouseWithOneMoreRoomActor::OnOwnerRememberedSpawnNextRoom()
{
    const int32 NextIndex = SpawnedRooms.Num();
    if (!GetWorld() || !ProceduralRoomClasses.IsValidIndex(NextIndex) || !ProceduralRoomClasses[NextIndex])
    {
        return nullptr;
    }

    const FVector SpawnLoc = GetActorLocation() + (RoomExtensionOffset * static_cast<float>(NextIndex + 1));
    AActor* NewRoom = GetWorld()->SpawnActor<AActor>(ProceduralRoomClasses[NextIndex], SpawnLoc, GetActorRotation());
    if (NewRoom)
    {
        SpawnedRooms.Add(NewRoom);
    }
    return NewRoom;
}
```

## 7. Stage 2 — Working Pre-Alpha Playable Vertical Slice Specification

Once Stage 1 compiles cleanly, GitHub Copilot must construct the **Pre-Alpha Playable Vertical Slice** (`L_PreAlpha_Act1_BattlefieldAndCrypt`) to verify every core system in a single playable 30–45 minute vertical slice of **Act I (*The Man Who Was Never Born*)**:

| Pre-Alpha Zone | Camera Mode | Gameplay & Story Sequence Tested | Underlying C++ Classes Verified |
| :--- | :--- | :--- | :--- |
| **Zone 1: The Ancient Battlefield** | `ExteriorBirdsEye` (`-38°` Pitch, `1150cm` Arm, `28°` FOV) | Kael awakens among centuries-old corpses with the ticking Black Clock in his sternum. Dying soldier dialogue plays (*"Do not let my daughter be born"*). Player tests Bird's-Eye locomotion, primary slash attacks, and **Echo Combat** (`TriggerEchoStrike(1.2f)`) against 3 battlefield wraiths. | `AKaelCharacter`, `UEdrathDualCameraComponent`, `UBlackClockComponent::RecordCombatFrame`, `UBlackClockComponent::TriggerEchoStrike`. |
| **Zone 2: TheCollapsed Gorge Bridge** | `ExteriorBirdsEye` | A shattered stone bridge blocks the road to the settlement. Activating **Possibility Shifting** (`TogglePossibilityShift`) swaps the bridge to its intact `RejectedTimeline` mesh and collision so Kael can cross. | `APossibilityShiftActor`, `UBlackClockComponent::TogglePossibilityShift`, `UEdrathNarrativeSubsystem::RegisterBlackClockActivation`. |
| **Zone 3: The 7-Day Looping Settlement (Exterior Streets)** | `ExteriorBirdsEye` | Villagers greet Kael as a long-time resident, though none can recall the past 7 days. Includes a test `ABeliefBuildingActor` shrine that dissolves if belief drops below `0.5f`. | `UEdrathNarrativeSubsystem::AdvanceSettlementDay`, `ABeliefBuildingActor`. |
| **Zone 4: Kael's Settlement House (Interior Inspection)** | **Smooth Transition → `InteriorThirdPerson`** (`-12°` Pitch, `240cm` Arm, `75°` FOV) | Crossing the front threshold triggers `AEdrathInteriorTransitionVolume`. The roof hides and the camera glides over Kael's right shoulder. Player inspects Kael's handwriting on the desk and the portrait of Kael beside the woman with the scratched-out face. | `AEdrathInteriorTransitionVolume::OnInteriorOverlapBegin`, `UEdrathDualCameraComponent::SetCameraPerspective`, `UEdrathNarrativeSubsystem::AddOrMutateEvidence`. |
| **Zone 5: The Local Crypt & The 114-Year-Dead Priest** | `InteriorThirdPerson` | Guided by the priest's voice, Kael enters the crypt interior volume, uses **Possibility Shifting** to open a rusted portcullis, and discovers the priest's skeletal corpse (dead for 114 years). Activating **Memory Reconstruction** on the corpse reveals the priest was speaking from a memory echo, breaking the 7-day loop (`BreakAct1SettlementLoop()`). | `UBlackClockComponent::EnterMemoryReconstruction`, `UEdrathNarrativeSubsystem::BreakAct1SettlementLoop`. |

### Automated Pre-Alpha Verification Test (`Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp`)

GitHub Copilot must generate and run this Unreal Automation Test to verify that the Base Architecture and Pre-Alpha Vertical Slice logic pass deterministically:

```cpp
// File: Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Character/KaelCharacter.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathPreAlphaVerticalSliceTest,
    "Edrath.PreAlpha.CoreSystemsAndDualCameraVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FEdrathPreAlphaVerticalSliceTest::RunTest(const FString& Parameters)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    UGameInstance* GI = NewObject<UGameInstance>(TestWorld);
    TestWorld->SetGameInstance(GI);
    GI->Init();

    // 1. Spawn Kael & Verify Default Bird's-Eye Camera Mode
    AKaelCharacter* Kael = TestWorld->SpawnActor<AKaelCharacter>();
    TestNotNull(TEXT("KaelCharacter must spawn cleanly"), Kael);
    TestEqual(
        TEXT("Default Camera Mode must be ExteriorBirdsEye"),
        Kael->DualCameraComponent->GetCurrentCameraMode(),
        EEdrathCameraMode::ExteriorBirdsEye
    );

    // 2. Verify Transition to Interior Third-Person Perspective Inside Buildings
    Kael->DualCameraComponent->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson, nullptr);
    TestEqual(
        TEXT("Interior Transition must switch Camera Mode to InteriorThirdPerson"),
        Kael->DualCameraComponent->GetCurrentCameraMode(),
        EEdrathCameraMode::InteriorThirdPerson
    );

    // 3. Verify Possibility Shifting Toggles Bridge Collision & Increments Pruned Futures
    APossibilityShiftActor* Bridge = TestWorld->SpawnActor<APossibilityShiftActor>();
    Bridge->SetActorLocation(Kael->GetActorLocation() + FVector(200.0f, 0.0f, 0.0f));
    const bool bShifted = Kael->ActivateBlackClockShift();
    TestTrue(TEXT("Black Clock Possibility Shift must succeed"), bShifted);
    TestEqual(
        TEXT("Bridge must shift to RejectedTimeline state"),
        Bridge->GetCurrentActorTimelineState(),
        ETimelineState::RejectedTimeline
    );

    // 4. Verify Echo Combat Buffer Records and Manifests Past Action Frame
    Kael->PerformPrimaryAttack();
    FEchoActionFrame EchoFrame;
    const bool bEchoSpawned = Kael->BlackClockComponent->TriggerEchoStrike(1.2f, EEdrathRegion::Veyl, EchoFrame);
    TestTrue(TEXT("Echo Combat strike must manifest from recorded action buffer"), bEchoSpawned);

    // 5. Verify Act I 7-Day Settlement Loop Reset & Crypt Twist Loop Break
    UEdrathNarrativeSubsystem* NarrativeSys = GI->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("NarrativeSubsystem must be initialized"), NarrativeSys);
    for (int32 i = 0; i < 7; ++i)
    {
        NarrativeSys->AdvanceSettlementDay();
    }
    TestEqual(TEXT("Settlement must reset to Day 1 after Day 7 while loop is active"), NarrativeSys->GetCurrentSettlementLoopDay(), 1);
    NarrativeSys->BreakAct1SettlementLoop();
    TestTrue(TEXT("Crypt Priest Twist must break the Act 1 settlement loop"), NarrativeSys->IsSettlementLoopBroken());

    TestWorld->DestroyWorld(false);
    return true;
}
```

## 8. Step-by-Step Copilot Execution Prompts (Copy-Paste Sequence)

To ensure GitHub Copilot builds the base architecture first and then constructs the working Pre-Alpha build without skipping files or getting confused, run these exact prompts in **JetBrains Rider Copilot Agent Mode** in strict numerical order:

1. **Prompt 1 (Repository & Build Scaffolding)**:
   > `@workspace Read README.md Sections 2, 5, 6.1, and 6.2. Generate .gitattributes, .github/copilot-instructions.md, Source/Edrath/Edrath.Build.cs, and Source/Edrath/Public/Core/EdrathTypes.h exactly as specified. Verify there are no missing includes.`
2. **Prompt 2 (Dual-Perspective Camera & Building Volume Base)**:
   > `@workspace Read README.md Sections 4, 6.3, and 6.4. Generate EdrathDualCameraComponent (.h/.cpp) and EdrathInteriorTransitionVolume (.h/.cpp) so exterior exploration uses Bird's-Eye Isometric (-38 deg pitch, 1150cm arm, 28 FOV) and entering any building smoothly transitions to Over-the-Shoulder Third-Person (-12 deg pitch, 240cm arm, 75 FOV).`
3. **Prompt 3 (The Black Clock, Echo Combat, & Possibility Shifting Base)**:
   > `@workspace Read README.md Sections 6.5 and 6.6. Generate BlackClockComponent (.h/.cpp) and PossibilityShiftActor (.h/.cpp) implementing Possibility Shifting, Memory Reconstruction ownership tracking, and the 120-frame Echo Combat circular buffer.`
4. **Prompt 4 (Narrative Subsystem, Protagonist Rig, & Dynamic World Actors)**:
   > `@workspace Read README.md Sections 3, 6.7, 6.8, and 6.9. Generate EdrathNarrativeSubsystem (.h/.cpp), KaelCharacter (.h/.cpp), BeliefBuildingActor (.h/.cpp), and HouseWithOneMoreRoomActor (.h/.cpp). Compile the Edrath C++ module and resolve any compiler warnings.`
5. **Prompt 5 (Pre-Alpha Playable Vertical Slice & Automated Verification)**:
   > `@workspace Read README.md Section 7. Generate Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp, compile, and run the Edrath.PreAlpha.CoreSystemsAndDualCameraVerification automation test, then scaffold the Pre-Alpha Act I level blueprint and actors for Zones 1 through 5.`
