#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "EdrathDualCameraComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraPerspectiveChanged, EEdrathCameraMode, NewMode, AActor*, InteriorVolumeActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraTransitionProgress, float, TransitionAlpha);

/**
 * UEdrathDualCameraComponent
 * Manages seamless transitions between Exterior Bird's-Eye (Isometric 38 deg)
 * and Interior Over-the-Shoulder Third-Person perspective with anti-clipping collision probing,
 * sub-frame quaternion slerp doorway orientation, and field-of-view interpolation.
 */
UCLASS(ClassGroup=(EdrathCamera), meta=(BlueprintSpawnableComponent))
class EDRATH_API UEdrathDualCameraComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEdrathDualCameraComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Initializes references to the owning character's SpringArm and Camera components. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void InitializeCameraRig(USpringArmComponent* InSpringArm, UCameraComponent* InCamera, ACharacter* InOwnerCharacter);

    /** Transitions smoothly between Exterior Bird's-Eye and Interior Third-Person modes with optional doorway alignment. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void SetCameraPerspective(EEdrathCameraMode NewMode, AActor* TriggeringVolume = nullptr);

    /** Adjusts zoom distance when in Exterior Bird's-Eye mode. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void AdjustExteriorZoom(float WheelAxisDelta);

    /** Rotates the Exterior Bird's-Eye camera yaw in degree increments or smooth orbit. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void RotateExteriorYaw(float YawDeltaDegrees);

    /** Returns active perspective mode. */
    UFUNCTION(BlueprintPure, Category="Edrath|Camera")
    EEdrathCameraMode GetCurrentCameraMode() const { return CurrentMode; }

    /** Returns whether perspective is currently in dynamic transition. */
    UFUNCTION(BlueprintPure, Category="Edrath|Camera")
    bool IsBlendingPerspective() const { return bIsBlendingPerspective; }

    /** Returns transition progress in range [0.0f, 1.0f]. */
    UFUNCTION(BlueprintPure, Category="Edrath|Camera")
    float GetTransitionAlpha() const { return TransitionAlpha; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Camera")
    FOnCameraPerspectiveChanged OnCameraPerspectiveChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Camera")
    FOnCameraTransitionProgress OnCameraTransitionProgress;

protected:
    // --- Exterior Bird's-Eye Isometric Settings ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    float ExteriorTargetArmLength = 1150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    float ExteriorMinArmLength = 850.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    float ExteriorMaxArmLength = 1450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    float ExteriorPitchDegrees = -38.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    float ExteriorYawDegrees = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    float ExteriorFOV = 28.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Exterior")
    FVector ExteriorSocketOffset = FVector::ZeroVector;

    // --- Interior Over-the-Shoulder Third-Person Settings ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    float InteriorTargetArmLength = 240.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    float InteriorDefaultPitchDegrees = -12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    float InteriorFOV = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    FVector InteriorSocketOffset = FVector(0.0f, 65.0f, 45.0f);

    // --- Dynamic Transition & Anti-Clipping Settings ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Transition")
    float BlendInterpSpeed = 4.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Transition")
    float TransitionDuration = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Transition")
    float ProbeSphereRadius = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Transition")
    bool bEnableAntiClippingProbe = true;

private:
    void PerformCameraAntiClippingSweep(float DesiredArmLength);
    void UpdateDoorwayQuaternionSlerp(float DeltaTime);

    UPROPERTY()
    TObjectPtr<USpringArmComponent> SpringArmComp;

    UPROPERTY()
    TObjectPtr<UCameraComponent> CameraComp;

    UPROPERTY()
    TObjectPtr<ACharacter> OwnerCharacter;

    EEdrathCameraMode CurrentMode = EEdrathCameraMode::ExteriorBirdsEye;

    bool bIsBlendingPerspective = false;
    float TransitionAlpha = 1.0f;
    float TransitionElapsed = 0.0f;

    float BlendStartArmLength = 1150.0f;
    float BlendTargetArmLength = 1150.0f;

    FVector BlendStartSocketOffset = FVector::ZeroVector;
    FVector BlendTargetSocketOffset = FVector::ZeroVector;

    float BlendStartFOV = 28.0f;
    float BlendTargetFOV = 28.0f;

    FRotator TargetDoorwayRotation = FRotator::ZeroRotator;
    bool bAligningDoorwayRotation = false;
};
