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
        AttackButton->OnClicked.AddDynamic(this, &UEdrathTouchControlsWidget::HandleAttackClicked);
    }
    if (DodgeButton)
    {
        DodgeButton->OnClicked.AddDynamic(this, &UEdrathTouchControlsWidget::HandleDodgeClicked);
    }
    if (ParryButton)
    {
        ParryButton->OnClicked.AddDynamic(this, &UEdrathTouchControlsWidget::HandleParryClicked);
    }
    if (ShiftTimelineButton)
    {
        ShiftTimelineButton->OnClicked.AddDynamic(this, &UEdrathTouchControlsWidget::HandleShiftTimelineClicked);
    }
    if (EchoStrikeButton)
    {
        EchoStrikeButton->OnClicked.AddDynamic(this, &UEdrathTouchControlsWidget::HandleEchoStrikeClicked);
    }
    if (ToggleLedgerButton)
    {
        ToggleLedgerButton->OnClicked.AddDynamic(this, &UEdrathTouchControlsWidget::HandleToggleLedgerClicked);
    }

    EvaluatePlatformVisibility();
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
