#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "Engine/TimerHandle.h"
#include "EdrathCombatFeedbackSubsystem.generated.h"

/**
 * EEdrathImpactSeverity
 * Classifies combat collision weight for hit-stop freeze frames, camera shake, and audio cues.
 */
UENUM(BlueprintType)
enum class EEdrathImpactSeverity : uint8
{
    LightSlash      UMETA(DisplayName = "Light Slash"),
    HeavyStrike     UMETA(DisplayName = "Heavy Strike"),
    EchoResonance   UMETA(DisplayName = "Echo Timeline Resonance"),
    PerfectParry    UMETA(DisplayName = "Perfect Chronal Parry"),
    PoiseShatter    UMETA(DisplayName = "Poise Shatter / Guard Break")
};

/**
 * FEdrathHitStopProfile
 * Deterministic time-dilation and feedback envelope for a single combat impact.
 */
USTRUCT(BlueprintType)
struct FEdrathHitStopProfile
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    float DurationSeconds = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    float TimeDilationScale = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    float CameraShakeIntensity = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    FName ImpactAudioTag = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    bool bTriggersChronalRipple = false;
};

/**
 * FEdrathPoiseEvaluationResult
 * Outcome of applying poise damage, accounting for Echo resonance and parry counters.
 */
USTRUCT(BlueprintType)
struct FEdrathPoiseEvaluationResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    float RemainingPoise = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    bool bPoiseBroken = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    float StaggerDurationSeconds = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    EEdrathImpactSeverity ResultingSeverity = EEdrathImpactSeverity::LightSlash;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|CombatFeedback")
    FEdrathHitStopProfile HitStopProfile;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitStopTriggered, EEdrathImpactSeverity, Severity, const FEdrathHitStopProfile&, Profile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoiseShattered, float, StaggerDurationSeconds);

/**
 * UEdrathCombatFeedbackSubsystem
 * GameInstance subsystem governing combat hit-stop freeze frames, poise shatter evaluation,
 * perfect parry chronal windows, and thermal-aware feedback damping.
 */
UCLASS()
class EDRATH_API UEdrathCombatFeedbackSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Triggers a damped hit-stop profile, respecting a minimum refractory cooldown against multi-hit stutter. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CombatFeedback")
    FEdrathHitStopProfile TriggerHitStop(EEdrathImpactSeverity Severity, float CurrentWorldTimeSeconds);

    /** Evaluates poise damage with Echo Strike (1.35x) and Parry Counter (2.0x) multipliers. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CombatFeedback")
    FEdrathPoiseEvaluationResult EvaluatePoiseBreak(
        float CurrentPoise,
        float IncomingPoiseDamage,
        bool bIsParryCounter,
        bool bIsEchoStrike,
        float CurrentWorldTimeSeconds);

    /** Returns true if an incoming strike landed inside the 140ms Perfect Chronal Parry window. */
    UFUNCTION(BlueprintPure, Category="Edrath|CombatFeedback")
    bool EvaluatePerfectParryWindow(float ParryStartTimestamp, float HitImpactTimestamp) const;

    /** Returns Black Clock stability reward for standard vs perfect parries. */
    UFUNCTION(BlueprintPure, Category="Edrath|CombatFeedback")
    float GetParryClockStabilityReward(bool bIsPerfectParry) const;

    /** Returns true if a hit-stop freeze frame is currently active at the given timestamp. */
    UFUNCTION(BlueprintPure, Category="Edrath|CombatFeedback")
    bool IsHitStopActive(float CurrentWorldTimeSeconds) const;

    /** Returns the most recently triggered hit-stop profile. */
    UFUNCTION(BlueprintPure, Category="Edrath|CombatFeedback")
    FEdrathHitStopProfile GetLastHitStopProfile() const { return LastHitStopProfile; }

    /** Applies localized CustomTimeDilation to a specific actor and schedules automatic restoration. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CombatFeedback")
    void ApplyPerActorHitStopDilation(AActor* TargetActor, float DilationScale, float DurationSeconds);

    /** Returns count of actors currently tracked with pending hit-stop restoration timers. */
    UFUNCTION(BlueprintPure, Category="Edrath|CombatFeedback")
    int32 GetActiveHitStopTimerCount() const { return ActiveActorHitStopTimers.Num(); }

    /** Clears all pending hit-stop restoration timers and restores CustomTimeDilation = 1.0f on active actors. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CombatFeedback")
    void ClearAllActorHitStopTimers();

    UPROPERTY(BlueprintAssignable, Category="Edrath|CombatFeedback")
    FOnHitStopTriggered OnHitStopTriggered;

    UPROPERTY(BlueprintAssignable, Category="Edrath|CombatFeedback")
    FOnPoiseShattered OnPoiseShattered;

private:
    FEdrathHitStopProfile BuildProfileForSeverity(EEdrathImpactSeverity Severity) const;

    UPROPERTY()
    FEdrathHitStopProfile LastHitStopProfile;

    TMap<TWeakObjectPtr<AActor>, FTimerHandle> ActiveActorHitStopTimers;

    float LastHitStopTimestampSeconds = -10.0f;
    float ActiveHitStopEndTimestampSeconds = -10.0f;
    float HitStopRefractoryCooldownSeconds = 0.12f;
    float PerfectParryWindowSeconds = 0.14f;
};
