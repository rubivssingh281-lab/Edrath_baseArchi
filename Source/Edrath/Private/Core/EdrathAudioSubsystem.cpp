#include "Core/EdrathAudioSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UEdrathAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UEdrathTerrainSubsystem* TerrainSubsystem = Collection.InitializeDependency<UEdrathTerrainSubsystem>();
    if (TerrainSubsystem)
    {
        TerrainSubsystem->OnFootstepLanded.AddDynamic(this, &UEdrathAudioSubsystem::HandleTerrainFootstepLanded);
    }

    ActiveProfile = EEdrathAcousticProfile::ExteriorOpenField;
    LastCameraMode = EEdrathCameraMode::ExteriorBirdsEye;
    RecalculateProfileSettings();

    ClockAudioState.CurrentBPM = 60.0f;
    ClockAudioState.PitchShiftMultiplier = 1.0f;
    ClockAudioState.SpatialSpreadRadius = 450.0f;
    ClockAudioState.DistortionDisturbance = 0.0f;
    ClockAudioState.SternumWorldLocation = FVector::ZeroVector;
}

void UEdrathAudioSubsystem::Deinitialize()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathTerrainSubsystem* TerrainSubsystem = GI->GetSubsystem<UEdrathTerrainSubsystem>())
        {
            TerrainSubsystem->OnFootstepLanded.RemoveDynamic(this, &UEdrathAudioSubsystem::HandleTerrainFootstepLanded);
        }
    }

    Super::Deinitialize();
}

void UEdrathAudioSubsystem::SetCameraPerspectiveAudioMode(EEdrathCameraMode CameraMode)
{
    LastCameraMode = CameraMode;

    // Do not override supernatural timeline distortions or conscious archives
    if (ActiveProfile != EEdrathAcousticProfile::TimelineDistortion &&
        ActiveProfile != EEdrathAcousticProfile::ConsciousArchive)
    {
        if (CameraMode == EEdrathCameraMode::ExteriorBirdsEye)
        {
            ActiveProfile = EEdrathAcousticProfile::ExteriorOpenField;
        }
        else
        {
            ActiveProfile = EEdrathAcousticProfile::InteriorClaustrophobicCrypt;
        }
    }

    RecalculateProfileSettings();
}

void UEdrathAudioSubsystem::SetAcousticProfile(EEdrathAcousticProfile NewProfile)
{
    ActiveProfile = NewProfile;
    RecalculateProfileSettings();
}

void UEdrathAudioSubsystem::RecalculateProfileSettings()
{
    switch (ActiveProfile)
    {
        case EEdrathAcousticProfile::ExteriorOpenField:
            ActiveSettings.LowPassCutoffHz = 20000.0f;
            ActiveSettings.ReverbWetDryMix = 0.20f;
            ActiveSettings.ReverbDecayTimeSeconds = 1.2f;
            ActiveSettings.StereoSpreadWidth = 1.0f;
            ActiveSettings.FoleyPresenceGain = 0.85f;
            ActiveSettings.TemporalFlangeRateHz = 0.0f;
            break;

        case EEdrathAcousticProfile::InteriorClaustrophobicCrypt:
            ActiveSettings.LowPassCutoffHz = 12000.0f;
            ActiveSettings.ReverbWetDryMix = 0.55f;
            ActiveSettings.ReverbDecayTimeSeconds = 3.2f;
            ActiveSettings.StereoSpreadWidth = 0.45f;
            ActiveSettings.FoleyPresenceGain = 1.35f;
            ActiveSettings.TemporalFlangeRateHz = 0.0f;
            break;

        case EEdrathAcousticProfile::TimelineDistortion:
            ActiveSettings.LowPassCutoffHz = 4800.0f;
            ActiveSettings.ReverbWetDryMix = 0.75f;
            ActiveSettings.ReverbDecayTimeSeconds = 5.0f;
            ActiveSettings.StereoSpreadWidth = 0.85f;
            ActiveSettings.FoleyPresenceGain = 0.65f;
            ActiveSettings.TemporalFlangeRateHz = 3.5f;
            break;

        case EEdrathAcousticProfile::ConsciousArchive:
            ActiveSettings.LowPassCutoffHz = 16000.0f;
            ActiveSettings.ReverbWetDryMix = 0.85f;
            ActiveSettings.ReverbDecayTimeSeconds = 6.8f;
            ActiveSettings.StereoSpreadWidth = 0.95f;
            ActiveSettings.FoleyPresenceGain = 1.05f;
            ActiveSettings.TemporalFlangeRateHz = 1.2f;
            break;

        case EEdrathAcousticProfile::DeepSubterranean:
            ActiveSettings.LowPassCutoffHz = 8500.0f;
            ActiveSettings.ReverbWetDryMix = 0.60f;
            ActiveSettings.ReverbDecayTimeSeconds = 4.5f;
            ActiveSettings.StereoSpreadWidth = 0.60f;
            ActiveSettings.FoleyPresenceGain = 1.20f;
            ActiveSettings.TemporalFlangeRateHz = 0.5f;
            break;

        default:
            break;
    }

    OnAcousticProfileChanged.Broadcast(ActiveProfile, ActiveSettings);
}

float UEdrathAudioSubsystem::CalculateBlackClockBPM(float StabilityRatio) const
{
    const float ClampedRatio = FMath::Clamp(StabilityRatio, 0.0f, 1.0f);
    const float BaseBPM = 60.0f;
    const float StressBPM = (1.0f - ClampedRatio) * 110.0f;
    const float CriticalBonus = (ClampedRatio < 0.25f) ? ((0.25f - ClampedRatio) / 0.25f) * 30.0f : 0.0f;

    return FMath::Clamp(BaseBPM + StressBPM + CriticalBonus, 60.0f, 200.0f);
}

