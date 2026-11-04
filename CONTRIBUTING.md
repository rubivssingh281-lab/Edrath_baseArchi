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
