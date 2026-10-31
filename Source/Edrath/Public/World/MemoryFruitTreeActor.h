#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/EdrathTypes.h"
#include "MemoryFruitTreeActor.generated.h"

class AActor;
class UStaticMeshComponent;
class ULevelStreamingDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMemoryFruitHarvested, EKaelLifeEchoArchetype, EchoArchetype, AActor*, MemoryArea);

UCLASS()
class EDRATH_API AMemoryFruitTreeActor : public AActor
{
    GENERATED_BODY()

public:
    AMemoryFruitTreeActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Hollowgreen")
    TObjectPtr<UStaticMeshComponent> TreeMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Hollowgreen")
    EKaelLifeEchoArchetype MemoryEchoArchetype = EKaelLifeEchoArchetype::PlaguedHealer_Act3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Hollowgreen")
    FGameplayTag ForestMutationTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Hollowgreen")
    FName MemoryLevelPackage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Hollowgreen")
    TSubclassOf<AActor> MemoryAreaActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Hollowgreen")
    float MemoryAreaStreamingRadius = 4500.0f;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Hollowgreen")
    FOnMemoryFruitHarvested OnMemoryFruitHarvested;

    UFUNCTION(BlueprintCallable, Category="Edrath|Hollowgreen")
    AActor* HarvestMemoryFruit(AActor* Harvester);

    UFUNCTION(BlueprintCallable, Category="Edrath|Hollowgreen")
    void ModifyMemoryAnchor(FGameplayTag AnchorTag, FText MutationDescription);

    UFUNCTION(BlueprintPure, Category="Edrath|Hollowgreen")
    bool HasBeenHarvested() const { return bHasBeenHarvested; }

    UFUNCTION(BlueprintPure, Category="Edrath|Hollowgreen")
    EKaelLifeEchoArchetype GetMemoryEchoArchetype() const { return MemoryEchoArchetype; }

private:
    UPROPERTY()
    TObjectPtr<AActor> SpawnedMemoryArea;

    UPROPERTY()
    TObjectPtr<ULevelStreamingDynamic> StreamedMemoryLevel;

    bool bHasBeenHarvested = false;
};
