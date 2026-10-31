#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "EdrathHardwareOptimizationSubsystem.generated.h"

UCLASS()
class EDRATH_API UEdrathHardwareOptimizationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|Performance")
    void ApplyCameraModeOptimization(EEdrathCameraMode CameraMode);

    UFUNCTION(BlueprintCallable, Category="Edrath|Performance")
    void OptimizeTimelineMemoryFootprint(ETimelineState ActiveState);

    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    EEdrathCameraMode GetOptimizedCameraMode() const { return OptimizedCameraMode; }

    UFUNCTION(BlueprintPure, Category="Edrath|Performance")
    ETimelineState GetOptimizedTimelineState() const { return OptimizedTimelineState; }

private:
    void ApplyBaseHardwareConfiguration();
    void SetConsoleVariable(const TCHAR* VariableName, int32 Value) const;
    void SetConsoleVariable(const TCHAR* VariableName, float Value) const;

    EEdrathCameraMode OptimizedCameraMode = EEdrathCameraMode::ExteriorBirdsEye;
    ETimelineState OptimizedTimelineState = ETimelineState::PresentTimeline;
};
