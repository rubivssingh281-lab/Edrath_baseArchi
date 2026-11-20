#include "UI/EdrathHUD.h"
#include "UI/BlackClockWidget.h"
#include "UI/ForensicLedgerWidget.h"
#include "UI/MemoryDialogueWidget.h"
#include "UI/EdrathReticleWidget.h"
#include "UI/EchoCombatBufferWidget.h"
#include "UI/ForensicContradictionWidget.h"
#include "UI/EdrathTouchControlsWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

AEdrathHUD::AEdrathHUD()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEdrathHUD::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    // 1. Persistent Black Clock Status
    if (BlackClockWidgetClass)
    {
        BlackClockWidget = CreateWidget<UBlackClockWidget>(PC, BlackClockWidgetClass);
        if (BlackClockWidget)
        {
            BlackClockWidget->AddToViewport(10);
        }
    }

    // 2. Dual-Perspective Reticle & Inspection Prompt
    if (ReticleWidgetClass)
    {
        ReticleWidget = CreateWidget<UEdrathReticleWidget>(PC, ReticleWidgetClass);
        if (ReticleWidget)
        {
            ReticleWidget->AddToViewport(5);
        }
    }

    // 3. Echo Combat Action & 120-Frame Timeline Buffer
    if (EchoBufferWidgetClass)
    {
        EchoBufferWidget = CreateWidget<UEchoCombatBufferWidget>(PC, EchoBufferWidgetClass);
        if (EchoBufferWidget)
        {
            EchoBufferWidget->AddToViewport(12);
        }
    }

    // 4. Mobile / Touch Control Overlay
    if (TouchControlsWidgetClass)
    {
        TouchControlsWidget = CreateWidget<UEdrathTouchControlsWidget>(PC, TouchControlsWidgetClass);
        if (TouchControlsWidget)
        {
            TouchControlsWidget->AddToViewport(15);
        }
    }

    // 5. Forensic Ledger Inspection Panel
    if (ForensicLedgerWidgetClass)
    {
        ForensicLedgerWidget = CreateWidget<UForensicLedgerWidget>(PC, ForensicLedgerWidgetClass);
        if (ForensicLedgerWidget)
        {
            ForensicLedgerWidget->AddToViewport(20);
            ForensicLedgerWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    // 6. Forensic Contradiction Deduction Board
    if (ContradictionWidgetClass)
    {
        ContradictionWidget = CreateWidget<UForensicContradictionWidget>(PC, ContradictionWidgetClass);
        if (ContradictionWidget)
        {
            ContradictionWidget->AddToViewport(25);
            ContradictionWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    // 7. Memory Echo Dialogue & Investigation
    if (MemoryDialogueWidgetClass)
    {
        MemoryDialogueWidget = CreateWidget<UMemoryDialogueWidget>(PC, MemoryDialogueWidgetClass);
        if (MemoryDialogueWidget)
        {
            MemoryDialogueWidget->AddToViewport(30);
            MemoryDialogueWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void AEdrathHUD::ToggleForensicLedger()
{
    if (bIsLedgerOpen)
    {
        CloseForensicLedger();
    }
    else
    {
        OpenForensicLedger();
    }
}

void AEdrathHUD::OpenForensicLedger()
{
    if (!ForensicLedgerWidget)
    {
        return;
    }

    ForensicLedgerWidget->RefreshEvidenceList();
    ForensicLedgerWidget->SetVisibility(ESlateVisibility::Visible);
    bIsLedgerOpen = true;

    if (APlayerController* PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(ForensicLedgerWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void AEdrathHUD::CloseForensicLedger()
{
    if (!ForensicLedgerWidget)
    {
        return;
    }

    ForensicLedgerWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsLedgerOpen = false;

    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (!bIsContradictionBoardOpen)
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
    }
}

void AEdrathHUD::ToggleContradictionBoard()
{
    if (bIsContradictionBoardOpen)
    {
        CloseContradictionBoard();
    }
    else
    {
        OpenContradictionBoard();
    }
}

void AEdrathHUD::OpenContradictionBoard()
{
    if (!ContradictionWidget)
    {
        return;
    }

    ContradictionWidget->SetVisibility(ESlateVisibility::Visible);
    bIsContradictionBoardOpen = true;

    if (APlayerController* PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(ContradictionWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void AEdrathHUD::CloseContradictionBoard()
{
    if (!ContradictionWidget)
    {
        return;
    }

    ContradictionWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsContradictionBoardOpen = false;

    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (!bIsLedgerOpen)
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
    }
}

void AEdrathHUD::OpenMemoryDialogue(
    const FText& SpeakerName,
    const FText& DialogueText,
    const TArray<FText>& Choices)
{
    if (!MemoryDialogueWidget)
    {
        return;
    }

    MemoryDialogueWidget->DisplayDialogue(SpeakerName, DialogueText, Choices);

    if (APlayerController* PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(MemoryDialogueWidget->TakeWidget());
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void AEdrathHUD::CloseMemoryDialogue()
{
    if (!MemoryDialogueWidget)
    {
        return;
    }

    MemoryDialogueWidget->Dismiss();

    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (!bIsLedgerOpen && !bIsContradictionBoardOpen)
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
    }
}

void AEdrathHUD::SetReticlePerspectiveMode(EEdrathCameraMode NewCameraMode)
{
    if (ReticleWidget)
    {
        ReticleWidget->SetCameraPerspectiveMode(NewCameraMode);
    }
}
