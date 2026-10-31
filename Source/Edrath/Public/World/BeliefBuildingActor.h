#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BeliefBuildingActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class EDRATH_API ABeliefBuildingActor : public AActor
{
    GENERATED_BODY()

public:
    ABeliefBuildingActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Veyl")
    TObjectPtr<UStaticMeshComponent> BuildingMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl")
    FGameplayTag BuildingBeliefTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl")
    float ExistenceBeliefThreshold = 0.5f;

    UFUNCTION(BlueprintCallable, Category="Edrath|Veyl")
    void EvaluateBeliefExistence(FGameplayTag ChangedTag, float NewBeliefWeight);

protected:
    virtual void BeginPlay() override;
};
