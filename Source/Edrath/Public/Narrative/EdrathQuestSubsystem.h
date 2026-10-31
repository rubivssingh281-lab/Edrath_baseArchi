#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "EdrathQuestSubsystem.generated.h"

class AHouseWithOneMoreRoomActor;

UENUM(BlueprintType)
enum class EEdrathQuestProgress : uint8
{
    Inactive,
    Active,
    Resolved
};

UENUM(BlueprintType)
enum class EEdrathQuestOutcome : uint8
{
    None,
    WeddingTimelinesMerged,
    WeddingMemoryEchoSevered,
    BuriedFutureLovingFather,
    BuriedFutureViolentStranger,
    DeadVillagerResurrected,
    MayorVoteExposed,
    MayorVotePreserved,
    DragonMemoriesReturned,
    DragonPeacePreserved,
    TrueWarCriminalExposed,
    InnocentKingBlamed,
    FinalAtrocityRoomUnlocked
};

USTRUCT(BlueprintType)
struct FEdrathQuestState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Quests")
    FGameplayTag QuestTag;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Quests")
    EEdrathQuestProgress Progress = EEdrathQuestProgress::Inactive;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Quests")
    EEdrathQuestOutcome Outcome = EEdrathQuestOutcome::None;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Quests")
    FGameplayTag ChosenBranchTag;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Quests")
    FText WorldConsequence;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestResolved, FGameplayTag, QuestTag, EEdrathQuestOutcome, Outcome);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestBranchRecorded, FGameplayTag, UnchosenPossibilityTag);

UCLASS()
class EDRATH_API UEdrathQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    void StartQuest(FGameplayTag QuestTag);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    FEdrathQuestState GetQuestState(FGameplayTag QuestTag) const;

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsQuestResolved(FGameplayTag QuestTag) const;

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveWeddingNotHappened(bool bMergeTimelines);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveBuriedFuture(bool bLovingFatherMemory);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveVillageVotesOnDead(bool bResurrectDeadVillager, bool bExposeRiggedVote);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveDragonNoFire(bool bReturnCrystallizedMemories);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveLastHonestKing(bool bExposeTrueWarCriminal);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool RegisterHouseOwnerRecollection();

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool ResolveHouseOneMoreRoom();

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    void RegisterWeddingFamilyActor(AActor* FamilyActor);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    void RegisterLivingVillagerActor(AActor* VillagerActor);

    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    void BindHouseWithOneMoreRoom(AHouseWithOneMoreRoomActor* HouseActor);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    int32 GetHouseOwnerRecollectionCount() const { return HouseOwnerRecollectionCount; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsDragonApocalyptic() const { return bDragonApocalyptic; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsCivilWarReignited() const { return bCivilWarReignited; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnQuestResolved OnQuestResolved;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnQuestBranchRecorded OnQuestBranchRecorded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests|HouseOneMoreRoom")
    int32 RequiredHouseOwnerRecollections = 4;

private:
    void EnsureQuestInitialized(FGameplayTag QuestTag);
    bool ResolveQuest(FGameplayTag QuestTag, EEdrathQuestOutcome Outcome, FGameplayTag ChosenBranchTag, FGameplayTag UnchosenBranchTag, FText Consequence);
    void RecordUnchosenBranch(FGameplayTag UnchosenBranchTag);
    bool HasStarted(FGameplayTag QuestTag) const;

    TMap<FGameplayTag, FEdrathQuestState> QuestStates;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> WeddingFamilyActors;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> LivingVillagerActors;

    UPROPERTY()
    TObjectPtr<AHouseWithOneMoreRoomActor> HouseWithOneMoreRoom;

    int32 HouseOwnerRecollectionCount = 0;
    bool bDragonApocalyptic = false;
    bool bCivilWarReignited = false;
    bool bMayorVoteExposed = false;
};
