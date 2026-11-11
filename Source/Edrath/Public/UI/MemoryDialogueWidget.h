#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MemoryDialogueWidget.generated.h"

class UTextBlock;
class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueChoiceSelected, int32, ChoiceIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueDismissed);

/**
 * UMemoryDialogueWidget handles subtitle and interaction choices for:
 * - The 114-year-dead priest memory dialogue in the crypt.
 * - Battlefield dying soldier interactions.
 * - Witness testimony contradictions and Black Clock memory choices.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UMemoryDialogueWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Dialogue")
    void DisplayDialogue(const FText& InSpeakerName, const FText& InDialogueText, const TArray<FText>& InChoices);

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Dialogue")
    bool ChooseOption(int32 ChoiceIndex);

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Dialogue")
    void Dismiss();

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Dialogue")
    FText GetCurrentSpeaker() const { return CurrentSpeaker; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Dialogue")
    FText GetCurrentDialogueText() const { return CurrentDialogueText; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Dialogue")
    const TArray<FText>& GetCurrentChoices() const { return CurrentChoices; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Dialogue")
    bool IsDialogueActive() const { return bIsActive; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Dialogue")
    FOnDialogueChoiceSelected OnChoiceSelected;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Dialogue")
    FOnDialogueDismissed OnDialogueClosed;

protected:
    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Dialogue")
    TObjectPtr<UTextBlock> SpeakerNameLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Dialogue")
    TObjectPtr<UTextBlock> DialogueBodyLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Dialogue")
    TObjectPtr<UPanelWidget> ChoicesContainer;

    /** Hook for Blueprint to instantiate custom Slate button widgets per choice */
    UFUNCTION(BlueprintImplementableEvent, Category="Edrath|UI|Dialogue")
    void OnPopulateChoiceButton(int32 ChoiceIndex, const FText& ChoiceText);

private:
    FText CurrentSpeaker;
    FText CurrentDialogueText;
    TArray<FText> CurrentChoices;
    bool bIsActive = false;
};
