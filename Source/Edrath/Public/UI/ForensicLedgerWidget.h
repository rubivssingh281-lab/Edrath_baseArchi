#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/EdrathTypes.h"
#include "ForensicLedgerWidget.generated.h"

class UTextBlock;
class UPanelWidget;
class UEdrathNarrativeSubsystem;

UENUM(BlueprintType)
enum class EForensicLedgerFilter : uint8
{
    AllClues,
    TwistCluesOnly,
    MutatedCluesOnly
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnForensicEvidenceSelected, const FEdrathEvidenceRecord&, SelectedEvidence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnForensicLedgerRefreshed, int32, TotalCluesCount);

/**
 * UForensicLedgerWidget displays Kael's narrative evidence ledger:
 * - Forensic examination of handwriting, portraits, and corpse artifacts.
 * - Tracks evidence mutation versions when memories or timelines change.
 * - Highlights foreshadowing clues linked to the 6 core narrative twists.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UForensicLedgerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|ForensicLedger")
    void RefreshEvidenceList();

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|ForensicLedger")
    void SetFilterMode(EForensicLedgerFilter NewFilter);

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|ForensicLedger")
    bool SelectEvidenceByTag(FGameplayTag EvidenceTag);

    UFUNCTION(BlueprintPure, Category="Edrath|UI|ForensicLedger")
    bool GetSelectedEvidence(FEdrathEvidenceRecord& OutEvidence) const;

    UFUNCTION(BlueprintPure, Category="Edrath|UI|ForensicLedger")
    const TArray<FEdrathEvidenceRecord>& GetFilteredEvidenceList() const { return FilteredRecords; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|ForensicLedger")
    int32 GetTotalCluesCount() const { return CachedRecords.Num(); }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|ForensicLedger")
    int32 GetTwistCluesCount() const;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|ForensicLedger")
    FOnForensicEvidenceSelected OnEvidenceSelected;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|ForensicLedger")
    FOnForensicLedgerRefreshed OnLedgerRefreshed;

protected:
    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|ForensicLedger")
    TObjectPtr<UTextBlock> EvidenceTitleLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|ForensicLedger")
    TObjectPtr<UTextBlock> EvidenceDescriptionLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|ForensicLedger")
    TObjectPtr<UTextBlock> MutationVersionLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|ForensicLedger")
    TObjectPtr<UTextBlock> TwistWarningLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|ForensicLedger")
    TObjectPtr<UPanelWidget> EvidenceListContainer;

    /** Hook for Blueprint Slate item creation when refreshed */
    UFUNCTION(BlueprintImplementableEvent, Category="Edrath|UI|ForensicLedger")
    void OnPopulateEvidenceItem(const FEdrathEvidenceRecord& Record);

private:
    void ApplyFilter();
    void UpdateDetailView(const FEdrathEvidenceRecord& Record);

    EForensicLedgerFilter CurrentFilter = EForensicLedgerFilter::AllClues;
    TArray<FEdrathEvidenceRecord> CachedRecords;
    TArray<FEdrathEvidenceRecord> FilteredRecords;
    FEdrathEvidenceRecord SelectedRecord;
    bool bHasSelection = false;
};
