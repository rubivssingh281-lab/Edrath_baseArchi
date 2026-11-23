#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "World/EdrathTerrainSubsystem.h"
#include "EdrathAudioSubsystem.generated.h"

/**
 * EEdrathAcousticProfile
 * Defines spatial acoustic zones and environmental reverberation models in Edrath.
 */
UENUM(BlueprintType)
enum class EEdrathAcousticProfile : uint8
{
    ExteriorOpenField           UMETA(DisplayName = "Exterior Open Field"),
    InteriorClaustrophobicCrypt UMETA(DisplayName = "Interior Claustrophobic Crypt"),
    TimelineDistortion          UMETA(DisplayName = "Timeline Distortion / Anomaly"),
    ConsciousArchive            UMETA(DisplayName = "Conscious Archive Cathedral"),
    DeepSubterranean            UMETA(DisplayName = "Deep Subterranean Nhal")
};

/**
 * EEdrathLocomotionGait
 * Character movement intensity states driving acoustic footstep attenuation and cadence.
 */
UENUM(BlueprintType)
enum class EEdrathLocomotionGait : uint8
{
    Sneak       UMETA(DisplayName = "Sneak / Crouch"),
    Walk        UMETA(DisplayName = "Walk / Deliberate"),
    Jog         UMETA(DisplayName = "Jog / Standard"),
    Sprint      UMETA(DisplayName = "Sprint / Panic"),
    DodgeRoll   UMETA(DisplayName = "Dodge Roll / Tactical Slide")
};

/**
 * FEdrathAcousticSettings
 * Concrete filter, reverberation, and spatialization parameters applied to master audio bus.
 */
USTRUCT(BlueprintType)
struct FEdrathAcousticSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Audio")
    float LowPassCutoffHz = 20000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Audio")
    float ReverbWetDryMix = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Audio")
    float ReverbDecayTimeSeconds = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Audio")
    float StereoSpreadWidth = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Audio")
    float FoleyPresenceGain = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Audio")
    float TemporalFlangeRateHz = 0.0f;
};

/**
 * FEdrathFootstepAudioParams
 * Dynamic Foley modulation parameters calculated for surface impact.
 */
USTRUCT(BlueprintType)
struct FEdrathFootstepAudioParams
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    FName AudioCueTag = FName(TEXT("Footstep_Stone"));

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float PitchMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float VolumeMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float ResonanceDamping = 0.1f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    bool bSubmergedSplash = false;
};

/**
 * FEdrathClockAudioState
 * Real-time psychoacoustic parameters for the protagonist's ticking sternum Black Clock.
 */
USTRUCT(BlueprintType)
struct FEdrathClockAudioState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float CurrentBPM = 60.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float PitchShiftMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float SpatialSpreadRadius = 450.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    float DistortionDisturbance = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Audio")
    FVector SternumWorldLocation = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAcousticProfileChanged, EEdrathAcousticProfile, NewProfile, const FEdrathAcousticSettings&, NewSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlackClockAudioTempoChanged, float, NewBPM, float, PitchShift);

/**
 * UEdrathAudioSubsystem
 * GameInstance subsystem governing dual-perspective acoustic profiles,
 * physical surface footstep routing, Black Clock sternum tempo acceleration,
 * and velocity-modulated locomotion cadence.
 */
UCLASS()
class EDRATH_API UEdrathAudioSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Updates acoustic profile and filter parameters based on current camera perspective. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Audio")
    void SetCameraPerspectiveAudioMode(EEdrathCameraMode CameraMode);

    /** Manually sets active acoustic profile (e.g. entering a crypt or underground zone). */
    UFUNCTION(BlueprintCallable, Category="Edrath|Audio")
    void SetAcousticProfile(EEdrathAcousticProfile NewProfile);

    /** Returns currently active acoustic profile. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    EEdrathAcousticProfile GetActiveAcousticProfile() const { return ActiveProfile; }

    /** Returns active acoustic filter and reverb parameters. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    FEdrathAcousticSettings GetActiveAcousticSettings() const { return ActiveSettings; }

    /** Updates the spatialized sternum ticking tempo and distortion based on clock stability ratio (0.0 to 1.0). */
    UFUNCTION(BlueprintCallable, Category="Edrath|Audio")
    void UpdateBlackClockAcousticTempo(float StabilityRatio, const FVector& SternumLocation);

    /** Returns the current audio state of the sternum Black Clock. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    FEdrathClockAudioState GetBlackClockAudioState() const { return ClockAudioState; }

    /** Calculates dynamic heart-rate BPM from clock stability ratio. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    float CalculateBlackClockBPM(float StabilityRatio) const;

    /** Processes base footstep audio parameters given surface type and left/right foot. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Audio")
    FEdrathFootstepAudioParams ProcessFootstepAudio(EEdrathSurfaceType SurfaceType, const FVector& ImpactPoint, bool bIsLeftFoot);

    /** Processes velocity- and gait-modulated footstep audio with refractory window filtering. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Audio")
    FEdrathFootstepAudioParams ProcessLocomotionFootstepAudio(
        EEdrathSurfaceType SurfaceType,
        const FVector& ImpactPoint,
        bool bIsLeftFoot,
        float GroundSpeed,
        EEdrathLocomotionGait Gait,
        float CurrentWorldTimeSeconds);

    /** Calculates dynamic volume multiplier for gait and speed. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    float CalculateGaitVolumeMultiplier(EEdrathLocomotionGait Gait, float GroundSpeed) const;

    /** Evaluates whether footstep sound is allowed or suppressed by refractory timestamp window. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    bool CheckFootstepRefractoryAllowed(bool bIsLeftFoot, float CurrentWorldTimeSeconds) const;

    /** Responds to timeline state shifts (e.g. Present vs Rejected vs Memory Reconstruction). */
    UFUNCTION(BlueprintCallable, Category="Edrath|Audio")
    void HandleTimelineStateAudioShift(ETimelineState NewTimelineState);

    /** Retrieves the last computed footstep parameters for testing/inspection. */
    UFUNCTION(BlueprintPure, Category="Edrath|Audio")
    FEdrathFootstepAudioParams GetLastFootstepParams() const { return LastFootstepParams; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Audio")
    FOnAcousticProfileChanged OnAcousticProfileChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Audio")
    FOnBlackClockAudioTempoChanged OnBlackClockAudioTempoChanged;

private:
    UFUNCTION()
    void HandleTerrainFootstepLanded(EEdrathSurfaceType SurfaceType, FVector ImpactPoint, bool bIsLeftFoot);

    void RecalculateProfileSettings();

    UPROPERTY()
    EEdrathCameraMode LastCameraMode = EEdrathCameraMode::ExteriorBirdsEye;

    UPROPERTY()
    EEdrathAcousticProfile ActiveProfile = EEdrathAcousticProfile::ExteriorOpenField;

    UPROPERTY()
    FEdrathAcousticSettings ActiveSettings;

    UPROPERTY()
    FEdrathClockAudioState ClockAudioState;

    UPROPERTY()
    FEdrathFootstepAudioParams LastFootstepParams;

    float LastLeftFootTimestampSeconds = -1.0f;
    float LastRightFootTimestampSeconds = -1.0f;
    float FootstepRefractoryWindowSeconds = 0.085f;
};
