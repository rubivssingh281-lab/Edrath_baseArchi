#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "EdrathScalabilitySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScalabilityPresetChanged, EEdrathQualityPreset, NewPreset);

UCLASS()
class EDRATH_API UEdrathScalabilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

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

    UFUNCTION(BlueprintCallable, Category="Edrath|Scalability")
    void AutoDetectPlatformAndApplyDefaults();

    UPROPERTY(BlueprintAssignable, Category="Edrath|Scalability")
    FOnScalabilityPresetChanged OnScalabilityPresetChanged;

private:
    UPROPERTY()
    EEdrathQualityPreset CurrentQualityPreset = EEdrathQualityPreset::Epic_PC;

    UPROPERTY()
    int32 CurrentTargetFrameRate = 60;

    UPROPERTY()
    bool bDynamicResolutionEnabled = false;

    UPROPERTY()
    bool bLumenHardwareRayTracing = true;

    void ApplyInternalQualityGroupSettings(int32 QualityLevel);
};
