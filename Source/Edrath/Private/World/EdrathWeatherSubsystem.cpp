#include "World/EdrathWeatherSubsystem.h"
#include "Core/EdrathAudioSubsystem.h"

void UEdrathWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UEdrathHardwareOptimizationSubsystem* Hardware = Collection.InitializeDependency<UEdrathHardwareOptimizationSubsystem>())
    {
        CurrentThermalStatus = Hardware->GetCurrentThermalStatus();
        Hardware->OnThermalMitigationApplied.AddUniqueDynamic(this, &UEdrathWeatherSubsystem::HandleThermalMitigationApplied);
    }
    else
    {
        CurrentThermalStatus = EEdrathThermalStatus::Nominal;
    }

    CurrentHazardState.ActiveWeather = EEdrathWeatherType::ClearChronalSky;
    RecalculateHazardMetrics();
}

void UEdrathWeatherSubsystem::Deinitialize()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathHardwareOptimizationSubsystem* Hardware = GI->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
        {
            Hardware->OnThermalMitigationApplied.RemoveDynamic(this, &UEdrathWeatherSubsystem::HandleThermalMitigationApplied);
        }
    }

    Super::Deinitialize();
}

void UEdrathWeatherSubsystem::HandleThermalMitigationApplied(EEdrathThermalStatus NewThermalStatus, float)
{
    ApplyThermalStatusToWeather(NewThermalStatus);
}

void UEdrathWeatherSubsystem::SetActiveWeather(EEdrathWeatherType NewWeather)
{
    if (CurrentHazardState.ActiveWeather == NewWeather)
    {
        return;
    }

    const EEdrathWeatherType OldWeather = CurrentHazardState.ActiveWeather;
    CurrentHazardState.ActiveWeather = NewWeather;
    RecalculateHazardMetrics();

    // Propagate ambient changes to audio subsystem if available
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathAudioSubsystem* Audio = GI->GetSubsystem<UEdrathAudioSubsystem>())
        {
            if (NewWeather == EEdrathWeatherType::CrownBlizzard)
            {
                Audio->SetAcousticProfile(EEdrathAcousticProfile::ExteriorOpenField);
            }
            else if (NewWeather == EEdrathWeatherType::NhalGeothermalSteam)
            {
                Audio->SetAcousticProfile(EEdrathAcousticProfile::DeepSubterranean);
            }
            else if (NewWeather == EEdrathWeatherType::UnwrittenChronalSurge || NewWeather == EEdrathWeatherType::VeylPossibilityFog)
            {
                Audio->SetAcousticProfile(EEdrathAcousticProfile::TimelineDistortion);
            }
        }
    }

    OnWeatherTransitioned.Broadcast(OldWeather, NewWeather, CurrentHazardState);
}

void UEdrathWeatherSubsystem::SetCanonicalRegionWeather(EEdrathRegion Region)
{
    switch (Region)
    {
        case EEdrathRegion::AshenExpanse:
            SetActiveWeather(EEdrathWeatherType::AshenChronalStorm);
            break;
        case EEdrathRegion::Veyl:
            SetActiveWeather(EEdrathWeatherType::VeylPossibilityFog);
            break;
        case EEdrathRegion::Hollowgreen:
            SetActiveWeather(EEdrathWeatherType::HollowgreenSporeRain);
            break;
        case EEdrathRegion::Nhal:
            SetActiveWeather(EEdrathWeatherType::NhalGeothermalSteam);
            break;
        case EEdrathRegion::CrownOfWinter:
            SetActiveWeather(EEdrathWeatherType::CrownBlizzard);
            break;
        case EEdrathRegion::UnwrittenSea:
            SetActiveWeather(EEdrathWeatherType::UnwrittenChronalSurge);
            break;
        default:
            SetActiveWeather(EEdrathWeatherType::ClearChronalSky);
            break;
    }
}

