#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathInteractableInterface.h"
#include "DragonMemoryHoardActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDragonHoardResolvedSignature, bool, bMemoriesReturned);

/**
 * ADragonMemoryHoardActor
 * Implements the mountain pass crystal hoard for Quest.DragonNoFire (A Dragon With No Fire).
 * The hoard holds crystallized memories stolen across Edrath, keeping the ancient dragon pacified.
 * Returning memories restores citizens' minds but unleashes the dragon's catastrophic wrath.
 */
UCLASS()
class EDRATH_API ADragonMemoryHoardActor : public AActor, public IEdrathInteractableInterface
{
    GENERATED_BODY()

public:
    ADragonMemoryHoardActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> CrystalHoardMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> DragonActorMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USphereComponent> InteractionSphere;

    /** Resolves whether to return the memories (restoring apocalyptic dragon) or preserve peaceful amnesia. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveHoardChoice(bool bReturnMemoriesToCitizens);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsHoardResolved() const { return bIsResolved; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool WereMemoriesReturned() const { return bMemoriesReturned; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsDragonApocalyptic() const { return bDragonApocalyptic; }

    // IEdrathInteractableInterface
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionTargetName_Implementation() const override;
    virtual bool ExecuteInteraction_Implementation(AActor* Interactor) override;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnDragonHoardResolvedSignature OnDragonHoardResolved;

private:
    bool bIsResolved = false;
    bool bMemoriesReturned = false;
    bool bDragonApocalyptic = false;
};
