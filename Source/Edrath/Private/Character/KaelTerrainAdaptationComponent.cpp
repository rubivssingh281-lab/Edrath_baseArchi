#include "Character/KaelTerrainAdaptationComponent.h"
#include "CollisionQueryParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

UKaelTerrainAdaptationComponent::UKaelTerrainAdaptationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UKaelTerrainAdaptationComponent::InitializeTerrainAdaptation(ACharacter* InCharacter, USkeletalMeshComponent* InMesh)
{
    CharacterOwner = InCharacter;
    SkeletalMesh = InMesh;
}

void UKaelTerrainAdaptationComponent::TraceFootSurface(FName SocketName, float& OutTargetOffset, FRotator& OutTargetRotation, FHitResult& OutHit)
{
    OutTargetOffset = 0.0f;
    OutTargetRotation = FRotator::ZeroRotator;

    if (!CharacterOwner || !GetWorld())
    {
        return;
    }

    FVector FootLocation = CharacterOwner->GetActorLocation();
    if (SkeletalMesh && SkeletalMesh->DoesSocketExist(SocketName))
    {
        FootLocation = SkeletalMesh->GetSocketLocation(SocketName);
    }
    else
    {
        const float SideSign = (SocketName == LeftFootSocketName) ? -1.0f : 1.0f;
        FootLocation += (CharacterOwner->GetActorRightVector() * (SideSign * 18.0f));
    }

    const FVector TraceStart = FootLocation + FVector(0.0f, 0.0f, FootTraceUpDistance);
    const FVector TraceEnd = FootLocation - FVector(0.0f, 0.0f, FootTraceDownDistance);

    FCollisionQueryParams Params(TEXT("EdrathFootIKTrace"), false, CharacterOwner);
    Params.bReturnPhysicalMaterial = true;

    if (GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, Params))
    {
        OutTargetOffset = OutHit.ImpactPoint.Z - FootLocation.Z;

        const FVector Norm = OutHit.ImpactNormal.GetSafeNormal();
        const float Pitch = FMath::RadiansToDegrees(-FMath::Atan2(Norm.X, Norm.Z));
        const float Roll = FMath::RadiansToDegrees(FMath::Atan2(Norm.Y, Norm.Z));

        OutTargetRotation = FRotator(Pitch, 0.0f, Roll);
    }
}

void UKaelTerrainAdaptationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bEnableFootIK || !CharacterOwner)
    {
        return;
    }

    float TargetLeftOffset = 0.0f;
    float TargetRightOffset = 0.0f;
    FRotator TargetLeftRot = FRotator::ZeroRotator;
    FRotator TargetRightRot = FRotator::ZeroRotator;
    FHitResult HitLeft, HitRight;

    TraceFootSurface(LeftFootSocketName, TargetLeftOffset, TargetLeftRot, HitLeft);
    TraceFootSurface(RightFootSocketName, TargetRightOffset, TargetRightRot, HitRight);

    // Calculate pelvis drop: match the lowest foot so feet maintain ground contact on uneven terrain
    const float TargetPelvisOffset = FMath::Min(0.0f, FMath::Min(TargetLeftOffset, TargetRightOffset));

    // Smooth interpolation to prevent foot snapping
    CurrentLeftFootOffset = FMath::FInterpTo(CurrentLeftFootOffset, TargetLeftOffset - TargetPelvisOffset, DeltaTime, FootInterpSpeed);
    CurrentRightFootOffset = FMath::FInterpTo(CurrentRightFootOffset, TargetRightOffset - TargetPelvisOffset, DeltaTime, FootInterpSpeed);
    CurrentPelvisOffset = FMath::FInterpTo(CurrentPelvisOffset, TargetPelvisOffset, DeltaTime, FootInterpSpeed);

    CurrentLeftFootRotation = FMath::RInterpTo(CurrentLeftFootRotation, TargetLeftRot, DeltaTime, FootInterpSpeed);
    CurrentRightFootRotation = FMath::RInterpTo(CurrentRightFootRotation, TargetRightRot, DeltaTime, FootInterpSpeed);

    // Update physical surface material and slope incline via Terrain Subsystem
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathTerrainSubsystem* TerrainSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathTerrainSubsystem>())
        {
            const FHitResult& DominantHit = HitLeft.bBlockingHit ? HitLeft : HitRight;
            if (DominantHit.bBlockingHit)
            {
                CurrentSurfaceType = TerrainSubsystem->GetSurfaceTypeFromHit(DominantHit);
                CurrentTerrainInclineDegrees = TerrainSubsystem->CalculateSlopeAngleDegrees(DominantHit.ImpactNormal);
            }
        }
    }
}
