#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/EdrathTypes.h"
#include "World/EdrathWeatherSubsystem.h"
#include "EdrathSaveGame.generated.h"

UCLASS()
class EDRATH_API UEdrathSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UEdrathSaveGame();

    UPROPERTY(VisibleAnywhere, Category="Edrath|SaveData")
    FString SaveSlotName = TEXT("Edrath_AutoSave");

    UPROPERTY(VisibleAnywhere, Category="Edrath|SaveData")
    int32 UserIndex = 0;

    UPROPERTY(VisibleAnywhere, Category="Edrath|SaveData")
    FDateTime SaveTimestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Progression")
    int32 CurrentActNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Progression")
    EEdrathRegion CurrentRegion = EEdrathRegion::Veyl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Player")
    FVector PlayerWorldPosition = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Player")
    FRotator PlayerWorldRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Player")
    float PlayerHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Player")
    float PlayerMaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|BlackClock")
    float ClockStability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|BlackClock")
    float MaxClockStability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Act1")
    int32 CurrentSettlementLoopDay = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Act1")
    bool bAct1LoopBroken = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|World")
    TMap<EEdrathRegion, float> RegionalInstabilityMap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    TArray<FEdrathEvidenceRecord> EvidenceRegistry;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    TMap<FName, FCompanionRelationshipState> CompanionRelationshipMap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    TArray<FGameplayTag> UnchosenPossibilityTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    int32 ClockUsagePrunedFuturesCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    bool bUnlockedFourthPath = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    TMap<FGameplayTag, float> BuildingBeliefWeights;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    TMap<FGameplayTag, int32> ForestMutationVersions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Narrative")
    int32 TotalExteriorDaysElapsed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Forensics")
    TArray<FGameplayTag> DiscoveredClueTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Forensics")
    TArray<FEdrathClueMatrixNode> SavedClueMatrixNodes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|Forensics")
    TArray<FGameplayTag> ResolvedBreakthroughTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SaveData|World")
    EEdrathWeatherType ActiveWeatherType = EEdrathWeatherType::ClearChronalSky;
};
