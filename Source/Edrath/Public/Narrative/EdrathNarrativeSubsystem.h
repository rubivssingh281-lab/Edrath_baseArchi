#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "EdrathNarrativeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettlementLoopReset, int32, TotalLoopResets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeliefWeightChanged, FGameplayTag, BuildingTag, float, NewBeliefWeight);

UCLASS()
class EDRATH_API UEdrathNarrativeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Act1")
    void AdvanceSettlementDay();

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Act1")
    void BreakAct1SettlementLoop();

    UFUNCTION(BlueprintPure, Category="Edrath|Narrative|Act1")
    int32 GetCurrentSettlementLoopDay() const { return CurrentSettlementDay; }

    UFUNCTION(BlueprintPure, Category="Edrath|Narrative|Act1")
    bool IsSettlementLoopBroken() const { return bAct1LoopBroken; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Act2")
    void SetCitizenBeliefWeight(FGameplayTag BuildingTag, float NewWeight);

    UFUNCTION(BlueprintPure, Category="Edrath|Narrative|Act2")
    float GetCitizenBeliefWeight(FGameplayTag BuildingTag) const;

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Evidence")
    void RecordRecoveredMemory(FGameplayTag MemoryTag, FName TrueOwnerNPC);

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Evidence")
    void AddOrMutateEvidence(FGameplayTag EvidenceTag, FText NewDescription, bool bIsTwistClue);

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Instability")
    void RegisterBlackClockActivation(EEdrathRegion Region, float InstabilityDelta);

    UFUNCTION(BlueprintPure, Category="Edrath|Narrative|Instability")
    int32 GetPrunedFuturesCount() const { return ClockUsagePrunedFuturesCount; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Act5")
    void AdvanceExteriorTimeDays(int32 SimulatedDays);

    UFUNCTION(BlueprintPure, Category="Edrath|Narrative|Act5")
    int32 GetTotalSimulatedExteriorDays() const { return TotalExteriorDaysElapsed; }

    UFUNCTION(BlueprintPure, Category="Edrath|Narrative|Endings")
    bool HasUnlockedFourthPath() const;

    UFUNCTION(BlueprintCallable, Category="Edrath|Narrative|Endings")
    EEdrathEndingType EvaluateFinalEnding(EEdrathEndingType PlayerSelectedPhilosophy) const;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Narrative")
    FOnSettlementLoopReset OnSettlementLoopReset;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Narrative")
    FOnBeliefWeightChanged OnBeliefWeightChanged;

private:
    int32 CurrentSettlementDay = 1;
    int32 SettlementLoopResetCount = 0;
    bool bAct1LoopBroken = false;
    int32 ClockUsagePrunedFuturesCount = 0;
    int32 TotalExteriorDaysElapsed = 0;
    TMap<FGameplayTag, float> BuildingBeliefWeights;
    TMap<FGameplayTag, FName> MemoryTrueOwnerRegistry;
    TMap<FGameplayTag, FEdrathEvidenceRecord> EvidenceRegistry;
    TMap<EEdrathRegion, float> RegionalInstabilityMap;
    TMap<FName, FCompanionRelationshipState> CompanionStates;
};
