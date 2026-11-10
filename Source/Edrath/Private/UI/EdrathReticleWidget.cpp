#include "UI/EdrathReticleWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

void UEdrathReticleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentMode = EEdrathReticleMode::IsometricCursor;
    bHasInspectionPrompt = false;
    bCanInteract = false;
    bIsForensicFocused = false;
    bTelegraphVisible = false;
    TelegraphRadius = 0.0f;
    CurrentSpread = 0.0f;

    ClearInspectionPrompt();
}

void UEdrathReticleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (CurrentSpread > 0.0f)
    {
        CurrentSpread = FMath::FInterpTo(CurrentSpread, 0.0f, InDeltaTime, 8.0f);
    }
}

void UEdrathReticleWidget::SetReticleMode(EEdrathReticleMode NewMode)
{
    if (CurrentMode == NewMode)
    {
        return;
    }

    CurrentMode = NewMode;

    switch (CurrentMode)
    {
    case EEdrathReticleMode::IsometricCursor:
        if (CrosshairCenterDot)
        {
            CrosshairCenterDot->SetVisibility(ESlateVisibility::Collapsed);
        }
        if (ForensicFocusRing)
        {
            ForensicFocusRing->SetVisibility(ESlateVisibility::Collapsed);
        }
        break;

    case EEdrathReticleMode::ThirdPersonCrosshair:
        if (CrosshairCenterDot)
        {
            CrosshairCenterDot->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        if (ForensicFocusRing)
        {
            ForensicFocusRing->SetVisibility(ESlateVisibility::Collapsed);
        }
        break;

    case EEdrathReticleMode::ForensicInspectFocus:
        if (CrosshairCenterDot)
        {
            CrosshairCenterDot->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        if (ForensicFocusRing)
        {
            ForensicFocusRing->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        break;

    case EEdrathReticleMode::Hidden:
        if (CrosshairCenterDot)
        {
            CrosshairCenterDot->SetVisibility(ESlateVisibility::Collapsed);
        }
        if (ForensicFocusRing)
        {
            ForensicFocusRing->SetVisibility(ESlateVisibility::Collapsed);
        }
        ClearInspectionPrompt();
        break;
    }

    OnReticleModeChanged.Broadcast(CurrentMode);
}

void UEdrathReticleWidget::SetCameraPerspectiveMode(EEdrathCameraMode NewCameraMode)
{
    if (NewCameraMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        SetReticleMode(EEdrathReticleMode::IsometricCursor);
    }
    else if (NewCameraMode == EEdrathCameraMode::InteriorThirdPerson)
    {
        if (bIsForensicFocused)
        {
            SetReticleMode(EEdrathReticleMode::ForensicInspectFocus);
        }
        else
        {
            SetReticleMode(EEdrathReticleMode::ThirdPersonCrosshair);
        }
    }
}

void UEdrathReticleWidget::UpdateReticlePosition(const FVector2D& InScreenPosition, const FVector& InWorldLocation)
{
    TargetScreenPosition = InScreenPosition;
    TargetWorldLocation = InWorldLocation;
}

void UEdrathReticleWidget::SetInspectionPrompt(const FText& InActionPrompt, const FText& InTargetName, bool bInteractable)
{
    bHasInspectionPrompt = true;
    bCanInteract = bInteractable;
    CurrentActionPrompt = InActionPrompt;
    CurrentTargetName = InTargetName;

    if (ActionPromptLabel)
    {
        ActionPromptLabel->SetText(CurrentActionPrompt);
    }

    if (TargetNameLabel)
    {
        TargetNameLabel->SetText(CurrentTargetName);
    }

    if (PromptContainer)
    {
        PromptContainer->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UEdrathReticleWidget::ClearInspectionPrompt()
{
    bHasInspectionPrompt = false;
    bCanInteract = false;
    CurrentActionPrompt = FText::GetEmpty();
    CurrentTargetName = FText::GetEmpty();

    if (PromptContainer)
    {
        PromptContainer->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UEdrathReticleWidget::SetIsometricTelegraph(float InRadius, bool bVisible)
{
    TelegraphRadius = FMath::Max(0.0f, InRadius);
    bTelegraphVisible = bVisible;
}

void UEdrathReticleWidget::SetForensicFocus(bool bInFocus, const FText& ClueDescription)
{
    bIsForensicFocused = bInFocus;
    FocusedClueDescription = ClueDescription;

    if (CurrentMode != EEdrathReticleMode::Hidden && CurrentMode != EEdrathReticleMode::IsometricCursor)
    {
        SetReticleMode(bInFocus ? EEdrathReticleMode::ForensicInspectFocus : EEdrathReticleMode::ThirdPersonCrosshair);
    }

    OnInspectionFocusChanged.Broadcast(bIsForensicFocused, FocusedClueDescription);
}

void UEdrathReticleWidget::SetReticleSpread(float InSpread)
{
    CurrentSpread = FMath::Clamp(InSpread, 0.0f, 100.0f);
}
