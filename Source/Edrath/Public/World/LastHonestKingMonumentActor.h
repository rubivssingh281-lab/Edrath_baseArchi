#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "LastHonestKingMonumentActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLastHonestKingResolvedSignature, bool, bExposedTrueCriminal);

/**
 * ALastHonestKingMonumentActor
 * Implements the royal plaza monolith for Quest.LastHonestKing (The Last Honest King).
 * The monarch publicly took the blame for atrocities committed by an erased general.
 * Resolves the choice: exposing the erased war criminal (clearing king, reigniting civil war)
 * vs preserving the false confession to sustain national peace.
 */
UCLASS()
class EDRATH_API ALastHonestKingMonumentActor : public AActor
{
    GENERATED_BODY()

public:
    ALastHonestKingMonumentActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> MonumentMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> ConfessionPlaqueMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> KingCharacterActor;

    /** Forensically inspects the plaque to uncover traces of an erased name under the king's signature. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool InspectConfessionPlaque();

    /** Resolves the royal confession: true = expose erased criminal (causes war), false = preserve peace */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveRoyalConfession(bool bExposeTrueCriminal);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsPlaqueInspected() const { return bPlaqueInspected; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsMonumentResolved() const { return bIsResolved; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool WasTrueCriminalExposed() const { return bTrueCriminalExposed; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnLastHonestKingResolvedSignature OnMonumentResolved;

private:
    bool bPlaqueInspected = false;
    bool bIsResolved = false;
    bool bTrueCriminalExposed = false;
};
