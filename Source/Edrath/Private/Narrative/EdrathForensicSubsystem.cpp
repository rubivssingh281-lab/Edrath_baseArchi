#include "Narrative/EdrathForensicSubsystem.h"
#include "Core/EdrathGameplayTags.h"

void UEdrathForensicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ClueMatrix.Empty();
    UnlockedBreakthroughs.Empty();
}

void UEdrathForensicSubsystem::Deinitialize()
{
    ClueMatrix.Empty();
    UnlockedBreakthroughs.Empty();
    Super::Deinitialize();
}

bool UEdrathForensicSubsystem::RegisterDiscoveredClue(FGameplayTag ClueTag, EEdrathRegion Region, const FText& ObservationNotes)
{
    if (!ClueTag.IsValid())
    {
        return false;
    }

    if (ClueMatrix.Contains(ClueTag))
    {
        return false; // Already discovered
    }

    FEdrathClueMatrixNode NewNode;
    NewNode.ClueTag = ClueTag;
    NewNode.Region = Region;
    NewNode.Status = EEdrathClueInvestigationStatus::Discovered;
    NewNode.ObservationNotes = ObservationNotes;
    NewNode.bIsContradictionResolved = false;

    ClueMatrix.Add(ClueTag, NewNode);
    OnClueDiscovered.Broadcast(ClueTag, Region);
    return true;
}

bool UEdrathForensicSubsystem::MarkClueAnalyzed(FGameplayTag ClueTag)
{
    if (FEdrathClueMatrixNode* Node = ClueMatrix.Find(ClueTag))
    {
        if (Node->Status == EEdrathClueInvestigationStatus::Discovered)
        {
            Node->Status = EEdrathClueInvestigationStatus::AnalyzedInLedger;
            return true;
        }
    }
    return false;
}

bool UEdrathForensicSubsystem::CommitBreakthrough(FGameplayTag BreakthroughTag, FGameplayTag ClueA, FGameplayTag ClueB)
{
    if (!BreakthroughTag.IsValid())
    {
        return false;
    }

    if (UnlockedBreakthroughs.Contains(BreakthroughTag))
    {
        return false; // Already unlocked
    }

    UnlockedBreakthroughs.Add(BreakthroughTag);

    if (FEdrathClueMatrixNode* NodeA = ClueMatrix.Find(ClueA))
    {
        NodeA->Status = EEdrathClueInvestigationStatus::SynthesizedBreakthrough;
        NodeA->bIsContradictionResolved = true;
        NodeA->LinkedBreakthroughTag = BreakthroughTag;
    }

    if (FEdrathClueMatrixNode* NodeB = ClueMatrix.Find(ClueB))
    {
        NodeB->Status = EEdrathClueInvestigationStatus::SynthesizedBreakthrough;
        NodeB->bIsContradictionResolved = true;
        NodeB->LinkedBreakthroughTag = BreakthroughTag;
    }

    OnBreakthroughUnlocked.Broadcast(BreakthroughTag, ClueA, ClueB);
    return true;
}

bool UEdrathForensicSubsystem::IsBreakthroughUnlocked(FGameplayTag BreakthroughTag) const
{
    return UnlockedBreakthroughs.Contains(BreakthroughTag);
}

EEdrathClueInvestigationStatus UEdrathForensicSubsystem::GetClueStatus(FGameplayTag ClueTag) const
{
    if (const FEdrathClueMatrixNode* Node = ClueMatrix.Find(ClueTag))
    {
        return Node->Status;
    }
    return EEdrathClueInvestigationStatus::Undiscovered;
}

TArray<FEdrathClueMatrixNode> UEdrathForensicSubsystem::GetCluesForRegion(EEdrathRegion Region) const
{
    TArray<FEdrathClueMatrixNode> OutClues;
    for (const auto& Kvp : ClueMatrix)
    {
        if (Kvp.Value.Region == Region)
        {
            OutClues.Add(Kvp.Value);
        }
    }
    return OutClues;
}

void UEdrathForensicSubsystem::SerializeToSaveGame(UEdrathSaveGame* SaveGame) const
{
    if (!SaveGame)
    {
        return;
    }

    SaveGame->DiscoveredClueTags.Empty();
    for (const auto& Kvp : ClueMatrix)
    {
        SaveGame->DiscoveredClueTags.Add(Kvp.Key);
    }

    SaveGame->ResolvedBreakthroughTags = UnlockedBreakthroughs;
}

void UEdrathForensicSubsystem::DeserializeFromSaveGame(const UEdrathSaveGame* SaveGame)
{
    if (!SaveGame)
    {
        return;
    }

    ClueMatrix.Empty();
    UnlockedBreakthroughs = SaveGame->ResolvedBreakthroughTags;

    for (const FGameplayTag& Tag : SaveGame->DiscoveredClueTags)
    {
        FEdrathClueMatrixNode Node;
        Node.ClueTag = Tag;
        Node.Region = EEdrathRegion::Veyl;
        Node.Status = UnlockedBreakthroughs.Num() > 0 ? EEdrathClueInvestigationStatus::AnalyzedInLedger : EEdrathClueInvestigationStatus::Discovered;
        Node.bIsContradictionResolved = false;
        ClueMatrix.Add(Tag, Node);
    }
}
