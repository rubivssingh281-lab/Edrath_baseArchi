#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "World/EdrathTerrainSubsystem.h"
#include "KaelTerrainAdaptationComponent.generated.h"

class ACharacter;
class USkeletalMeshComponent;

/**
 * UKaelTerrainAdaptationComponent
 * Provides real-time foot IK raycasting against landscape and architectural geometry,
 * calculating dynamic pelvis drops and foot orientation pitch/roll angles to ground
 * Kael realistically on uneven terrain, stairs, and ruins.
 */
UCLASS(ClassGroup=(EdrathCharacter), meta=(BlueprintSpawnableComponent))
class EDRATH_API UKaelTerrainAdaptationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKaelTerrainAdaptationComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Initializes references to owning character and skeletal mesh. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Character|TerrainIK")
    void InitializeTerrainAdaptation(ACharacter* InCharacter, USkeletalMeshComponent* InMesh);

    /** Returns left foot Z translation offset for Control Rig / AnimGraph. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    float GetLeftFootOffset() const { return CurrentLeftFootOffset; }

    /** Returns right foot Z translation offset for Control Rig / AnimGraph. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    float GetRightFootOffset() const { return CurrentRightFootOffset; }

    /** Returns pelvis Z translation drop for Control Rig / AnimGraph. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    float GetPelvisOffset() const { return CurrentPelvisOffset; }

    /** Returns left foot pitch/roll rotation for surface alignment. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    FRotator GetLeftFootRotation() const { return CurrentLeftFootRotation; }

    /** Returns right foot pitch/roll rotation for surface alignment. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    FRotator GetRightFootRotation() const { return CurrentRightFootRotation; }

    /** Returns current detected surface material under character. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    EEdrathSurfaceType GetCurrentSurfaceType() const { return CurrentSurfaceType; }

    /** Returns current ground slope incline angle in degrees. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|TerrainIK")
    float GetTerrainInclineDegrees() const { return CurrentTerrainInclineDegrees; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|TerrainIK|Settings")
    bool bEnableFootIK = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|TerrainIK|Settings")
    float FootTraceUpDistance = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|TerrainIK|Settings")
    float FootTraceDownDistance = 65.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|TerrainIK|Settings")
    float FootInterpSpeed = 16.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|TerrainIK|Settings")
    FName LeftFootSocketName = FName(TEXT("foot_l"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|TerrainIK|Settings")
    FName RightFootSocketName = FName(TEXT("foot_r"));

private:
    void TraceFootSurface(FName SocketName, float& OutTargetOffset, FRotator& OutTargetRotation, FHitResult& OutHit);

    UPROPERTY()
    TObjectPtr<ACharacter> CharacterOwner;

    UPROPERTY()
    TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

    float CurrentLeftFootOffset = 0.0f;
    float CurrentRightFootOffset = 0.0f;
    float CurrentPelvisOffset = 0.0f;

    FRotator CurrentLeftFootRotation = FRotator::ZeroRotator;
    FRotator CurrentRightFootRotation = FRotator::ZeroRotator;

    EEdrathSurfaceType CurrentSurfaceType = EEdrathSurfaceType::Default;
    float CurrentTerrainInclineDegrees = 0.0f;
};
