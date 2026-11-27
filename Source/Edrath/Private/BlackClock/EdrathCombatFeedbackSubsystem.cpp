#include "BlackClock/EdrathCombatFeedbackSubsystem.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "Core/EdrathAudioSubsystem.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UEdrathCombatFeedbackSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ActiveActorHitStopTimers.Empty();
    LastHitStopTimestampSeconds = -10.0f;
    ActiveHitStopEndTimestampSeconds = -10.0f;
    LastHitStopProfile = FEdrathHitStopProfile();
}

void UEdrathCombatFeedbackSubsystem::Deinitialize()
{
    ClearAllActorHitStopTimers();
    Super::Deinitialize();
}

FEdrathHitStopProfile UEdrathCombatFeedbackSubsystem::BuildProfileForSeverity(EEdrathImpactSeverity Severity) const
{
    FEdrathHitStopProfile Profile;

    switch (Severity)
    {
        case EEdrathImpactSeverity::LightSlash:
            Profile.DurationSeconds = 0.035f;
            Profile.TimeDilationScale = 0.35f;
            Profile.CameraShakeIntensity = 0.20f;
            Profile.ImpactAudioTag = FName(TEXT("Impact_LightSlash"));
            Profile.bTriggersChronalRipple = false;
            break;

        case EEdrathImpactSeverity::HeavyStrike:
            Profile.DurationSeconds = 0.075f;
            Profile.TimeDilationScale = 0.12f;
            Profile.CameraShakeIntensity = 0.55f;
            Profile.ImpactAudioTag = FName(TEXT("Impact_HeavyStrike"));
            Profile.bTriggersChronalRipple = false;
            break;

        case EEdrathImpactSeverity::EchoResonance:
            Profile.DurationSeconds = 0.095f;
            Profile.TimeDilationScale = 0.08f;
            Profile.CameraShakeIntensity = 0.70f;
            Profile.ImpactAudioTag = FName(TEXT("Impact_EchoResonance"));
            Profile.bTriggersChronalRipple = true;
            break;

        case EEdrathImpactSeverity::PerfectParry:
            Profile.DurationSeconds = 0.110f;
            Profile.TimeDilationScale = 0.04f;
            Profile.CameraShakeIntensity = 0.85f;
            Profile.ImpactAudioTag = FName(TEXT("Impact_PerfectParry"));
            Profile.bTriggersChronalRipple = true;
            break;

        case EEdrathImpactSeverity::PoiseShatter:
            Profile.DurationSeconds = 0.145f;
            Profile.TimeDilationScale = 0.03f;
            Profile.CameraShakeIntensity = 1.00f;
            Profile.ImpactAudioTag = FName(TEXT("Impact_PoiseShatter"));
            Profile.bTriggersChronalRipple = true;
            break;

        default:
            break;
    }

    return Profile;
}

FEdrathHitStopProfile UEdrathCombatFeedbackSubsystem::TriggerHitStop(EEdrathImpactSeverity Severity, float CurrentWorldTimeSeconds)
{
    FEdrathHitStopProfile Profile = BuildProfileForSeverity(Severity);

    // Prevent rapid multi-hit Echo Strikes from stacking freeze frames into stuttering
    const float TimeSinceLast = CurrentWorldTimeSeconds - LastHitStopTimestampSeconds;
    if (TimeSinceLast < HitStopRefractoryCooldownSeconds)
    {
        Profile.DurationSeconds *= 0.25f;
        Profile.TimeDilationScale = FMath::Max(Profile.TimeDilationScale, 0.50f);
        Profile.CameraShakeIntensity *= 0.40f;
    }

    // Respect thermal throttling envelope on laptop/mobile hardware
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathHardwareOptimizationSubsystem* Hardware = GI->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
        {
            const EEdrathThermalStatus Thermal = Hardware->GetCurrentThermalStatus();
            if (Thermal == EEdrathThermalStatus::Critical)
            {
                Profile.DurationSeconds *= 0.50f;
                Profile.bTriggersChronalRipple = false;
            }
        }

        if (UEdrathAudioSubsystem* Audio = GI->GetSubsystem<UEdrathAudioSubsystem>())
        {
            if (Severity == EEdrathImpactSeverity::PerfectParry || Severity == EEdrathImpactSeverity::PoiseShatter)
            {
                Audio->HandleTimelineStateAudioShift(ETimelineState::RejectedTimeline);
            }
        }
    }

    LastHitStopTimestampSeconds = CurrentWorldTimeSeconds;
    ActiveHitStopEndTimestampSeconds = CurrentWorldTimeSeconds + Profile.DurationSeconds;
    LastHitStopProfile = Profile;

    OnHitStopTriggered.Broadcast(Severity, Profile);
    return Profile;
}

