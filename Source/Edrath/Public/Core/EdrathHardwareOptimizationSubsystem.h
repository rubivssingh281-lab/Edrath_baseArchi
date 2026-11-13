#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "EdrathHardwareOptimizationSubsystem.generated.h"

/**
 * Detected hardware profile used for VRAM-aware budget allocation.
 */
USTRUCT(BlueprintType)
struct FEdrathHardwareProfile
{
    GENERATED_BODY()

    /** Dedicated GPU VRAM in MB (e.g. 8192 for RTX 5070 8GB). */
    UPROPERTY(BlueprintReadOnly, Category="Edrath|Hardware")
    int32 DedicatedVRAMMB = 0;

    /** Total system RAM in MB. */
    UPROPERTY(BlueprintReadOnly, Category="Edrath|Hardware")
    int32 SystemRAMMB = 0;

    /** Number of logical CPU cores (including hyperthreads). */
    UPROPERTY(BlueprintReadOnly, Category="Edrath|Hardware")
    int32 LogicalCores = 0;

    /** GPU adapter description string from the RHI. */
    UPROPERTY(BlueprintReadOnly, Category="Edrath|Hardware")
    FString GPUAdapterName;

    /** Whether the GPU hardware supports ray tracing. */
    UPROPERTY(BlueprintReadOnly, Category="Edrath|Hardware")
    bool bSupportsHardwareRayTracing = false;
};

/**
 * VRAM budget breakdown across rendering subsystems.
 * Ensures total allocation stays within the GPU's VRAM ceiling.
 */
USTRUCT(BlueprintType)
struct FEdrathVRAMBudgetAllocation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 TextureStreamingMB = 2560;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 NaniteStreamingMB = 1024;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 VSMEstimateMB = 768;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 LumenEstimateMB = 1200;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 FramebufferMB = 500;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 ShaderCacheMB = 500;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|VRAM")
    int32 DriverOverheadMB = 256;

    /** Sum of all subsystem allocations. Must stay under VRAM ceiling. */
    int32 GetTotalAllocatedMB() const
    {
        return TextureStreamingMB + NaniteStreamingMB + VSMEstimateMB
             + LumenEstimateMB + FramebufferMB + ShaderCacheMB + DriverOverheadMB;
    }
};

/**
 * Hardware-aware optimization subsystem with VRAM budget management.
 *
 * Detects GPU, CPU, and RAM at startup, then allocates a per-subsystem VRAM budget
 * that keeps total usage under the GPU's ceiling. Provides context-aware runtime
 * adjustments for camera mode transitions and timeline shifts (Possibility Shifting).
 *
 * For the RTX 5070 8GB development target:
 *   VRAM Ceiling = 8192 - 512 (driver) = 7680 MB usable
 *   Budget: Textures 2560 + Nanite 1024 + VSM 768 + Lumen 1200 + FB 500 + Shaders 500 + Driver 256 = 6808 MB
 *   Headroom: ~872 MB for spikes, editor overhead, and OS compositor
 */
UCLASS()
class EDRATH_API UEdrathHardwareOptimizationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** Apply camera-mode-specific rendering optimizations within the VRAM budget. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Performance")
    void ApplyCameraModeOptimization(EEdrathCameraMode CameraMode);

    /** Optimize memory footprint for the active timeline state (Possibility Shifting). */
    UFUNCTION(BlueprintCallable, Category="Edrath|Performance")
    void OptimizeTimelineMemoryFootprint(ETimelineState ActiveState);

    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    EEdrathCameraMode GetOptimizedCameraMode() const { return OptimizedCameraMode; }

    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    ETimelineState GetOptimizedTimelineState() const { return OptimizedTimelineState; }

    /** Get the detected hardware profile. */
    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    const FEdrathHardwareProfile& GetHardwareProfile() const { return HardwareProfile; }

    /** Get the current VRAM budget allocation breakdown. */
    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    const FEdrathVRAMBudgetAllocation& GetVRAMBudget() const { return VRAMBudget; }

    /** Get the maximum usable VRAM after reserving safety headroom (MB). */
    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    int32 GetVRAMCeilingMB() const { return VRAMCeilingMB; }

    /** Checks current VRAM consumption against the 90% safety threshold and dynamically throttles if exceeded. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Performance|VRAM")
    bool CheckAndEnforceVRAMSentinel(float SimulatedUsedVRAMMB = -1.0f);

    /** Current dynamic VRAM sentinel throttling level: 0 = Normal, 1 = Caution, 2 = Emergency Throttling */
    UFUNCTION(BlueprintPure, Category="Edrath|Performance|VRAM")
    int32 GetActiveVRAMThrottleLevel() const { return ActiveVRAMThrottleLevel; }

    /** Gets the 90% critical threshold in MB */
    UFUNCTION(BlueprintPure, Category="Edrath|Performance|VRAM")
    float GetVRAMSafetyThresholdMB() const { return static_cast<float>(VRAMCeilingMB) * 0.90f; }

private:
    /** Detect GPU, CPU, and RAM via platform APIs and RHI. */
    void DetectHardware();

    /** Calculate the VRAM ceiling (total - driver reservation). */
    void CalculateVRAMCeiling();

    /** Allocate VRAM budgets across rendering subsystems based on the ceiling. */
    void ApplyBaseHardwareConfiguration();

    /** Push the calculated VRAM budget to engine console variables. */
    void ApplyVRAMBudget();

    void SetConsoleVariable(const TCHAR* VariableName, int32 Value) const;
    void SetConsoleVariable(const TCHAR* VariableName, float Value) const;

    FEdrathHardwareProfile HardwareProfile;
    FEdrathVRAMBudgetAllocation VRAMBudget;

    /** Maximum usable VRAM in MB (detected - 512MB driver reservation). */
    int32 VRAMCeilingMB = 7680;

    EEdrathCameraMode OptimizedCameraMode = EEdrathCameraMode::ExteriorBirdsEye;
    ETimelineState OptimizedTimelineState = ETimelineState::PresentTimeline;
    int32 ActiveVRAMThrottleLevel = 0;
};