void UEdrathAudioSubsystem::UpdateBlackClockAcousticTempo(float StabilityRatio, const FVector& SternumLocation)
{
    const float ClampedRatio = FMath::Clamp(StabilityRatio, 0.0f, 1.0f);

    ClockAudioState.CurrentBPM = CalculateBlackClockBPM(ClampedRatio);
    ClockAudioState.PitchShiftMultiplier = 1.0f + (1.0f - ClampedRatio) * 0.35f;
    ClockAudioState.DistortionDisturbance = FMath::Clamp((0.40f - ClampedRatio) / 0.40f, 0.0f, 1.0f);
    ClockAudioState.SternumWorldLocation = SternumLocation;
    ClockAudioState.SpatialSpreadRadius = (LastCameraMode == EEdrathCameraMode::InteriorThirdPerson) ? 350.0f : 550.0f;

    OnBlackClockAudioTempoChanged.Broadcast(ClockAudioState.CurrentBPM, ClockAudioState.PitchShiftMultiplier);
}

FEdrathFootstepAudioParams UEdrathAudioSubsystem::ProcessFootstepAudio(EEdrathSurfaceType SurfaceType, const FVector& ImpactPoint, bool bIsLeftFoot)
{
    FEdrathFootstepAudioParams Params;

    switch (SurfaceType)
    {
        case EEdrathSurfaceType::Stone:
            Params.AudioCueTag = FName(TEXT("Footstep_Stone"));
            Params.PitchMultiplier = 1.00f;
            Params.VolumeMultiplier = 1.00f;
            Params.ResonanceDamping = 0.05f;
            Params.bSubmergedSplash = false;
            break;

        case EEdrathSurfaceType::DirtMud:
            Params.AudioCueTag = FName(TEXT("Footstep_Mud"));
            Params.PitchMultiplier = 0.92f;
            Params.VolumeMultiplier = 0.85f;
            Params.ResonanceDamping = 0.45f;
            Params.bSubmergedSplash = false;
            break;

        case EEdrathSurfaceType::AshenSand:
            Params.AudioCueTag = FName(TEXT("Footstep_Ash"));
            Params.PitchMultiplier = 0.88f;
            Params.VolumeMultiplier = 0.75f;
            Params.ResonanceDamping = 0.60f;
            Params.bSubmergedSplash = false;
            break;

        case EEdrathSurfaceType::FleshCorpses:
            Params.AudioCueTag = FName(TEXT("Footstep_Flesh"));
            Params.PitchMultiplier = 0.80f;
            Params.VolumeMultiplier = 1.15f;
            Params.ResonanceDamping = 0.70f;
            Params.bSubmergedSplash = false;
            break;

        case EEdrathSurfaceType::DeepSnow:
            Params.AudioCueTag = FName(TEXT("Footstep_Snow"));
            Params.PitchMultiplier = 1.10f;
            Params.VolumeMultiplier = 0.70f;
            Params.ResonanceDamping = 0.80f;
            Params.bSubmergedSplash = false;
            break;

        case EEdrathSurfaceType::WaterShallow:
            Params.AudioCueTag = FName(TEXT("Footstep_Water"));
            Params.PitchMultiplier = 1.05f;
            Params.VolumeMultiplier = 1.10f;
            Params.ResonanceDamping = 0.25f;
            Params.bSubmergedSplash = true;
            break;

        case EEdrathSurfaceType::Wood:
            Params.AudioCueTag = FName(TEXT("Footstep_Wood"));
            Params.PitchMultiplier = 1.02f;
            Params.VolumeMultiplier = 1.05f;
            Params.ResonanceDamping = 0.15f;
            Params.bSubmergedSplash = false;
            break;

        case EEdrathSurfaceType::Default:
        default:
            Params.AudioCueTag = FName(TEXT("Footstep_Stone"));
            Params.PitchMultiplier = 1.00f;
            Params.VolumeMultiplier = 0.90f;
            Params.ResonanceDamping = 0.10f;
            Params.bSubmergedSplash = false;
            break;
    }

    // Natural acoustic footstep humanization offset
    const float SubtleHumanization = bIsLeftFoot ? 0.985f : 1.015f;
    Params.PitchMultiplier *= SubtleHumanization;

    LastFootstepParams = Params;
    return Params;
}

void UEdrathAudioSubsystem::HandleTimelineStateAudioShift(ETimelineState NewTimelineState)
{
    switch (NewTimelineState)
    {
        case ETimelineState::PresentTimeline:
            if (LastCameraMode == EEdrathCameraMode::ExteriorBirdsEye)
            {
                SetAcousticProfile(EEdrathAcousticProfile::ExteriorOpenField);
            }
            else
            {
                SetAcousticProfile(EEdrathAcousticProfile::InteriorClaustrophobicCrypt);
            }
            break;

        case ETimelineState::RejectedTimeline:
            SetAcousticProfile(EEdrathAcousticProfile::TimelineDistortion);
            break;

        case ETimelineState::MemoryReconstruction:
            SetAcousticProfile(EEdrathAcousticProfile::ConsciousArchive);
            break;

        default:
            break;
    }
}

void UEdrathAudioSubsystem::HandleTerrainFootstepLanded(EEdrathSurfaceType SurfaceType, FVector ImpactPoint, bool bIsLeftFoot)
{
    ProcessFootstepAudio(SurfaceType, ImpactPoint, bIsLeftFoot);
}
