#include "UI/MemoryDialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UMemoryDialogueWidget::DisplayDialogue(
    const FText& InSpeakerName,
    const FText& InDialogueText,
    const TArray<FText>& InChoices)
{
    CurrentSpeaker = InSpeakerName;
    CurrentDialogueText = InDialogueText;
    CurrentChoices = InChoices;
    bIsActive = true;

    if (SpeakerNameLabel)
    {
        SpeakerNameLabel->SetText(InSpeakerName);
    }

    if (DialogueBodyLabel)
    {
        DialogueBodyLabel->SetText(InDialogueText);
    }

    if (ChoicesContainer)
    {
        ChoicesContainer->ClearChildren();
        for (int32 i = 0; i < InChoices.Num(); ++i)
        {
            OnPopulateChoiceButton(i, InChoices[i]);
        }
    }

    SetVisibility(ESlateVisibility::Visible);
}

bool UMemoryDialogueWidget::ChooseOption(int32 ChoiceIndex)
{
    if (!bIsActive || !CurrentChoices.IsValidIndex(ChoiceIndex))
    {
        return false;
    }

    OnChoiceSelected.Broadcast(ChoiceIndex);
    return true;
}

void UMemoryDialogueWidget::Dismiss()
{
    bIsActive = false;
    CurrentSpeaker = FText::GetEmpty();
    CurrentDialogueText = FText::GetEmpty();
    CurrentChoices.Empty();

    SetVisibility(ESlateVisibility::Collapsed);
    OnDialogueClosed.Broadcast();
}
