#include "Camera/EdrathDualCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Core/EdrathAudioSubsystem.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "UI/EdrathHUD.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"

UEdrathDualCameraComponent::UEdrathDualCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
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
    SpringArmComp->bEnableCameraLag = bEnableCameraLag;
    SpringArmComp->CameraLagSpeed = CameraLagSpeed;
    SpringArmComp->bEnableCameraRotationLag = bEnableCameraRotationLag;
    SpringArmComp->CameraRotationLagSpeed = CameraRotationLagSpeed;
    SpringArmComp->CameraLagMaxDistance = CameraLagMaxDistance;
    SpringArmComp->SetRelativeRotation(FRotator(ExteriorPitchDegrees, ExteriorYawDegrees, 0.0f));
    CameraComp->SetFieldOfView(ExteriorFOV);

    BlendStartArmLength = ExteriorTargetArmLength;
    BlendTargetArmLength = ExteriorTargetArmLength;
    BlendStartSocketOffset = ExteriorSocketOffset;
    BlendTargetSocketOffset = ExteriorSocketOffset;
    BlendStartFOV = ExteriorFOV;
    BlendTargetFOV = ExteriorFOV;
    TransitionAlpha = 1.0f;
    bIsBlendingPerspective = false;
}

void UEdrathDualCameraComponent::SetCameraPerspective(EEdrathCameraMode NewMode, AActor* TriggeringVolume)
{
    if (CurrentMode == NewMode && !bIsBlendingPerspective)
    {
        return;
    }

    CurrentMode = NewMode;

    // Notify Hardware Optimization Subsystem & Audio Subsystem atomically
    if (OwnerCharacter && OwnerCharacter->GetWorld() && OwnerCharacter->GetWorld()->GetGameInstance())
    {
        UGameInstance* GI = OwnerCharacter->GetWorld()->GetGameInstance();
        if (UEdrathHardwareOptimizationSubsystem* HardwareOptimization =
            GI->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
        {
            HardwareOptimization->ApplyCameraModeOptimization(NewMode);
        }
        if (UEdrathAudioSubsystem* AudioSubsystem = GI->GetSubsystem<UEdrathAudioSubsystem>())
        {
            AudioSubsystem->SetCameraPerspectiveAudioMode(NewMode);
        }
    }

    if (OwnerCharacter)
    {
        if (APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController()))
        {
            if (AEdrathHUD* HUD = Cast<AEdrathHUD>(PC->GetHUD()))
            {
                HUD->SetReticlePerspectiveMode(NewMode);
            }
        }
    }

    // Initialize Transition Interpolation Anchors
    if (SpringArmComp && CameraComp)
    {
        BlendStartArmLength = SpringArmComp->TargetArmLength;
        BlendStartSocketOffset = SpringArmComp->SocketOffset;
        BlendStartFOV = CameraComp->FieldOfView;

        const bool bEnteringInterior = (NewMode == EEdrathCameraMode::InteriorThirdPerson);
        BlendTargetArmLength = bEnteringInterior ? InteriorTargetArmLength : ExteriorTargetArmLength;
        BlendTargetSocketOffset = bEnteringInterior ? InteriorSocketOffset : ExteriorSocketOffset;
        BlendTargetFOV = bEnteringInterior ? InteriorFOV : ExteriorFOV;

        bIsBlendingPerspective = true;
        TransitionAlpha = 0.0f;
        TransitionElapsed = 0.0f;
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

            // Doorway Orientation Alignment via Quaternion Slerp
            if (TriggeringVolume)
            {
                const FVector EntryFacing = TriggeringVolume->GetActorForwardVector();
                TargetDoorwayRotation = EntryFacing.Rotation();
                TargetDoorwayRotation.Pitch = InteriorDefaultPitchDegrees;
                TargetDoorwayRotation.Roll = 0.0f;
                bAligningDoorwayRotation = true;
            }
            else if (APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
            {
                FRotator ControlRotation = OwnerCharacter->GetActorRotation();
                ControlRotation.Pitch = InteriorDefaultPitchDegrees;
                PlayerController->SetControlRotation(ControlRotation);
                bAligningDoorwayRotation = false;
            }
        }
        else
        {
            SpringArmComp->bUsePawnControlRotation = false;
            SpringArmComp->bDoCollisionTest = false;
            OwnerCharacter->bUseControllerRotationYaw = false;
            bAligningDoorwayRotation = false;

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
        if (!bIsBlendingPerspective)
        {
            BlendTargetArmLength = ExteriorTargetArmLength;
        }
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

void UEdrathDualCameraComponent::SnapExteriorYaw(float AngleStepDegrees)
{
    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye && AngleStepDegrees > 0.1f)
    {
        const float Snapped = FMath::GridSnap(ExteriorYawDegrees, AngleStepDegrees);
        ExteriorYawDegrees = FMath::Fmod(Snapped, 360.0f);
        if (ExteriorYawDegrees < 0.0f)
        {
            ExteriorYawDegrees += 360.0f;
        }
    }
}

void UEdrathDualCameraComponent::PlayCameraImpulse(float Magnitude, FVector Direction)
{
    if (FMath::IsNearlyZero(Magnitude))
    {
        return;
    }
    const FVector NormDir = Direction.IsNearlyZero() ? FVector(0.0f, 0.0f, -1.0f) : Direction.GetSafeNormal();
    CurrentCameraImpulseOffset += NormDir * Magnitude;
}

void UEdrathDualCameraComponent::UpdateCameraImpulse(float DeltaTime)
{
    if (!CurrentCameraImpulseOffset.IsNearlyZero(0.1f))
    {
        CurrentCameraImpulseOffset = FMath::VInterpTo(CurrentCameraImpulseOffset, FVector::ZeroVector, DeltaTime, 14.0f);
    }
    else
    {
        CurrentCameraImpulseOffset = FVector::ZeroVector;
    }
}

void UEdrathDualCameraComponent::UpdateTerrainSlopeAdaptation(float DeltaTime)
{
    if (!bEnableTerrainSlopeAdaptation || !OwnerCharacter || !GetWorld() || CurrentMode != EEdrathCameraMode::ExteriorBirdsEye)
    {
        CurrentTerrainPitchDelta = FMath::FInterpTo(CurrentTerrainPitchDelta, 0.0f, DeltaTime, TerrainPitchInterpSpeed);
        return;
    }

    const FVector CharLoc = OwnerCharacter->GetActorLocation();
    const FVector Forward = OwnerCharacter->GetActorForwardVector();

    const FVector TraceFrontStart = CharLoc + (Forward * TerrainTraceForwardDistance) + FVector(0.0f, 0.0f, 150.0f);
    const FVector TraceFrontEnd = TraceFrontStart - FVector(0.0f, 0.0f, TerrainTraceDownDistance + 150.0f);

    const FVector TraceBackStart = CharLoc - (Forward * (TerrainTraceForwardDistance * 0.5f)) + FVector(0.0f, 0.0f, 150.0f);
    const FVector TraceBackEnd = TraceBackStart - FVector(0.0f, 0.0f, TerrainTraceDownDistance + 150.0f);

    FCollisionQueryParams Params(TEXT("EdrathTerrainCameraSlope"), false, OwnerCharacter);
    FHitResult HitFront, HitBack;

    const bool bHitFront = GetWorld()->LineTraceSingleByChannel(HitFront, TraceFrontStart, TraceFrontEnd, ECC_Visibility, Params);
    const bool bHitBack = GetWorld()->LineTraceSingleByChannel(HitBack, TraceBackStart, TraceBackEnd, ECC_Visibility, Params);

    float TargetPitchDelta = 0.0f;
    if (bHitFront && bHitBack)
    {
        const float HeightDelta = HitFront.ImpactPoint.Z - HitBack.ImpactPoint.Z;
        const float DistanceXY = FVector::Dist2D(HitFront.ImpactPoint, HitBack.ImpactPoint);
        if (DistanceXY > 10.0f)
        {
            const float SlopeAngleRadians = FMath::Atan2(HeightDelta, DistanceXY);
            const float SlopeAngleDegrees = FMath::RadiansToDegrees(SlopeAngleRadians);
            TargetPitchDelta = FMath::Clamp(SlopeAngleDegrees * TerrainPitchCompensationScale, -MaxTerrainPitchDeltaDegrees, MaxTerrainPitchDeltaDegrees);
        }
    }

    CurrentTerrainPitchDelta = FMath::FInterpTo(CurrentTerrainPitchDelta, TargetPitchDelta, DeltaTime, TerrainPitchInterpSpeed);
}

void UEdrathDualCameraComponent::UpdateDoorwayQuaternionSlerp(float DeltaTime)
{
    if (!bAligningDoorwayRotation || !OwnerCharacter)
    {
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PlayerController)
    {
        bAligningDoorwayRotation = false;
        return;
    }

    const FQuat CurrentQuat = PlayerController->GetControlRotation().Quaternion();
    const FQuat TargetQuat = TargetDoorwayRotation.Quaternion();
    const FQuat SlerpedQuat = FQuat::Slerp(CurrentQuat, TargetQuat, FMath::Clamp(DeltaTime * BlendInterpSpeed, 0.0f, 1.0f));

    PlayerController->SetControlRotation(SlerpedQuat.Rotator());

    if (CurrentQuat.Equals(TargetQuat, 0.01f))
    {
        bAligningDoorwayRotation = false;
    }
}

void UEdrathDualCameraComponent::PerformCameraAntiClippingSweep(float DesiredArmLength)
{
    if (!bEnableAntiClippingProbe || !OwnerCharacter || !SpringArmComp || !GetWorld())
    {
        return;
    }

    const FVector TraceStart = OwnerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 60.0f);
    const FVector TraceDirection = -SpringArmComp->GetForwardVector();
    const FVector TraceEnd = TraceStart + (TraceDirection * DesiredArmLength);

    FCollisionQueryParams QueryParams(TEXT("EdrathCameraAntiClip"), false, OwnerCharacter);
    FHitResult HitResult;

    const bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        FQuat::Identity,
        ECC_Camera,
        FCollisionShape::MakeSphere(ProbeSphereRadius),
        QueryParams
    );

    if (bHit && HitResult.Distance > 60.0f)
    {
        // Clamp arm length to safe distance avoiding interior ceiling/wall clipping
        SpringArmComp->TargetArmLength = FMath::Min(SpringArmComp->TargetArmLength, HitResult.Distance - ProbeSphereRadius);
    }
}

void UEdrathDualCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!SpringArmComp || !CameraComp)
    {
        return;
    }

    // Advance dynamic transition alpha
    if (bIsBlendingPerspective)
    {
        TransitionElapsed += DeltaTime;
        TransitionAlpha = FMath::Clamp(TransitionElapsed / FMath::Max(0.01f, TransitionDuration), 0.0f, 1.0f);

        // Smooth cubic ease-in-out curve: 3a^2 - 2a^3
        const float SmoothStepAlpha = TransitionAlpha * TransitionAlpha * (3.0f - 2.0f * TransitionAlpha);

        SpringArmComp->TargetArmLength = FMath::Lerp(BlendStartArmLength, BlendTargetArmLength, SmoothStepAlpha);
        SpringArmComp->SocketOffset = FMath::Lerp(BlendStartSocketOffset, BlendTargetSocketOffset, SmoothStepAlpha);
        CameraComp->SetFieldOfView(FMath::Lerp(BlendStartFOV, BlendTargetFOV, SmoothStepAlpha));

        OnCameraTransitionProgress.Broadcast(TransitionAlpha);

        if (TransitionAlpha >= 1.0f)
        {
            bIsBlendingPerspective = false;
        }
    }
    else
    {
        const bool bExterior = (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye);
        const float TargetArmLength = bExterior ? ExteriorTargetArmLength : InteriorTargetArmLength;
        const FVector TargetSocketOffset = bExterior ? ExteriorSocketOffset : InteriorSocketOffset;
        const float TargetFOV = bExterior ? ExteriorFOV : InteriorFOV;

        SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetArmLength, DeltaTime, BlendInterpSpeed);
        SpringArmComp->SocketOffset = FMath::VInterpTo(SpringArmComp->SocketOffset, TargetSocketOffset, DeltaTime, BlendInterpSpeed);
        CameraComp->SetFieldOfView(FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, BlendInterpSpeed));
    }

    // Update terrain slope adaptation and camera impulse damping
    UpdateTerrainSlopeAdaptation(DeltaTime);
    UpdateCameraImpulse(DeltaTime);

    // Doorway slerp alignment in interior mode
    UpdateDoorwayQuaternionSlerp(DeltaTime);

    // Apply exterior fixed rotation with terrain slope pitch adaptation
    if (CurrentMode == EEdrathCameraMode::ExteriorBirdsEye && !bIsBlendingPerspective)
    {
        const float AdaptedPitch = ExteriorPitchDegrees + CurrentTerrainPitchDelta;
        const FRotator DesiredRotation(AdaptedPitch, ExteriorYawDegrees, 0.0f);
        SpringArmComp->SetRelativeRotation(FMath::RInterpTo(SpringArmComp->GetRelativeRotation(), DesiredRotation, DeltaTime, BlendInterpSpeed));
    }

    // Apply dynamic camera impulse offset
    if (!CurrentCameraImpulseOffset.IsNearlyZero())
    {
        SpringArmComp->SocketOffset += CurrentCameraImpulseOffset;
    }

    // Anti-clipping sphere probe when in third person or transitioning
    if (CurrentMode == EEdrathCameraMode::InteriorThirdPerson || bIsBlendingPerspective)
    {
        PerformCameraAntiClippingSweep(SpringArmComp->TargetArmLength);
    }
}
