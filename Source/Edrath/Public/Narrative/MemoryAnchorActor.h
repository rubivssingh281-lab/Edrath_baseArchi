#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "MemoryAnchorActor.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMemoryAnchorAlteredSignature, FGameplayTag, AnchorTag, FText, NewDescription, AActor*, Instigator);

/**
 * AMemoryAnchorActor
 * Physical interactive memory anchor placed inside Hollowgreen memory sub-levels.
 * Modifying this anchor alters the memory realm and updates parent forest world mutation
 * state (FWorldMutationState) via UEdrathNarrativeSubsystem::RecordForestMutation.
 */
UCLASS()
class EDRATH_API AMemoryAnchorActor : public AActor
{
    GENERATED_BODY()

public:
    AMemoryAnchorActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Memory")
    TObjectPtr<UStaticMeshComponent> AnchorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Memory")
    FGameplayTag AnchorTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Memory")
    EKaelLifeEchoArchetype AssociatedEchoArchetype = EKaelLifeEchoArchetype::PlaguedHealer_Act3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Memory")
    FText CurrentMemoryDescription;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Memory")
    FOnMemoryAnchorAlteredSignature OnMemoryAnchorAltered;

    /** Alters the memory anchor, rewriting this historical realm and updating parent forest mutation state. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Memory")
    bool AlterAnchor(AActor* InstigatorActor, FText NewDescription);

    UFUNCTION(BlueprintPure, Category="Edrath|Memory")
    bool IsAltered() const { return bIsAnchorAltered; }

    UFUNCTION(BlueprintPure, Category="Edrath|Memory")
    FGameplayTag GetAnchorTag() const { return AnchorTag; }

    UFUNCTION(BlueprintPure, Category="Edrath|Memory")
    EKaelLifeEchoArchetype GetAssociatedEchoArchetype() const { return AssociatedEchoArchetype; }

private:
    UPROPERTY()
    bool bIsAnchorAltered = false;
};
