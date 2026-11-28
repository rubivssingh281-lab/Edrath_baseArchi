#include "Core/EdrathVisualFXSubsystem.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Engine/World.h"

void UEdrathVisualFXSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CurrentEnvelope = FEdrathShaderEnvelope();
    TargetEnvelope = FEdrathShaderEnvelope();

    if (UEdrathWeatherSubsystem* Weather = Collection.InitializeDependency<UEdrathWeatherSubsystem>())
    {
        Weather->OnWeatherTransitioned.AddUniqueDynamic(this, &UEdrathVisualFXSubsystem::HandleWeatherTransitioned);
    }

    if (UEdrathCombatFeedbackSubsystem* CombatFeedback = Collection.InitializeDependency<UEdrathCombatFeedbackSubsystem>())
    {
        CombatFeedback->OnHitStopTriggered.AddUniqueDynamic(this, &UEdrathVisualFXSubsystem::HandleCombatHitStopTriggered);
    }
}

void UEdrathVisualFXSubsystem::Deinitialize()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathWeatherSubsystem* Weather = GI->GetSubsystem<UEdrathWeatherSubsystem>())
        {
            Weather->OnWeatherTransitioned.RemoveDynamic(this, &UEdrathVisualFXSubsystem::HandleWeatherTransitioned);
        }
        if (UEdrathCombatFeedbackSubsystem* CombatFeedback = GI->GetSubsystem<UEdrathCombatFeedbackSubsystem>())
        {
            CombatFeedback->OnHitStopTriggered.RemoveDynamic(this, &UEdrathVisualFXSubsystem::HandleCombatHitStopTriggered);
        }
    }

    Super::Deinitialize();
}

void UEdrathVisualFXSubsystem::ApplyTimelineStateVFX(ETimelineState NewTimelineState)
{
    switch (NewTimelineState)
    {
        case ETimelineState::RejectedTimeline:
            TargetEnvelope.TimelineDissolveAlpha = 1.0f;
            TargetEnvelope.ChromaticGlitchIntensity = FMath::Max(TargetEnvelope.ChromaticGlitchIntensity, 0.35f);
            break;

        case ETimelineState::MemoryReconstruction:
            TargetEnvelope.TimelineDissolveAlpha = 0.50f;
            TargetEnvelope.ChromaticGlitchIntensity = FMath::Max(TargetEnvelope.ChromaticGlitchIntensity, 0.55f);
            break;

        case ETimelineState::PresentTimeline:
        default:
            TargetEnvelope.TimelineDissolveAlpha = 0.0f;
            TargetEnvelope.ChromaticGlitchIntensity = 0.0f;
            break;
    }

    OnVisualFXEnvelopeUpdated.Broadcast(CurrentEnvelope);
}

void UEdrathVisualFXSubsystem::UpdateClockInstabilityDesaturation(float StabilityRatio)
{
    const float ClampedStability = FMath::Clamp(StabilityRatio, 0.0f, 1.0f);
    TargetEnvelope.ClockInstabilityDesaturation = FMath::Clamp((0.50f - ClampedStability) * 1.4f, 0.0f, 0.85f);
}

void UEdrathVisualFXSubsystem::TriggerContradictionBreakthroughGlitch(float GlitchIntensity)
{
    const float Clamped = FMath::Clamp(GlitchIntensity, 0.0f, 1.0f);
    CurrentEnvelope.ChromaticGlitchIntensity = Clamped;
    CurrentEnvelope.ChronalRippleIntensity = FMath::Max(CurrentEnvelope.ChronalRippleIntensity, Clamped * 0.8f);
    OnVisualFXEnvelopeUpdated.Broadcast(CurrentEnvelope);
}

