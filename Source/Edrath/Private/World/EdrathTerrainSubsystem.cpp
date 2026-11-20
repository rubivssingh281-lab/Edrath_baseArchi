#include "World/EdrathTerrainSubsystem.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void UEdrathTerrainSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UEdrathTerrainSubsystem::DetectSurfaceUnderLocation(const UObject* WorldContextObject, const FVector& WorldLocation, float TraceDistance, FHitResult& OutHit) const
{
    if (!WorldContextObject)
    {
        return false;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return false;
    }

    const FVector TraceStart = WorldLocation + FVector(0.0f, 0.0f, 50.0f);
    const FVector TraceEnd = WorldLocation - FVector(0.0f, 0.0f, FMath::Max(10.0f, TraceDistance));

    FCollisionQueryParams Params(TEXT("EdrathTerrainSurfaceQuery"), true);
    Params.bReturnPhysicalMaterial = true;

    return World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, Params);
}

EEdrathSurfaceType UEdrathTerrainSubsystem::GetSurfaceTypeFromHit(const FHitResult& HitResult) const
{
    if (!HitResult.bBlockingHit)
    {
        return EEdrathSurfaceType::Default;
    }

    // Check Physical Material Surface Type if assigned
    if (HitResult.PhysMaterial.IsValid())
    {
        const EPhysicalSurface PhysSurface = HitResult.PhysMaterial->SurfaceType;
        switch (PhysSurface)
        {
            case SurfaceType1: return EEdrathSurfaceType::Stone;
            case SurfaceType2: return EEdrathSurfaceType::DirtMud;
            case SurfaceType3: return EEdrathSurfaceType::AshenSand;
            case SurfaceType4: return EEdrathSurfaceType::FleshCorpses;
            case SurfaceType5: return EEdrathSurfaceType::DeepSnow;
            case SurfaceType6: return EEdrathSurfaceType::WaterShallow;
            case SurfaceType7: return EEdrathSurfaceType::Wood;
            default: break;
        }
    }

    // Fallback: Check actor tags or component tags for environment tagging
    if (const AActor* HitActor = HitResult.GetActor())
    {
        if (HitActor->ActorHasTag(TEXT("Terrain_Stone")) || HitActor->ActorHasTag(TEXT("Cathedral"))) return EEdrathSurfaceType::Stone;
        if (HitActor->ActorHasTag(TEXT("Terrain_Snow")) || HitActor->ActorHasTag(TEXT("Frost"))) return EEdrathSurfaceType::DeepSnow;
        if (HitActor->ActorHasTag(TEXT("Terrain_Ash")) || HitActor->ActorHasTag(TEXT("Desert"))) return EEdrathSurfaceType::AshenSand;
        if (HitActor->ActorHasTag(TEXT("Terrain_Flesh")) || HitActor->ActorHasTag(TEXT("Corpse"))) return EEdrathSurfaceType::FleshCorpses;
        if (HitActor->ActorHasTag(TEXT("Terrain_Mud")) || HitActor->ActorHasTag(TEXT("Forest"))) return EEdrathSurfaceType::DirtMud;
        if (HitActor->ActorHasTag(TEXT("Terrain_Wood")) || HitActor->ActorHasTag(TEXT("Plank"))) return EEdrathSurfaceType::Wood;
    }

    return EEdrathSurfaceType::Stone;
}

float UEdrathTerrainSubsystem::CalculateSlopeAngleDegrees(const FVector& SurfaceNormal) const
{
    if (SurfaceNormal.IsNearlyZero())
    {
        return 0.0f;
    }

    const FVector Norm = SurfaceNormal.GetSafeNormal();
    const float DotUp = FMath::Clamp(Norm.Z, -1.0f, 1.0f);
    return FMath::RadiansToDegrees(FMath::Acos(DotUp));
}

float UEdrathTerrainSubsystem::GetSurfaceFrictionMultiplier(EEdrathSurfaceType SurfaceType) const
{
    switch (SurfaceType)
    {
        case EEdrathSurfaceType::DeepSnow:     return 0.60f;
        case EEdrathSurfaceType::DirtMud:      return 0.82f;
        case EEdrathSurfaceType::AshenSand:    return 0.88f;
        case EEdrathSurfaceType::WaterShallow: return 0.70f;
        case EEdrathSurfaceType::FleshCorpses: return 0.90f;
        case EEdrathSurfaceType::Wood:         return 0.95f;
        case EEdrathSurfaceType::Stone:
        case EEdrathSurfaceType::Default:
        default:                               return 1.00f;
    }
}

FName UEdrathTerrainSubsystem::GetSurfaceAudioTag(EEdrathSurfaceType SurfaceType) const
{
    switch (SurfaceType)
    {
        case EEdrathSurfaceType::Stone:        return FName(TEXT("Footstep_Stone"));
        case EEdrathSurfaceType::DirtMud:      return FName(TEXT("Footstep_Mud"));
        case EEdrathSurfaceType::AshenSand:    return FName(TEXT("Footstep_Ash"));
        case EEdrathSurfaceType::FleshCorpses: return FName(TEXT("Footstep_Flesh"));
        case EEdrathSurfaceType::DeepSnow:     return FName(TEXT("Footstep_Snow"));
        case EEdrathSurfaceType::WaterShallow: return FName(TEXT("Footstep_Water"));
        case EEdrathSurfaceType::Wood:         return FName(TEXT("Footstep_Wood"));
        case EEdrathSurfaceType::Default:
        default:                               return FName(TEXT("Footstep_Default"));
    }
}

void UEdrathTerrainSubsystem::BroadcastFootstepLanding(EEdrathSurfaceType SurfaceType, const FVector& ImpactPoint, bool bIsLeftFoot)
{
    OnFootstepLanded.Broadcast(SurfaceType, ImpactPoint, bIsLeftFoot);
}
