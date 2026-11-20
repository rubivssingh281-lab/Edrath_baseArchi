#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Chaos/ChaosEngineInterface.h"
#include "EdrathTerrainSubsystem.generated.h"

/**
 * Surface material classifications for Edrath's physical landscape and architecture.
 */
UENUM(BlueprintType)
enum class EEdrathSurfaceType : uint8
{
    Default        UMETA(DisplayName = "Default / Neutral"),
    Stone          UMETA(DisplayName = "Stone / Cobblestone / Cathedral"),
    DirtMud        UMETA(DisplayName = "Dirt / Wet Mud / Forest Soil"),
    AshenSand      UMETA(DisplayName = "Ashen Sand / Desert Ruins"),
    FleshCorpses   UMETA(DisplayName = "Battlefield Corpses / Bone Rubble"),
    DeepSnow       UMETA(DisplayName = "Deep Snow / Frost Glaze"),
    WaterShallow   UMETA(DisplayName = "Shallow Water / River Bed"),
    Wood           UMETA(DisplayName = "Wooden Plank / Timber Bridge")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTerrainFootstepLanded, EEdrathSurfaceType, SurfaceType, FVector, ImpactPoint, bool, bIsLeftFoot);

/**
 * UEdrathTerrainSubsystem
 * Global GameInstance subsystem providing landscape physical material queries,
 * terrain slope calculations, surface-dependent friction, and footstep VFX/SFX routing.
 */
UCLASS()
class EDRATH_API UEdrathTerrainSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** Performs a vertical raycast from a world position downwards to detect terrain surface. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Terrain", meta=(WorldContext="WorldContextObject"))
    bool DetectSurfaceUnderLocation(const UObject* WorldContextObject, const FVector& WorldLocation, float TraceDistance, FHitResult& OutHit) const;

    /** Classifies physical surface type from hit result physical material or actor tags. */
    UFUNCTION(BlueprintPure, Category="Edrath|Terrain")
    EEdrathSurfaceType GetSurfaceTypeFromHit(const FHitResult& HitResult) const;

    /** Calculates slope incline angle in degrees from surface impact normal vector. */
    UFUNCTION(BlueprintPure, Category="Edrath|Terrain")
    float CalculateSlopeAngleDegrees(const FVector& SurfaceNormal) const;

    /** Returns surface-dependent movement friction multiplier (e.g. 0.4 for ice/snow, 1.0 for stone). */
    UFUNCTION(BlueprintPure, Category="Edrath|Terrain")
    float GetSurfaceFrictionMultiplier(EEdrathSurfaceType SurfaceType) const;

    /** Returns footstep sound cue gameplay tag identifier for audio routing. */
    UFUNCTION(BlueprintPure, Category="Edrath|Terrain")
    FName GetSurfaceAudioTag(EEdrathSurfaceType SurfaceType) const;

    /** Notifies subsystem of character footstep landing to trigger global decals or particle listeners. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Terrain")
    void BroadcastFootstepLanding(EEdrathSurfaceType SurfaceType, const FVector& ImpactPoint, bool bIsLeftFoot);

    UPROPERTY(BlueprintAssignable, Category="Edrath|Terrain")
    FOnTerrainFootstepLanded OnFootstepLanded;
};
