#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BuriedFutureGraveActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGraveExhumedSignature, bool, bLovingFatherMemory);

/**
 * ABuriedFutureGraveActor
 * Implements the interactive grave site for Quest.BuriedFuture (The Man Who Buried His Future).
 * The grave contains the farmer's own corpse dated 30 years into the future.
 * Exhuming it mutates the family's memory state between loving father and violent stranger.
 */
UCLASS()
class EDRATH_API ABuriedFutureGraveActor : public AActor
{
    GENERATED_BODY()

public:
    ABuriedFutureGraveActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> GraveMoundMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> FutureCorpseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> FarmerCharacterActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> FarmerWifeCharacterActor;

    /** Exhumes the grave and chooses which memory timeline anchors the family. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ExhumeFutureGrave(bool bAnchorLovingFatherMemory);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsExhumed() const { return bIsExhumed; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsLovingFatherAnchor() const { return bLovingFatherAnchor; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnGraveExhumedSignature OnGraveExhumed;

private:
    bool bIsExhumed = false;
    bool bLovingFatherAnchor = true;
};
