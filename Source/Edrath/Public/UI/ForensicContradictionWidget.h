#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/EdrathTypes.h"
#include "ForensicContradictionWidget.generated.h"

class UTextBlock;
class UButton;
class UPanelWidget;
class UEdrathNarrativeSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContradictionResolvedEvent, const FContradictionResolutionResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnContradictionFailedEvent, FGameplayTag, ClueA, FGameplayTag, ClueB);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnContradictionBreakthroughVisualFX, FGameplayTag, BreakthroughTag, float, GlitchIntensity, float, PulseDuration);

/**
 * UForensicContradictionWidget
 * Provides an interactive investigation and deduction interface for cross-referencing
 * conflicting clues, witness statements, and physical artifacts to uncover narrative breakthroughs.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UForensicContradictionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** Registers standard default Act 1 and core twist contradiction deduction rules */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    void RegisterDefaultDeductionRules();

    /** Adds a custom deduction hypothesis rule linking two contradictory clues to a breakthrough */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    void AddDeductionRule(const FContradictionDeductionRule& InRule);

    /** Selects the primary evidence clue in slot A */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    bool SlotPrimaryClue(FGameplayTag ClueTag);

    /** Selects the conflicting evidence clue in slot B */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    bool SlotContradictoryClue(FGameplayTag ClueTag);

    /** Evaluates whether slotted clues A and B form a valid contradiction breakthrough */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    FContradictionResolutionResult AttemptResolveContradiction();

    /** Clears slotted clues and resets deduction hypothesis board */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    void ClearDeductionSlots();

    /** Clears only slot A */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    void ClearSlotA();

    /** Clears only slot B */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Deduction")
    void ClearSlotB();

    /** Checks whether a specific clue tag is currently in either slot */
    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    bool IsClueSlotted(FGameplayTag ClueTag) const { return SlottedClueA == ClueTag || SlottedClueB == ClueTag; }

    /** Retrieves all registered deduction hypothesis rules */
    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    const TArray<FContradictionDeductionRule>& GetDeductionRules() const { return DeductionRules; }

    /** Retrieves the number of successfully resolved contradictions */
    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    int32 GetResolvedContradictionCount() const { return ResolvedBreakthroughs.Num(); }

    /** Checks if a specific breakthrough has already been deduced */
    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    bool HasBreakthroughBeenResolved(FGameplayTag BreakthroughTag) const;

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    FGameplayTag GetPrimaryClueTag() const { return SlottedClueA; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    FGameplayTag GetContradictoryClueTag() const { return SlottedClueB; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Deduction")
    bool AreBothSlotsFilled() const { return SlottedClueA.IsValid() && SlottedClueB.IsValid(); }

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Deduction")
    FOnContradictionResolvedEvent OnContradictionResolved;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Deduction")
    FOnContradictionFailedEvent OnContradictionFailed;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Deduction")
    FOnContradictionBreakthroughVisualFX OnBreakthroughVisualFX;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UI|Deduction|VisualFX")
    float BreakthroughChromaticGlitchIntensity = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UI|Deduction|VisualFX")
    float BreakthroughPulseDurationSeconds = 1.2f;
    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Deduction")
    FGameplayTag SlottedClueA;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Deduction")
    FGameplayTag SlottedClueB;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Deduction")
    TArray<FContradictionDeductionRule> DeductionRules;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Deduction")
    TArray<FGameplayTag> ResolvedBreakthroughs;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Deduction")
    TObjectPtr<UTextBlock> SlotALabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Deduction")
    TObjectPtr<UTextBlock> SlotBLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Deduction")
    TObjectPtr<UTextBlock> ResolutionResultLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Deduction")
    TObjectPtr<UButton> ResolveButton;
};
