#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "EdrathDualCameraComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraPerspectiveChanged, EEdrathCameraMode, NewMode, AActor*, InteriorVolumeActor);

UCLASS(ClassGroup=(EdrathCamera), meta=(BlueprintSpawnableComponent))
class EDRATH_API UEdrathDualCameraComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEdrathDualCameraComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void InitializeCameraRig(USpringArmComponent* InSpringArm, UCameraComponent* InCamera, ACharacter* InOwnerCharacter);

    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void SetCameraPerspective(EEdrathCameraMode NewMode, AActor* TriggeringVolume = nullptr);

    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void AdjustExteriorZoom(float WheelAxisDelta);

    UFUNCTION(BlueprintCallable, Category="Edrath|Camera")
    void RotateExteriorYaw(float YawDeltaDegrees);

    UFUNCTION(BlueprintPure, Category="Edrath|Camera")
    EEdrathCameraMode GetCurrentCameraMode() const { return CurrentMode; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Camera")
    FOnCameraPerspectiveChanged OnCameraPerspectiveChanged;

protected:
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    float InteriorTargetArmLength = 240.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    float InteriorDefaultPitchDegrees = -12.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    float InteriorFOV = 75.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Interior")
    FVector InteriorSocketOffset = FVector(0.0f, 65.0f, 45.0f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Camera|Transition")
    float BlendInterpSpeed = 4.5f;

private:
    UPROPERTY()
    TObjectPtr<USpringArmComponent> SpringArmComp;
    UPROPERTY()
    TObjectPtr<UCameraComponent> CameraComp;
    UPROPERTY()
    TObjectPtr<ACharacter> OwnerCharacter;
    EEdrathCameraMode CurrentMode = EEdrathCameraMode::ExteriorBirdsEye;
};
