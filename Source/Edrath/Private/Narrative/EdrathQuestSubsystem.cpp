#include "Narrative/EdrathQuestSubsystem.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "World/HouseWithOneMoreRoomActor.h"

void UEdrathQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    const TArray<FGameplayTag> QuestTags{
        EdrathGameplayTags::QuestWeddingNotHappened,
        EdrathGameplayTags::QuestBuriedFuture,
        EdrathGameplayTags::QuestVillageVotesOnDead,
        EdrathGameplayTags::QuestDragonNoFire,
        EdrathGameplayTags::QuestLastHonestKing,
        EdrathGameplayTags::QuestHouseOneMoreRoom};
    for (const FGameplayTag& QuestTag : QuestTags)
    {
        EnsureQuestInitialized(QuestTag);
    }
}

void UEdrathQuestSubsystem::EnsureQuestInitialized(FGameplayTag QuestTag)
{
    if (!QuestTag.IsValid())
    {
        return;
    }
    FEdrathQuestState& State = QuestStates.FindOrAdd(QuestTag);
    State.QuestTag = QuestTag;
}

void UEdrathQuestSubsystem::StartQuest(FGameplayTag QuestTag)
{
    EnsureQuestInitialized(QuestTag);
    if (FEdrathQuestState* State = QuestStates.Find(QuestTag))
    {
        if (State->Progress == EEdrathQuestProgress::Inactive)
        {
            State->Progress = EEdrathQuestProgress::Active;
        }
    }
}

FEdrathQuestState UEdrathQuestSubsystem::GetQuestState(FGameplayTag QuestTag) const
{
    if (const FEdrathQuestState* State = QuestStates.Find(QuestTag))
    {
        return *State;
    }
    return FEdrathQuestState();
}

bool UEdrathQuestSubsystem::IsQuestResolved(FGameplayTag QuestTag) const
{
    const FEdrathQuestState* State = QuestStates.Find(QuestTag);
    return State && State->Progress == EEdrathQuestProgress::Resolved;
}

bool UEdrathQuestSubsystem::HasStarted(FGameplayTag QuestTag) const
{
    const FEdrathQuestState* State = QuestStates.Find(QuestTag);
    return State && State->Progress != EEdrathQuestProgress::Inactive;
}

void UEdrathQuestSubsystem::RecordUnchosenBranch(FGameplayTag UnchosenBranchTag)
{
    if (!UnchosenBranchTag.IsValid() || !GetGameInstance())
    {
        return;
    }
    if (UEdrathNarrativeSubsystem* Narrative = GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
    {
        Narrative->AddUnchosenPossibilityTag(UnchosenBranchTag);
        OnQuestBranchRecorded.Broadcast(UnchosenBranchTag);
    }
}

bool UEdrathQuestSubsystem::ResolveQuest(
    FGameplayTag QuestTag,
    EEdrathQuestOutcome Outcome,
    FGameplayTag ChosenBranchTag,
    FGameplayTag UnchosenBranchTag,
    FText Consequence)
{
    StartQuest(QuestTag);
    FEdrathQuestState* State = QuestStates.Find(QuestTag);
    if (!State || State->Progress == EEdrathQuestProgress::Resolved)
    {
        return false;
    }
    State->Progress = EEdrathQuestProgress::Resolved;
    State->Outcome = Outcome;
    State->ChosenBranchTag = ChosenBranchTag;
    State->WorldConsequence = MoveTemp(Consequence);
    if (GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->AddOrMutateEvidence(QuestTag, State->WorldConsequence, true);
        }
    }
    RecordUnchosenBranch(UnchosenBranchTag);
    OnQuestResolved.Broadcast(QuestTag, Outcome);
    return true;
}

void UEdrathQuestSubsystem::RegisterWeddingFamilyActor(AActor* FamilyActor)
{
    if (FamilyActor)
    {
        WeddingFamilyActors.AddUnique(FamilyActor);
    }
}

void UEdrathQuestSubsystem::RegisterLivingVillagerActor(AActor* VillagerActor)
{
    if (VillagerActor)
    {
        LivingVillagerActors.AddUnique(VillagerActor);
    }
}

bool UEdrathQuestSubsystem::ResolveWeddingNotHappened(bool bMergeTimelines)
{
    if (bMergeTimelines)
    {
        for (AActor* FamilyActor : WeddingFamilyActors)
        {
            if (FamilyActor)
            {
                FamilyActor->Destroy();
            }
        }
        WeddingFamilyActors.Reset();
        return ResolveQuest(
            EdrathGameplayTags::QuestWeddingNotHappened,
            EEdrathQuestOutcome::WeddingTimelinesMerged,
            EdrathGameplayTags::QuestWeddingMergeTimelines,
            EdrathGameplayTags::QuestWeddingSeverMemoryEcho,
            FText::FromString(TEXT("The wedding timelines merge and the current families are erased.")));
    }
    return ResolveQuest(
        EdrathGameplayTags::QuestWeddingNotHappened,
        EEdrathQuestOutcome::WeddingMemoryEchoSevered,
        EdrathGameplayTags::QuestWeddingSeverMemoryEcho,
        EdrathGameplayTags::QuestWeddingMergeTimelines,
        FText::FromString(TEXT("The forty-year memory echo is severed and both present families remain.")));
}

bool UEdrathQuestSubsystem::ResolveBuriedFuture(bool bLovingFatherMemory)
{
    return ResolveQuest(
        EdrathGameplayTags::QuestBuriedFuture,
        bLovingFatherMemory ? EEdrathQuestOutcome::BuriedFutureLovingFather : EEdrathQuestOutcome::BuriedFutureViolentStranger,
        bLovingFatherMemory
            ? EdrathGameplayTags::QuestBuriedFutureLovingFather
            : EdrathGameplayTags::QuestBuriedFutureViolentStranger,
        bLovingFatherMemory
            ? EdrathGameplayTags::QuestBuriedFutureViolentStranger
            : EdrathGameplayTags::QuestBuriedFutureLovingFather,
        bLovingFatherMemory
            ? FText::FromString(TEXT("The family remembers a loving father."))
            : FText::FromString(TEXT("The family remembers a violent stranger.")));
}

