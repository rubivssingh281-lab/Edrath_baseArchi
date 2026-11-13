#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EdrathInteriorTransitionVolume.generated.h"

class UBoxComponent;
class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteriorTransitionStateChanged, bool, bIsInterior, AActor*, TransitionVolume);

/**
 * AEdrathInteriorTransitionVolume
 * Handles trigger detection for interior crypts, houses, and dungeons.
 * Smoothly blends camera perspective, manages dithered roof culling fade,
 * and tracks Crown of Winter exterior simulated time dilation.
 */
UCLASS()
class EDRATH_API AEdrathInteriorTransitionVolume : public AActor
{
    GENERATED_BODY()

public:
    AEdrathInteriorTransitionVolume();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Interior")
    TObjectPtr<UBoxComponent> InteriorBoundsBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Interior")
    TObjectPtr<UArrowComponent> DoorwayEntryArrow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior")
    TArray<TObjectPtr<AActor>> RoofActorsToToggle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior")
    bool bIsCrownOfWinterTimeDilatedInterior = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior")
    float SimulatedDaysPerInteriorSecond = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior|RoofFade")
    float RoofFadeDuration = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior|RoofFade")
    FName RoofOpacityMaterialParamName = TEXT("RoofDitherOpacity");

    UFUNCTION(BlueprintPure, Category="Edrath|Interior")
    bool IsPlayerInside() const { return bPlayerInside; }

    UFUNCTION(BlueprintPure, Category="Edrath|Interior")
    float GetCurrentRoofOpacity() const { return CurrentRoofOpacity; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Interior")
    FOnInteriorTransitionStateChanged OnInteriorTransitionStateChanged;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnInteriorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnInteriorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

private:
    void UpdateRoofFade(float DeltaTime);
    void ApplyRoofMaterialOpacity(float Opacity);

    double InteriorEntryTimestampSeconds = 0.0;
    bool bPlayerInside = false;

    float CurrentRoofOpacity = 1.0f;
    float TargetRoofOpacity = 1.0f;
    bool bIsFadingRoof = false;
};
