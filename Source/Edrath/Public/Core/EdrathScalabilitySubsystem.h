#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "EdrathScalabilitySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScalabilityPresetChanged, EEdrathQualityPreset, NewPreset);

/**
 * Runtime scalability subsystem with hardware-aware auto-detection.
 *
 * Detects GPU VRAM and system RAM at startup to select the optimal quality preset.
 * The Development_Laptop preset is tailored for 6-12GB VRAM GPUs: it uses software
 * Lumen at maximum probe density, freeing ~2GB VRAM for maximum Nanite, VSM, and
 * texture quality — pushing the hardware to its visual limit within the VRAM envelope.
 */
UCLASS()
class EDRATH_API UEdrathScalabilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** Apply a specific quality preset, adjusting all scalability groups and rendering modes. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void ApplyQualityPreset(EEdrathQualityPreset Preset);

    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    EEdrathQualityPreset GetCurrentQualityPreset() const { return CurrentQualityPreset; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void SetTargetFrameRate(int32 TargetFPS);

    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    int32 GetTargetFrameRate() const { return CurrentTargetFrameRate; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void SetResolutionScale(float NewScale);

    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    float GetResolutionScale() const;

    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void SetDynamicResolutionEnabled(bool bEnable);

    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    bool IsDynamicResolutionEnabled() const { return bDynamicResolutionEnabled; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void SetLumenHardwareRayTracingEnabled(bool bEnable);

    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    bool IsLumenHardwareRayTracingEnabled() const { return bLumenHardwareRayTracing; }

    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    bool IsRunningOnMobile() const;

    /** Detect hardware capabilities and apply the best matching quality preset. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void AutoDetectPlatformAndApplyDefaults();

    /** Get detected dedicated GPU VRAM in megabytes. Returns 0 if detection failed. */
    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    int32 GetDetectedVRAMMB() const { return DetectedVRAMMB; }

    /** Get detected total system RAM in megabytes. */
    UFUNCTION(BlueprintPure, Category="Edrath|Scalability")
    int32 GetDetectedSystemRAMMB() const { return DetectedSystemRAMMB; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Scalability")
    FOnScalabilityPresetChanged OnScalabilityPresetChanged;

private:
    UPROPERTY()
    EEdrathQualityPreset CurrentQualityPreset = EEdrathQualityPreset::Development_Laptop;

    UPROPERTY()
    int32 CurrentTargetFrameRate = 60;

    UPROPERTY()
    bool bDynamicResolutionEnabled = true;

    UPROPERTY()
    bool bLumenHardwareRayTracing = false;

    UPROPERTY()
    int32 DetectedVRAMMB = 0;

    UPROPERTY()
    int32 DetectedSystemRAMMB = 0;

    /** Probe the RHI for GPU VRAM and system memory. */
    void DetectHardwareCapabilities();

    /** Apply UE5 scalability group settings for a given quality level (0-3). */
    void ApplyInternalQualityGroupSettings(int32 QualityLevel);

    /** Apply Development_Laptop-specific rendering overrides — max Lumen + TSR quality. */
    void ApplyLaptopDevelopmentOverrides();
};
