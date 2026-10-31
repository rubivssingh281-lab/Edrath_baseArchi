#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EdrathInteriorTransitionVolume.generated.h"

class UBoxComponent;

UCLASS()
class EDRATH_API AEdrathInteriorTransitionVolume : public AActor
{
    GENERATED_BODY()

public:
    AEdrathInteriorTransitionVolume();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Interior")
    TObjectPtr<UBoxComponent> InteriorBoundsBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior")
    TArray<TObjectPtr<AActor>> RoofActorsToToggle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior")
    bool bIsCrownOfWinterTimeDilatedInterior = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interior")
    float SimulatedDaysPerInteriorSecond = 180.0f;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnInteriorOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnInteriorOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

private:
    double InteriorEntryTimestampSeconds = 0.0;
};
