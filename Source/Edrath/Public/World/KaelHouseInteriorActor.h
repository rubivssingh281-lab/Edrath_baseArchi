#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/EdrathTypes.h"
#include "KaelHouseInteriorActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AEdrathInteriorTransitionVolume;
class AForensicClueActor;
class AAct1PreAlphaSliceDirector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHouseInteriorStateChangedSignature, bool, bPlayerInside);

/**
 * Kael House Interior Actor - Zone 4 Settlement Interior
 * Encapsulates Kael's personal residence in the 7-day looping settlement.
 * Coordinates roof occlusion dithering, dual-perspective OTS camera transition,
 * and the two forensic clue anchors (handwriting desk and scratched woman portrait).
 */
UCLASS()
class EDRATH_API AKaelHouseInteriorActor : public AActor
{
    GENERATED_BODY()

public:
    AKaelHouseInteriorActor();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UStaticMeshComponent> HouseStructureMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UStaticMeshComponent> ExteriorRoofMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UBoxComponent> InteriorBoundsTrigger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Forensics")
    TObjectPtr<AForensicClueActor> HandwritingClue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Forensics")
    TObjectPtr<AForensicClueActor> ScratchedPortraitClue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|State")
    bool bIsPlayerInside = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|State")
    bool bHandwritingInspected = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|State")
    bool bPortraitInspected = false;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Events")
    FOnHouseInteriorStateChangedSignature OnHouseInteriorStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Edrath|House")
    void SetInteriorMode(bool bInside);

    UFUNCTION(BlueprintPure, Category = "Edrath|House")
    bool AreAllForensicCluesInspected() const { return bHandwritingInspected && bPortraitInspected; }

    UFUNCTION(BlueprintCallable, Category = "Edrath|House")
    void BindToDirector(AAct1PreAlphaSliceDirector* InDirector);

protected:
    UFUNCTION()
    void HandleInteriorBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void HandleInteriorEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

    UFUNCTION()
    void HandleClueInspected(FGameplayTag EvidenceTag, AActor* Inspector);

private:
    UPROPERTY()
    TWeakObjectPtr<AAct1PreAlphaSliceDirector> BoundDirector;
};
