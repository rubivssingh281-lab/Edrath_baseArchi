#include "UI/ForensicLedgerWidget.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UForensicLedgerWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshEvidenceList();
}

void UForensicLedgerWidget::RefreshEvidenceList()
{
    CachedRecords.Empty();

    if (const UWorld* World = GetWorld())
    {
        if (const UGameInstance* GI = World->GetGameInstance())
        {
            if (const UEdrathNarrativeSubsystem* Narrative = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                CachedRecords = Narrative->GetAllEvidenceRecords();
            }
        }
    }

    ApplyFilter();

    if (EvidenceListContainer)
    {
        EvidenceListContainer->ClearChildren();
        for (const FEdrathEvidenceRecord& Record : FilteredRecords)
        {
            OnPopulateEvidenceItem(Record);
        }
    }

    if (FilteredRecords.Num() > 0)
    {
        SelectEvidenceByTag(FilteredRecords[0].EvidenceTag);
    }
    else
    {
        bHasSelection = false;
        SelectedRecord = FEdrathEvidenceRecord();
        if (EvidenceTitleLabel)
        {
            EvidenceTitleLabel->SetText(NSLOCTEXT("EdrathUI", "NoCluesTitle", "No Evidence Collected"));
        }
        if (EvidenceDescriptionLabel)
        {
            EvidenceDescriptionLabel->SetText(NSLOCTEXT("EdrathUI", "NoCluesDesc", "Explore the world to uncover forensic evidence."));
        }
        if (MutationVersionLabel)
        {
            MutationVersionLabel->SetText(FText::GetEmpty());
        }
        if (TwistWarningLabel)
        {
            TwistWarningLabel->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    OnLedgerRefreshed.Broadcast(CachedRecords.Num());
}

void UForensicLedgerWidget::SetFilterMode(EForensicLedgerFilter NewFilter)
{
    CurrentFilter = NewFilter;
    ApplyFilter();
}

void UForensicLedgerWidget::ApplyFilter()
{
    FilteredRecords.Empty();

    for (const FEdrathEvidenceRecord& Record : CachedRecords)
    {
        switch (CurrentFilter)
        {
            case EForensicLedgerFilter::AllClues:
                FilteredRecords.Add(Record);
                break;
            case EForensicLedgerFilter::TwistCluesOnly:
                if (Record.bIsForeshadowingTwistClue)
                {
                    FilteredRecords.Add(Record);
                }
                break;
            case EForensicLedgerFilter::MutatedCluesOnly:
                if (Record.MutationVersion > 1)
                {
                    FilteredRecords.Add(Record);
                }
                break;
        }
    }
}

bool UForensicLedgerWidget::SelectEvidenceByTag(FGameplayTag EvidenceTag)
{
    for (const FEdrathEvidenceRecord& Record : CachedRecords)
    {
        if (Record.EvidenceTag == EvidenceTag)
        {
            SelectedRecord = Record;
            bHasSelection = true;
            UpdateDetailView(Record);
            OnEvidenceSelected.Broadcast(Record);
            return true;
        }
    }
    return false;
}

bool UForensicLedgerWidget::GetSelectedEvidence(FEdrathEvidenceRecord& OutEvidence) const
{
    if (bHasSelection)
    {
        OutEvidence = SelectedRecord;
        return true;
    }
    return false;
}

int32 UForensicLedgerWidget::GetTwistCluesCount() const
{
    int32 Count = 0;
    for (const FEdrathEvidenceRecord& Record : CachedRecords)
    {
        if (Record.bIsForeshadowingTwistClue)
        {
            ++Count;
        }
    }
    return Count;
}

void UForensicLedgerWidget::UpdateDetailView(const FEdrathEvidenceRecord& Record)
{
    if (EvidenceTitleLabel)
    {
        EvidenceTitleLabel->SetText(FText::FromName(Record.EvidenceTag.GetTagName()));
    }

    if (EvidenceDescriptionLabel)
    {
        EvidenceDescriptionLabel->SetText(Record.CurrentDescription);
    }

    if (MutationVersionLabel)
    {
        const FText VerText = FText::Format(
            NSLOCTEXT("EdrathUI", "EvidenceMutationFmt", "Timeline Mutation Revision: v{0}"),
            FText::AsNumber(Record.MutationVersion));
        MutationVersionLabel->SetText(VerText);
    }

    if (TwistWarningLabel)
    {
        if (Record.bIsForeshadowingTwistClue)
        {
            TwistWarningLabel->SetVisibility(ESlateVisibility::Visible);
            TwistWarningLabel->SetText(NSLOCTEXT("EdrathUI", "TwistWarningText", "[Ontological Contradiction Detected]"));
        }
        else
        {
            TwistWarningLabel->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}
