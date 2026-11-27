#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BeliefBuildingActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeliefBuildingDissolvedSignature, FGameplayTag, BuildingTag, bool, bIsDissolved);

/**
 * ABeliefBuildingActor
 * An architectural structure in the Kingdom of Veyl that physically exists only
 * when citizen collective belief weight remains above ExistenceBeliefThreshold.
 * When belief falls below threshold, the actor seamlessly dissolves via Shaders/PossibilityDissolve.usf
 * material parameter modulation and disables physical collisions.
 */
UCLASS()
class EDRATH_API ABeliefBuildingActor : public AActor
{
    GENERATED_BODY()

public:
    ABeliefBuildingActor();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Veyl")
    TObjectPtr<UStaticMeshComponent> BuildingMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl")
    FGameplayTag BuildingBeliefTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl")
    float ExistenceBeliefThreshold = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl|Dissolve")
    float DissolveInterpSpeed = 3.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl|Dissolve")
    float DissolveEdgeWidth = 0.08f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Veyl|Dissolve")
    FLinearColor DissolveEdgeColor = FLinearColor(0.2f, 0.6f, 1.0f, 1.0f);

    UFUNCTION(BlueprintCallable, Category="Edrath|Veyl")
    void EvaluateBeliefExistence(FGameplayTag ChangedTag, float NewBeliefWeight);

    UFUNCTION(BlueprintPure, Category="Edrath|Veyl")
    float GetCurrentDissolveAlpha() const { return CurrentDissolveAlpha; }

    UFUNCTION(BlueprintPure, Category="Edrath|Veyl")
    bool IsBuildingManifested() const { return bIsManifested; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Veyl")
    FOnBeliefBuildingDissolvedSignature OnBuildingDissolveStateChanged;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    float CurrentDissolveAlpha = 0.0f;
    float TargetDissolveAlpha = 0.0f;
    bool bIsManifested = true;

    UPROPERTY()
    TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
};