void UEdrathWeatherSubsystem::RecalculateHazardMetrics()
{
    float BaseThermalParticleScale = 1.0f;
    switch (CurrentThermalStatus)
    {
        case EEdrathThermalStatus::Critical:
            BaseThermalParticleScale = 0.25f;
            break;
        case EEdrathThermalStatus::Serious:
            BaseThermalParticleScale = 0.50f;
            break;
        case EEdrathThermalStatus::Fair:
            BaseThermalParticleScale = 0.75f;
            break;
        case EEdrathThermalStatus::Nominal:
        default:
            BaseThermalParticleScale = 1.00f;
            break;
    }

    switch (CurrentHazardState.ActiveWeather)
    {
        case EEdrathWeatherType::AshenChronalStorm:
            CurrentHazardState.VisibilityDistanceUnits = 3500.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 0.75f;
            CurrentHazardState.MovementSpeedMultiplier = 0.90f;
            CurrentHazardState.FrostGlazeIntensity = 0.0f;
            CurrentHazardState.SporeToxicityRate = 0.0f;
            CurrentHazardState.WindIntensity = 0.85f;
            CurrentHazardState.ParticleSpawnRateScale = 1.25f * BaseThermalParticleScale;
            break;

        case EEdrathWeatherType::VeylPossibilityFog:
            CurrentHazardState.VisibilityDistanceUnits = 2500.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 0.25f;
            CurrentHazardState.MovementSpeedMultiplier = 1.0f;
            CurrentHazardState.FrostGlazeIntensity = 0.0f;
            CurrentHazardState.SporeToxicityRate = 0.0f;
            CurrentHazardState.WindIntensity = 0.15f;
            CurrentHazardState.ParticleSpawnRateScale = 0.90f * BaseThermalParticleScale;
            break;

        case EEdrathWeatherType::HollowgreenSporeRain:
            CurrentHazardState.VisibilityDistanceUnits = 5500.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 0.10f;
            CurrentHazardState.MovementSpeedMultiplier = 0.95f;
            CurrentHazardState.FrostGlazeIntensity = 0.0f;
            CurrentHazardState.SporeToxicityRate = 1.20f;
            CurrentHazardState.WindIntensity = 0.35f;
            CurrentHazardState.ParticleSpawnRateScale = 1.10f * BaseThermalParticleScale;
            break;

        case EEdrathWeatherType::NhalGeothermalSteam:
            CurrentHazardState.VisibilityDistanceUnits = 4000.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 0.05f;
            CurrentHazardState.MovementSpeedMultiplier = 0.95f;
            CurrentHazardState.FrostGlazeIntensity = 0.0f;
            CurrentHazardState.SporeToxicityRate = 0.0f;
            CurrentHazardState.WindIntensity = 0.20f;
            CurrentHazardState.ParticleSpawnRateScale = 0.80f * BaseThermalParticleScale;
            break;

        case EEdrathWeatherType::CrownBlizzard:
            CurrentHazardState.VisibilityDistanceUnits = 1800.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 1.10f;
            CurrentHazardState.MovementSpeedMultiplier = 0.70f;
            CurrentHazardState.FrostGlazeIntensity = 0.90f;
            CurrentHazardState.SporeToxicityRate = 0.0f;
            CurrentHazardState.WindIntensity = 1.00f;
            CurrentHazardState.ParticleSpawnRateScale = 1.50f * BaseThermalParticleScale;
            break;

        case EEdrathWeatherType::UnwrittenChronalSurge:
            CurrentHazardState.VisibilityDistanceUnits = 6000.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 1.50f;
            CurrentHazardState.MovementSpeedMultiplier = 1.10f;
            CurrentHazardState.FrostGlazeIntensity = 0.0f;
            CurrentHazardState.SporeToxicityRate = 0.0f;
            CurrentHazardState.WindIntensity = 0.90f;
            CurrentHazardState.ParticleSpawnRateScale = 1.35f * BaseThermalParticleScale;
            break;

        case EEdrathWeatherType::ClearChronalSky:
        default:
            CurrentHazardState.VisibilityDistanceUnits = 14000.0f;
            CurrentHazardState.ChronalInstabilityRatePerSec = 0.0f;
            CurrentHazardState.MovementSpeedMultiplier = 1.0f;
            CurrentHazardState.FrostGlazeIntensity = 0.0f;
            CurrentHazardState.SporeToxicityRate = 0.0f;
            CurrentHazardState.WindIntensity = 0.10f;
            CurrentHazardState.ParticleSpawnRateScale = 1.0f * BaseThermalParticleScale;
            break;
    }
}

int32 UEdrathWeatherSubsystem::CalculateThermalScaledParticleCount(int32 BaseParticleCount) const
{
    return FMath::Max(10, FMath::RoundToInt(static_cast<float>(BaseParticleCount) * CurrentHazardState.ParticleSpawnRateScale));
}

void UEdrathWeatherSubsystem::ApplyThermalStatusToWeather(EEdrathThermalStatus ThermalStatus)
{
    CurrentThermalStatus = ThermalStatus;
    RecalculateHazardMetrics();
}
