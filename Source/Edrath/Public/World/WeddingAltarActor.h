#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "WeddingAltarActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeddingTimelineResolved, bool, bTimelinesMerged);

/**
 * AWeddingAltarActor
 * Implements the interactive world altar for Quest.WeddingNotHappened (The Wedding That Hasn't Happened).
 * Manages the wedding ritual grounds where two strangers remember 40 years of marriage.
 * Resolves the choice: merging timelines (erasing their current families) vs severing the memory echo.
 */
UCLASS()
class EDRATH_API AWeddingAltarActor : public AActor
{
    GENERATED_BODY()

public:
    AWeddingAltarActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> AltarMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TArray<TObjectPtr<AActor>> CurrentLivingFamilyActors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> GroomActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> BrideActor;

    /** Resolves the wedding quest: true = merge timelines (erasing families), false = sever memory echo */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ExecuteWeddingChoice(bool bMergeTimelines);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsWeddingResolved() const { return bIsResolved; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool WereTimelinesMerged() const { return bMergedTimelines; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnWeddingTimelineResolved OnWeddingTimelineResolved;

private:
    bool bIsResolved = false;
    bool bMergedTimelines = false;
};