FEdrathPoiseEvaluationResult UEdrathCombatFeedbackSubsystem::EvaluatePoiseBreak(
    float CurrentPoise,
    float IncomingPoiseDamage,
    bool bIsParryCounter,
    bool bIsEchoStrike,
    float CurrentWorldTimeSeconds)
{
    FEdrathPoiseEvaluationResult Result;

    float Multiplier = 1.0f;
    if (bIsEchoStrike)
    {
        Multiplier *= 1.35f;
    }
    if (bIsParryCounter)
    {
        Multiplier *= 2.00f;
    }

    const float EffectivePoiseDamage = FMath::Max(0.0f, IncomingPoiseDamage) * Multiplier;
    Result.RemainingPoise = FMath::Max(0.0f, CurrentPoise - EffectivePoiseDamage);
    Result.bPoiseBroken = (CurrentPoise > 0.0f) && (Result.RemainingPoise <= 0.0f);

    if (Result.bPoiseBroken)
    {
        Result.ResultingSeverity = EEdrathImpactSeverity::PoiseShatter;
        Result.StaggerDurationSeconds = bIsEchoStrike ? 2.40f : 1.85f;
        OnPoiseShattered.Broadcast(Result.StaggerDurationSeconds);
    }
    else if (bIsParryCounter)
    {
        Result.ResultingSeverity = EEdrathImpactSeverity::PerfectParry;
    }
    else if (bIsEchoStrike)
    {
        Result.ResultingSeverity = EEdrathImpactSeverity::EchoResonance;
    }
    else if (EffectivePoiseDamage >= 30.0f)
    {
        Result.ResultingSeverity = EEdrathImpactSeverity::HeavyStrike;
    }
    else
    {
        Result.ResultingSeverity = EEdrathImpactSeverity::LightSlash;
    }

    Result.HitStopProfile = TriggerHitStop(Result.ResultingSeverity, CurrentWorldTimeSeconds);
    return Result;
}

bool UEdrathCombatFeedbackSubsystem::EvaluatePerfectParryWindow(float ParryStartTimestamp, float HitImpactTimestamp) const
{
    const float Delta = HitImpactTimestamp - ParryStartTimestamp;
    return (Delta >= 0.0f) && (Delta <= PerfectParryWindowSeconds);
}

float UEdrathCombatFeedbackSubsystem::GetParryClockStabilityReward(bool bIsPerfectParry) const
{
    return bIsPerfectParry ? 20.0f : 10.0f;
}

bool UEdrathCombatFeedbackSubsystem::IsHitStopActive(float CurrentWorldTimeSeconds) const
{
    return (CurrentWorldTimeSeconds >= LastHitStopTimestampSeconds) &&
           (CurrentWorldTimeSeconds <= ActiveHitStopEndTimestampSeconds);
}

void UEdrathCombatFeedbackSubsystem::ClearAllActorHitStopTimers()
{
    for (TPair<TWeakObjectPtr<AActor>, FTimerHandle>& Entry : ActiveActorHitStopTimers)
    {
        if (Entry.Key.IsValid())
        {
            AActor* TrackedActor = Entry.Key.Get();
            TrackedActor->CustomTimeDilation = 1.0f;
            if (UWorld* World = TrackedActor->GetWorld())
            {
                World->GetTimerManager().ClearTimer(Entry.Value);
            }
        }
    }
    ActiveActorHitStopTimers.Empty();
}

void UEdrathCombatFeedbackSubsystem::ApplyPerActorHitStopDilation(AActor* TargetActor, float DilationScale, float DurationSeconds)
{
    if (!IsValid(TargetActor))
    {
        return;
    }

    // Prune any destroyed actors whose weak pointers became invalid
    for (auto It = ActiveActorHitStopTimers.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }

    UWorld* World = TargetActor->GetWorld();
    TWeakObjectPtr<AActor> WeakTarget(TargetActor);

    // Cancel any pre-existing restoration timer so overlapping hits extend instead of truncating hit-stop
    if (FTimerHandle* ExistingHandle = ActiveActorHitStopTimers.Find(WeakTarget))
    {
        if (World)
        {
            World->GetTimerManager().ClearTimer(*ExistingHandle);
        }
        ActiveActorHitStopTimers.Remove(WeakTarget);
    }

    const float ClampedScale = FMath::Clamp(DilationScale, 0.01f, 1.0f);
    TargetActor->CustomTimeDilation = ClampedScale;

    if (World && DurationSeconds > 0.0f)
    {
        TWeakObjectPtr<UEdrathCombatFeedbackSubsystem> WeakSelf(this);
        FTimerHandle& HandleRef = ActiveActorHitStopTimers.Add(WeakTarget);
        World->GetTimerManager().SetTimer(
            HandleRef,
            [WeakSelf, WeakTarget]()
            {
                if (WeakTarget.IsValid())
                {
                    WeakTarget->CustomTimeDilation = 1.0f;
                }
                if (WeakSelf.IsValid())
                {
                    WeakSelf->ActiveActorHitStopTimers.Remove(WeakTarget);
                }
            },
            DurationSeconds,
            false);
    }
}
