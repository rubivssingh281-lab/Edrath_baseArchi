#include "Camera/EdrathDualCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

UEdrathDualCameraComponent::UEdrathDualCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UEdrathDualCameraComponent::InitializeCameraRig(USpringArmComponent* InSpringArm, UCameraComponent* InCamera, ACharacter* InOwnerCharacter)
{
    SpringArmComp = InSpringArm;
    CameraComp = InCamera;
    OwnerCharacter = InOwnerCharacter;
    if (!SpringArmComp || !CameraComp)
    {
        return;
    }

    SpringArmComp->TargetArmLength = ExteriorTargetArmLength;
    SpringArmComp->SocketOffset = ExteriorSocketOffset;
    SpringArmComp->bUsePawnControlRotation = false;
    SpringArmComp->bDoCollisionTest = false;
    SpringArmComp->SetRelativeRotation(FRotator(ExteriorPitchDegrees, ExteriorYawDegrees, 0.0f));
    CameraComp->SetFieldOfView(ExteriorFOV);
}

void UEdrathDualCameraComponent::SetCameraPerspective(EEdrathCameraMode NewMode, AActor* TriggeringVolume)
{
    if (CurrentMode == NewMode)
    {
        return;
    }

    CurrentMode = NewMode;
    if (OwnerCharacter && OwnerCharacter->GetWorld() && OwnerCharacter->GetWorld()->GetGameInstance())
    {
        if (UEdrathHardwareOptimizationSubsystem* HardwareOptimization =
            OwnerCharacter->GetWorld()->GetGameInstance()->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
        {
            HardwareOptimization->ApplyCameraModeOptimization(NewMode);
        }
    }
    if (OwnerCharacter && SpringArmComp)
    {
        UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
        if (NewMode == EEdrathCameraMode::InteriorThirdPerson)
        {
            SpringArmComp->bUsePawnControlRotation = true;
            SpringArmComp->bDoCollisionTest = true;
            OwnerCharacter->bUseControllerRotationYaw = false;
            if (Movement)
            {
                Movement->bOrientRotationToMovement = true;
                Movement->MaxWalkSpeed = 420.0f;
            }
            if (APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
            {
                FRotator ControlRotation = OwnerCharacter->GetActorRotation();
                ControlRotation.Pitch = InteriorDefaultPitchDegrees;
                PlayerController->SetControlRotation(ControlRotation);
            }
        }
        else
        {
            SpringArmComp->bUsePawnControlRotation = false;
            SpringArmComp->bDoCollisionTest = false;
            OwnerCharacter->bUseControllerRotationYaw = false;
            if (Movement)
            {
                Movement->bOrientRotationToMovement = true;
                Movement->MaxWalkSpeed = 600.0f;
            }
        }
    }
    OnCameraPerspectiveChanged.Broadcast(CurrentMode, TriggeringVolume);
}

void UEdrathDualCameraComponent::AdjustExteriorZoom(float WheelAxisDelta)
{
    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        ExteriorTargetArmLength = FMath::Clamp(ExteriorTargetArmLength - WheelAxisDelta * 75.0f, ExteriorMinArmLength, ExteriorMaxArmLength);
    }
}

void UEdrathDualCameraComponent::RotateExteriorYaw(float YawDeltaDegrees)
{
    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        ExteriorYawDegrees = FMath::Fmod(ExteriorYawDegrees + YawDeltaDegrees, 360.0f);
        if (ExteriorYawDegrees < 0.0f)
        {
            ExteriorYawDegrees += 360.0f;
        }
    }
}

void UEdrathDualCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!SpringArmComp || !CameraComp)
    {
        return;
    }

    const bool bExterior = CurrentMode == EEdrathCameraMode::ExteriorBirdsEye;
    const float TargetArmLength = bExterior ? ExteriorTargetArmLength : InteriorTargetArmLength;
    const FVector TargetSocketOffset = bExterior ? ExteriorSocketOffset : InteriorSocketOffset;
    const float TargetFOV = bExterior ? ExteriorFOV : InteriorFOV;
    SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetArmLength, DeltaTime, BlendInterpSpeed);
    SpringArmComp->SocketOffset = FMath::VInterpTo(SpringArmComp->SocketOffset, TargetSocketOffset, DeltaTime, BlendInterpSpeed);
    CameraComp->SetFieldOfView(FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, BlendInterpSpeed));
    if (bExterior)
    {
        const FRotator DesiredRotation(ExteriorPitchDegrees, ExteriorYawDegrees, 0.0f);
        SpringArmComp->SetRelativeRotation(FMath::RInterpTo(SpringArmComp->GetRelativeRotation(), DesiredRotation, DeltaTime, BlendInterpSpeed));
    }
}
