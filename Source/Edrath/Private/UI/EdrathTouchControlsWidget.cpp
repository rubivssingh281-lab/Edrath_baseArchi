#include "UI/EdrathTouchControlsWidget.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "EdrathPlatformSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UEdrathTouchControlsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (AttackButton)
    {
        AttackButton->OnClicked.AddUniqueDynamic(this, &UEdrathTouchControlsWidget::HandleAttackClicked);
    }
    if (DodgeButton)
    {
        DodgeButton->OnClicked.AddUniqueDynamic(this, &UEdrathTouchControlsWidget::HandleDodgeClicked);
    }
    if (ParryButton)
    {
        ParryButton->OnClicked.AddUniqueDynamic(this, &UEdrathTouchControlsWidget::HandleParryClicked);
    }
    if (ShiftTimelineButton)
    {
        ShiftTimelineButton->OnClicked.AddUniqueDynamic(this, &UEdrathTouchControlsWidget::HandleShiftTimelineClicked);
    }
    if (EchoStrikeButton)
    {
        EchoStrikeButton->OnClicked.AddUniqueDynamic(this, &UEdrathTouchControlsWidget::HandleEchoStrikeClicked);
    }
    if (ToggleLedgerButton)
    {
        ToggleLedgerButton->OnClicked.AddUniqueDynamic(this, &UEdrathTouchControlsWidget::HandleToggleLedgerClicked);
    }

    EvaluatePlatformVisibility();
}

void UEdrathTouchControlsWidget::NativeDestruct()
{
    if (AttackButton)
    {
        AttackButton->OnClicked.RemoveDynamic(this, &UEdrathTouchControlsWidget::HandleAttackClicked);
    }
    if (DodgeButton)
    {
        DodgeButton->OnClicked.RemoveDynamic(this, &UEdrathTouchControlsWidget::HandleDodgeClicked);
    }
    if (ParryButton)
    {
        ParryButton->OnClicked.RemoveDynamic(this, &UEdrathTouchControlsWidget::HandleParryClicked);
    }
    if (ShiftTimelineButton)
    {
        ShiftTimelineButton->OnClicked.RemoveDynamic(this, &UEdrathTouchControlsWidget::HandleShiftTimelineClicked);
    }
    if (EchoStrikeButton)
    {
        EchoStrikeButton->OnClicked.RemoveDynamic(this, &UEdrathTouchControlsWidget::HandleEchoStrikeClicked);
    }
    if (ToggleLedgerButton)
    {
        ToggleLedgerButton->OnClicked.RemoveDynamic(this, &UEdrathTouchControlsWidget::HandleToggleLedgerClicked);
    }

    Super::NativeDestruct();
}

void UEdrathTouchControlsWidget::EvaluatePlatformVisibility()
{
    bool bShouldEnable = false;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathPlatformSubsystem* PlatformSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathPlatformSubsystem>())
        {
            bShouldEnable = PlatformSys->IsMobilePlatform();
        }
    }

    SetTouchControlsActive(bShouldEnable);
}

void UEdrathTouchControlsWidget::SetTouchControlsActive(bool bActive)
{
    bIsTouchActive = bActive;

    SetVisibility(bIsTouchActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    if (ActionButtonsContainer)
    {
        ActionButtonsContainer->SetVisibility(bIsTouchActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UEdrathTouchControlsWidget::HandleAttackClicked()
{
    OnAttackButtonPressed.Broadcast();
}

void UEdrathTouchControlsWidget::HandleDodgeClicked()
{
    OnDodgeButtonPressed.Broadcast();
}

void UEdrathTouchControlsWidget::HandleParryClicked()
{
    OnParryButtonPressed.Broadcast();
}

void UEdrathTouchControlsWidget::HandleShiftTimelineClicked()
{
    OnShiftTimelinePressed.Broadcast();
}

void UEdrathTouchControlsWidget::HandleEchoStrikeClicked()
{
    OnEchoStrikePressed.Broadcast();
}

void UEdrathTouchControlsWidget::HandleToggleLedgerClicked()
{
    OnToggleLedgerPressed.Broadcast();
}
