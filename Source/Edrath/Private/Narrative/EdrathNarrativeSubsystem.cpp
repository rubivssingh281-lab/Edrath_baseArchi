#include "Narrative/EdrathNarrativeSubsystem.h"

void UEdrathNarrativeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    const TArray<FName> CoreCompanions{TEXT("Ilyra"), TEXT("Orren"), TEXT("Senn"), TEXT("Maevra")};
    for (const FName& Name : CoreCompanions)
    {
        FCompanionRelationshipState State;
        State.CompanionID = Name;
        State.Loyalty = Name == TEXT("Maevra") ? 25.0f : 55.0f;
        CompanionStates.Add(Name, State);
    }
}

void UEdrathNarrativeSubsystem::AdvanceSettlementDay()
{
    ++CurrentSettlementDay;
    if (!bAct1LoopBroken && CurrentSettlementDay > 7)
    {
        CurrentSettlementDay = 1;
        ++SettlementLoopResetCount;
        OnSettlementLoopReset.Broadcast(SettlementLoopResetCount);
    }
}

void UEdrathNarrativeSubsystem::BreakAct1SettlementLoop()
{
    bAct1LoopBroken = true;
}

void UEdrathNarrativeSubsystem::SetCitizenBeliefWeight(FGameplayTag BuildingTag, float NewWeight)
{
    const float ClampedWeight = FMath::Clamp(NewWeight, 0.0f, 1.0f);
    BuildingBeliefWeights.Add(BuildingTag, ClampedWeight);
    OnBeliefWeightChanged.Broadcast(BuildingTag, ClampedWeight);
}

float UEdrathNarrativeSubsystem::GetCitizenBeliefWeight(FGameplayTag BuildingTag) const
{
    if (const float* Weight = BuildingBeliefWeights.Find(BuildingTag))
    {
        return *Weight;
    }
    return 1.0f;
}

void UEdrathNarrativeSubsystem::RecordRecoveredMemory(FGameplayTag MemoryTag, FName TrueOwnerNPC)
{
    MemoryTrueOwnerRegistry.Add(MemoryTag, TrueOwnerNPC);
}

void UEdrathNarrativeSubsystem::AddOrMutateEvidence(FGameplayTag EvidenceTag, FText NewDescription, bool bIsTwistClue)
{
    FEdrathEvidenceRecord& Record = EvidenceRegistry.FindOrAdd(EvidenceTag);
    if (Record.EvidenceTag.IsValid())
    {
        ++Record.MutationVersion;
    }
    else
    {
        Record.EvidenceTag = EvidenceTag;
        Record.MutationVersion = 1;
    }
    Record.CurrentDescription = MoveTemp(NewDescription);
    Record.bIsForeshadowingTwistClue = bIsTwistClue;
}

void UEdrathNarrativeSubsystem::RecordForestMutation(FGameplayTag MutationTag, FText MutationDescription)
{
    int32& MutationVersion = ForestMutationVersions.FindOrAdd(MutationTag);
    ++MutationVersion;
    AddOrMutateEvidence(MutationTag, MoveTemp(MutationDescription), true);
    OnForestMutationChanged.Broadcast(MutationTag, MutationVersion);
}

int32 UEdrathNarrativeSubsystem::GetForestMutationVersion(FGameplayTag MutationTag) const
{
    return ForestMutationVersions.FindRef(MutationTag);
}

void UEdrathNarrativeSubsystem::RegisterLivingNpcMemory(FGuid LivingNpcId, FText InitialMemoryDescription)
{
    if (LivingNpcId.IsValid())
    {
        LivingNpcMemoryRegistry.FindOrAdd(LivingNpcId) = MoveTemp(InitialMemoryDescription);
    }
}

void UEdrathNarrativeSubsystem::MutateLivingNpcMemory(FGuid LivingNpcId, FText NewMemoryDescription)
{
    if (LivingNpcId.IsValid())
    {
        LivingNpcMemoryRegistry.Add(LivingNpcId, NewMemoryDescription);
        OnLivingNpcMemoryMutated.Broadcast(LivingNpcId, NewMemoryDescription);
    }
}

FText UEdrathNarrativeSubsystem::GetLivingNpcMemory(FGuid LivingNpcId) const
{
    if (const FText* Memory = LivingNpcMemoryRegistry.Find(LivingNpcId))
    {
        return *Memory;
    }
    return FText::GetEmpty();
}

void UEdrathNarrativeSubsystem::AddUnchosenPossibilityTag(FGameplayTag PossibilityTag)
{
    if (PossibilityTag.IsValid())
    {
        UnchosenPossibilityTags.AddUnique(PossibilityTag);
    }
}

void UEdrathNarrativeSubsystem::RemoveUnchosenPossibilityTag(FGameplayTag PossibilityTag)
{
    UnchosenPossibilityTags.Remove(PossibilityTag);
}

void UEdrathNarrativeSubsystem::RegisterBlackClockActivation(EEdrathRegion Region, float InstabilityDelta)
{
    ++ClockUsagePrunedFuturesCount;
    float& Instability = RegionalInstabilityMap.FindOrAdd(Region);
    Instability = FMath::Clamp(Instability + InstabilityDelta, 0.0f, 100.0f);
}

void UEdrathNarrativeSubsystem::AdvanceExteriorTimeDays(int32 SimulatedDays)
{
    TotalExteriorDaysElapsed += FMath::Max(0, SimulatedDays);
}

TArray<FEdrathEvidenceRecord> UEdrathNarrativeSubsystem::GetAllEvidenceRecords() const
{
    TArray<FEdrathEvidenceRecord> OutRecords;
    EvidenceRegistry.GenerateValueArray(OutRecords);
    return OutRecords;
}

void UEdrathNarrativeSubsystem::RestoreEvidenceRecords(const TArray<FEdrathEvidenceRecord>& InRecords)
{
    EvidenceRegistry.Empty();
    for (const FEdrathEvidenceRecord& Record : InRecords)
    {
        if (Record.EvidenceTag.IsValid())
        {
            EvidenceRegistry.Add(Record.EvidenceTag, Record);
        }
    }
}

void UEdrathNarrativeSubsystem::RestoreSettlementLoopState(int32 Day, bool bBroken)
{
    CurrentSettlementDay = FMath::Clamp(Day, 1, 7);
    bAct1LoopBroken = bBroken;
}

bool UEdrathNarrativeSubsystem::HasUnlockedFourthPath() const
{
    int32 ClueCount = 0;
    for (const TPair<FGameplayTag, FEdrathEvidenceRecord>& Pair : EvidenceRegistry)
    {
        ClueCount += Pair.Value.bIsForeshadowingTwistClue ? 1 : 0;
    }
    return ClueCount >= 6;
}

EEdrathEndingType UEdrathNarrativeSubsystem::EvaluateFinalEnding(EEdrathEndingType PlayerSelectedPhilosophy) const
{
    if (PlayerSelectedPhilosophy == EEdrathEndingType::UncomfortableTruth && !HasUnlockedFourthPath())
    {
        return EEdrathEndingType::MortalWorld;
    }
    return PlayerSelectedPhilosophy;
}
