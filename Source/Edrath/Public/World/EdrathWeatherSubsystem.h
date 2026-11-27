#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "EdrathWeatherSubsystem.generated.h"

/**
 * EEdrathWeatherType
 * Act-specific chronal weather patterns and atmospheric anomalies across Edrath.
 */
UENUM(BlueprintType)
enum class EEdrathWeatherType : uint8
{
    ClearChronalSky         UMETA(DisplayName = "Clear Chronal Sky"),
    AshenChronalStorm       UMETA(DisplayName = "Ashen Chronal Storm (Act 1)"),
    VeylPossibilityFog      UMETA(DisplayName = "Veyl Possibility Fog (Act 2)"),
    HollowgreenSporeRain    UMETA(DisplayName = "Hollowgreen Spore Rain (Act 3)"),
    NhalGeothermalSteam     UMETA(DisplayName = "Nhal Geothermal Steam (Act 4)"),
    CrownBlizzard           UMETA(DisplayName = "Crown of Winter Blizzard (Act 5)"),
    UnwrittenChronalSurge   UMETA(DisplayName = "Unwritten Sea Chronal Surge (Act 6)")
};

/**
 * FEdrathWeatherHazardState
 * Real-time gameplay and rendering hazard metrics imposed by the active weather condition.
 */
USTRUCT(BlueprintType)
struct FEdrathWeatherHazardState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    EEdrathWeatherType ActiveWeather = EEdrathWeatherType::ClearChronalSky;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float VisibilityDistanceUnits = 12000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float ChronalInstabilityRatePerSec = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float MovementSpeedMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float FrostGlazeIntensity = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float SporeToxicityRate = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float WindIntensity = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Weather")
    float ParticleSpawnRateScale = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeatherTransitioned, EEdrathWeatherType, OldWeather, EEdrathWeatherType, NewWeather, const FEdrathWeatherHazardState&, NewHazardState);

/**
 * UEdrathWeatherSubsystem
 * GameInstance subsystem orchestrating regional atmospheric hazards,
 * Niagara particle density scaling under thermal throttling,
 * and environmental audio ambiance coupling.
 */
UCLASS()
class EDRATH_API UEdrathWeatherSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Transitions to a specific weather condition, calculating regional hazards and particle budgets. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Weather")
    void SetActiveWeather(EEdrathWeatherType NewWeather);

    /** Transitions to the canonical atmospheric condition for a given region. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Weather")
    void SetCanonicalRegionWeather(EEdrathRegion Region);

    /** Returns current active weather enum. */
    UFUNCTION(BlueprintPure, Category="Edrath|Weather")
    EEdrathWeatherType GetActiveWeatherType() const { return CurrentHazardState.ActiveWeather; }

    /** Returns full real-time weather hazard state struct. */
    UFUNCTION(BlueprintPure, Category="Edrath|Weather")
    FEdrathWeatherHazardState GetActiveHazardState() const { return CurrentHazardState; }

    /** Computes thermal-throttled particle counts to maintain stable laptop/mobile frame pacing. */
    UFUNCTION(BlueprintPure, Category="Edrath|Weather")
    int32 CalculateThermalScaledParticleCount(int32 BaseParticleCount) const;

    /** Adapts particle spawn scaling when hardware thermal status shifts. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Weather")
    void ApplyThermalStatusToWeather(EEdrathThermalStatus ThermalStatus);

    UPROPERTY(BlueprintAssignable, Category="Edrath|Weather")
    FOnWeatherTransitioned OnWeatherTransitioned;

private:
    UFUNCTION()
    void HandleThermalMitigationApplied(EEdrathThermalStatus NewThermalStatus, float MaxTargetFPS);

    void RecalculateHazardMetrics();

    UPROPERTY()
    FEdrathWeatherHazardState CurrentHazardState;

    UPROPERTY()
    EEdrathThermalStatus CurrentThermalStatus = EEdrathThermalStatus::Nominal;
};
