#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "World/EdrathWeatherSubsystem.h"
#include "BlackClock/EdrathCombatFeedbackSubsystem.h"
#include "EdrathVisualFXSubsystem.generated.h"

class UMaterialParameterCollection;

/**
 * FEdrathShaderEnvelope
 * Unified runtime material/post-process parameter targets synchronized with Shaders/*.usf
 * (PossibilityDissolve.usf, MemoryEchoGhost.usf, PackedRGBACustomizer.usf).
 */
USTRUCT(BlueprintType)
struct FEdrathShaderEnvelope
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|VisualFX")
    float TimelineDissolveAlpha = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|VisualFX")
    float ChromaticGlitchIntensity = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|VisualFX")
    float FrostGlazeMask = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|VisualFX")
    float SporeHazeMask = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|VisualFX")
    float ChronalRippleIntensity = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|VisualFX")
    float ClockInstabilityDesaturation = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualFXEnvelopeUpdated, const FEdrathShaderEnvelope&, CurrentEnvelope);

/**
 * UEdrathVisualFXSubsystem
 * GameInstance subsystem driving smooth HLSL/MPC parameter envelopes from
 * timeline shifts, regional weather hazards, combat hit-stop ripples, and forensic breakthroughs.
 */
UCLASS()
class EDRATH_API UEdrathVisualFXSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Assigns the project MaterialParameterCollection driven by this subsystem during play. */
    UFUNCTION(BlueprintCallable, Category="Edrath|VisualFX")
    void SetRealityParameterCollection(UMaterialParameterCollection* InCollection) { RealityMPC = InCollection; }

    /** Updates dissolve and chromatic aberration targets when Black Clock timeline state shifts. */
    UFUNCTION(BlueprintCallable, Category="Edrath|VisualFX")
    void ApplyTimelineStateVFX(ETimelineState NewTimelineState);

    /** Sets desaturation proportional to Black Clock stability exhaustion (0.0 stable -> 1.0 collapsed). */
    UFUNCTION(BlueprintCallable, Category="Edrath|VisualFX")
    void UpdateClockInstabilityDesaturation(float StabilityRatio);

    /** Triggers a decaying chromatic aberration spike upon Forensic Contradiction breakthroughs. */
    UFUNCTION(BlueprintCallable, Category="Edrath|VisualFX")
    void TriggerContradictionBreakthroughGlitch(float GlitchIntensity = 0.85f);

    /** Smoothly steps current shader envelope toward target values and decays transient pulses. */
    UFUNCTION(BlueprintCallable, Category="Edrath|VisualFX")
    void StepVisualFXBlend(float DeltaSeconds, float InterpSpeed = 8.0f);

    UFUNCTION(BlueprintPure, Category="Edrath|VisualFX")
    FEdrathShaderEnvelope GetCurrentEnvelope() const { return CurrentEnvelope; }

    UFUNCTION(BlueprintPure, Category="Edrath|VisualFX")
    FEdrathShaderEnvelope GetTargetEnvelope() const { return TargetEnvelope; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|VisualFX")
    FOnVisualFXEnvelopeUpdated OnVisualFXEnvelopeUpdated;

private:
    UFUNCTION()
    void HandleWeatherTransitioned(EEdrathWeatherType OldWeather, EEdrathWeatherType NewWeather, const FEdrathWeatherHazardState& HazardState);

    UFUNCTION()
    void HandleCombatHitStopTriggered(EEdrathImpactSeverity Severity, const FEdrathHitStopProfile& Profile);

    void PushEnvelopeToMPC();

    UPROPERTY()
    TObjectPtr<UMaterialParameterCollection> RealityMPC;

    UPROPERTY()
    FEdrathShaderEnvelope CurrentEnvelope;

    UPROPERTY()
    FEdrathShaderEnvelope TargetEnvelope;
};
