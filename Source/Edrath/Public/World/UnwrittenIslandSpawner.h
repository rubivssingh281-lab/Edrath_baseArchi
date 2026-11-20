#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "UnwrittenIslandSpawner.generated.h"

class AActor;
class UHierarchicalInstancedStaticMeshComponent;

USTRUCT(BlueprintType)
struct FUnwrittenIslandInstance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UnwrittenSea")
    FGameplayTag SourcePossibilityTag;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UnwrittenSea")
    TObjectPtr<AActor> IslandActor = nullptr;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UnwrittenSea")
    FVector WorldLocation = FVector::ZeroVector;
};

UCLASS()
class EDRATH_API AUnwrittenIslandSpawner : public AActor
{
    GENERATED_BODY()

public:
    AUnwrittenIslandSpawner();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|UnwrittenSea")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> IslandInstanceComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UnwrittenSea")
    TSubclassOf<AActor> MaterializedIslandClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UnwrittenSea")
    float IslandSpacing = 1600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UnwrittenSea")
    float IslandCullDistance = 6500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UnwrittenSea")
    int32 MaxVisibleIslands = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UnwrittenSea")
    TObjectPtr<AActor> CullingReferenceActor;

    UFUNCTION(BlueprintCallable, Category="Edrath|UnwrittenSea")
    int32 MaterializeUnchosenPossibilities();

    UFUNCTION(BlueprintCallable, Category="Edrath|UnwrittenSea")
    void RefreshDistanceCulling();

    UFUNCTION(BlueprintPure, Category="Edrath|UnwrittenSea")
    TArray<FUnwrittenIslandInstance> GetMaterializedIslands() const { return MaterializedIslands; }

private:
    UPROPERTY()
    TArray<FUnwrittenIslandInstance> MaterializedIslands;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedIslandActors;
};