bool UEdrathQuestSubsystem::ResolveVillageVotesOnDead(bool bResurrectDeadVillager, bool bExposeRiggedVote)
{
    if (bResurrectDeadVillager && !LivingVillagerActors.IsEmpty())
    {
        if (AActor* ErasedVillager = LivingVillagerActors[0])
        {
            ErasedVillager->Destroy();
        }
        LivingVillagerActors.RemoveAt(0);
    }
    bMayorVoteExposed = bExposeRiggedVote;
    const EEdrathQuestOutcome Outcome = bResurrectDeadVillager
        ? EEdrathQuestOutcome::DeadVillagerResurrected
        : (bExposeRiggedVote ? EEdrathQuestOutcome::MayorVoteExposed : EEdrathQuestOutcome::MayorVotePreserved);
    return ResolveQuest(
        EdrathGameplayTags::QuestVillageVotesOnDead,
        Outcome,
        bResurrectDeadVillager
            ? EdrathGameplayTags::QuestVillageResurrectDead
            : (bExposeRiggedVote
                ? EdrathGameplayTags::QuestVillageExposeMayor
                : EdrathGameplayTags::QuestVillagePreserveMayor),
        bResurrectDeadVillager
            ? EdrathGameplayTags::QuestVillagePreserveMayor
            : (bExposeRiggedVote
                ? EdrathGameplayTags::QuestVillagePreserveMayor
                : EdrathGameplayTags::QuestVillageExposeMayor),
        bResurrectDeadVillager
            ? FText::FromString(TEXT("A deceased villager returns and a living villager is erased retroactively."))
            : (bExposeRiggedVote
                ? FText::FromString(TEXT("The mayor's rigged vote is exposed."))
                : FText::FromString(TEXT("The rigged vote remains hidden to preserve the village order.")));
}

bool UEdrathQuestSubsystem::ResolveDragonNoFire(bool bReturnCrystallizedMemories)
{
    bDragonApocalyptic = bReturnCrystallizedMemories;
    return ResolveQuest(
        EdrathGameplayTags::QuestDragonNoFire,
        bReturnCrystallizedMemories ? EEdrathQuestOutcome::DragonMemoriesReturned : EEdrathQuestOutcome::DragonPeacePreserved,
        bReturnCrystallizedMemories
            ? EdrathGameplayTags::QuestDragonReturnMemories
            : EdrathGameplayTags::QuestDragonPreservePeace,
        bReturnCrystallizedMemories
            ? EdrathGameplayTags::QuestDragonPreservePeace
            : EdrathGameplayTags::QuestDragonReturnMemories,
        bReturnCrystallizedMemories
            ? FText::FromString(TEXT("Citizens recover their memories and the dragon's apocalyptic fire returns."))
            : FText::FromString(TEXT("The dragon remains peaceful while the citizens remain amnesiac.")));
}

bool UEdrathQuestSubsystem::ResolveLastHonestKing(bool bExposeTrueWarCriminal)
{
    bCivilWarReignited = bExposeTrueWarCriminal;
    return ResolveQuest(
        EdrathGameplayTags::QuestLastHonestKing,
        bExposeTrueWarCriminal ? EEdrathQuestOutcome::TrueWarCriminalExposed : EEdrathQuestOutcome::InnocentKingBlamed,
        bExposeTrueWarCriminal
            ? EdrathGameplayTags::QuestLastKingExposeTruth
            : EdrathGameplayTags::QuestLastKingPreservePeace,
        bExposeTrueWarCriminal
            ? EdrathGameplayTags::QuestLastKingPreservePeace
            : EdrathGameplayTags::QuestLastKingExposeTruth,
        bExposeTrueWarCriminal
            ? FText::FromString(TEXT("The erased war criminal is exposed and civil war reignites."))
            : FText::FromString(TEXT("The innocent king bears the blame so the peace survives.")));
}

void UEdrathQuestSubsystem::BindHouseWithOneMoreRoom(AHouseWithOneMoreRoomActor* HouseActor)
{
    HouseWithOneMoreRoom = HouseActor;
}

bool UEdrathQuestSubsystem::RegisterHouseOwnerRecollection()
{
    if (IsQuestResolved(EdrathGameplayTags::QuestHouseOneMoreRoom))
    {
        return false;
    }
    StartQuest(EdrathGameplayTags::QuestHouseOneMoreRoom);
    ++HouseOwnerRecollectionCount;
    if (HouseWithOneMoreRoom)
    {
        HouseWithOneMoreRoom->OnOwnerRememberedSpawnNextRoom();
    }
    return true;
}

bool UEdrathQuestSubsystem::ResolveHouseOneMoreRoom()
{
    if (HouseOwnerRecollectionCount < FMath::Max(1, RequiredHouseOwnerRecollections) || !HouseWithOneMoreRoom)
    {
        return false;
    }
    if (!HouseWithOneMoreRoom->UnlockFinalAtrocityRoom())
    {
        return false;
    }
    return ResolveQuest(
        EdrathGameplayTags::QuestHouseOneMoreRoom,
        EEdrathQuestOutcome::FinalAtrocityRoomUnlocked,
        EdrathGameplayTags::QuestHouseRememberOwner,
        FGameplayTag(),
        FText::FromString(TEXT("The final atrocity room opens at the end of the expanding house.")));
}