void UEdrathVisualFXSubsystem::StepVisualFXBlend(float DeltaSeconds, float InterpSpeed)
{
    if (DeltaSeconds <= 0.0f)
    {
        return;
    }

    // Decay transient combat ripple impulsively while interpolating continuous state channels
    TargetEnvelope.ChronalRippleIntensity = FMath::FInterpTo(TargetEnvelope.ChronalRippleIntensity, 0.0f, DeltaSeconds, InterpSpeed * 0.75f);

    CurrentEnvelope.TimelineDissolveAlpha = FMath::FInterpTo(CurrentEnvelope.TimelineDissolveAlpha, TargetEnvelope.TimelineDissolveAlpha, DeltaSeconds, InterpSpeed);
    CurrentEnvelope.ChromaticGlitchIntensity = FMath::FInterpTo(CurrentEnvelope.ChromaticGlitchIntensity, TargetEnvelope.ChromaticGlitchIntensity, DeltaSeconds, InterpSpeed);
    CurrentEnvelope.FrostGlazeMask = FMath::FInterpTo(CurrentEnvelope.FrostGlazeMask, TargetEnvelope.FrostGlazeMask, DeltaSeconds, InterpSpeed);
    CurrentEnvelope.SporeHazeMask = FMath::FInterpTo(CurrentEnvelope.SporeHazeMask, TargetEnvelope.SporeHazeMask, DeltaSeconds, InterpSpeed);
    CurrentEnvelope.ChronalRippleIntensity = FMath::FInterpTo(CurrentEnvelope.ChronalRippleIntensity, TargetEnvelope.ChronalRippleIntensity, DeltaSeconds, InterpSpeed);
    CurrentEnvelope.ClockInstabilityDesaturation = FMath::FInterpTo(CurrentEnvelope.ClockInstabilityDesaturation, TargetEnvelope.ClockInstabilityDesaturation, DeltaSeconds, InterpSpeed);

    PushEnvelopeToMPC();
    OnVisualFXEnvelopeUpdated.Broadcast(CurrentEnvelope);
}

void UEdrathVisualFXSubsystem::HandleWeatherTransitioned(
    EEdrathWeatherType,
    EEdrathWeatherType,
    const FEdrathWeatherHazardState& HazardState)
{
    TargetEnvelope.FrostGlazeMask = FMath::Clamp(HazardState.FrostGlazeIntensity, 0.0f, 1.0f);
    TargetEnvelope.SporeHazeMask = FMath::Clamp(HazardState.SporeToxicityRate / 1.5f, 0.0f, 1.0f);
}

void UEdrathVisualFXSubsystem::HandleCombatHitStopTriggered(EEdrathImpactSeverity, const FEdrathHitStopProfile& Profile)
{
    if (Profile.bTriggersChronalRipple)
    {
        // Respect laptop/mobile Critical thermal status before firing expensive screen-space ripples
        bool bSuppressRipple = false;
        if (UGameInstance* GI = GetGameInstance())
        {
            if (const UEdrathHardwareOptimizationSubsystem* Hardware = GI->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
            {
                bSuppressRipple = (Hardware->GetCurrentThermalStatus() == EEdrathThermalStatus::Critical);
            }
        }

        if (!bSuppressRipple)
        {
            CurrentEnvelope.ChronalRippleIntensity = FMath::Clamp(Profile.CameraShakeIntensity, 0.25f, 1.0f);
            TargetEnvelope.ChronalRippleIntensity = CurrentEnvelope.ChronalRippleIntensity;
        }
    }
}

void UEdrathVisualFXSubsystem::PushEnvelopeToMPC()
{
    UWorld* World = GetWorld();
    if (!World || !RealityMPC)
    {
        return;
    }

    UKismetMaterialLibrary::SetScalarParameterValue(World, RealityMPC, FName(TEXT("TimelineDissolveAlpha")), CurrentEnvelope.TimelineDissolveAlpha);
    UKismetMaterialLibrary::SetScalarParameterValue(World, RealityMPC, FName(TEXT("ChromaticGlitchIntensity")), CurrentEnvelope.ChromaticGlitchIntensity);
    UKismetMaterialLibrary::SetScalarParameterValue(World, RealityMPC, FName(TEXT("FrostGlazeMask")), CurrentEnvelope.FrostGlazeMask);
    UKismetMaterialLibrary::SetScalarParameterValue(World, RealityMPC, FName(TEXT("SporeHazeMask")), CurrentEnvelope.SporeHazeMask);
    UKismetMaterialLibrary::SetScalarParameterValue(World, RealityMPC, FName(TEXT("ChronalRippleIntensity")), CurrentEnvelope.ChronalRippleIntensity);
    UKismetMaterialLibrary::SetScalarParameterValue(World, RealityMPC, FName(TEXT("ClockInstabilityDesaturation")), CurrentEnvelope.ClockInstabilityDesaturation);
}
