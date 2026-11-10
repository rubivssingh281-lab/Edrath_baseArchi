#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EdrathTouchControlsWidget.generated.h"

class UButton;
class UPanelWidget;
class UEdrathPlatformSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTouchActionTriggered);

/**
 * UEdrathTouchControlsWidget
 * Provides a mobile-first on-screen touch interface:
 * - Virtual action buttons for Attack, Dodge, Parry, Possibility Shift, and Echo Strike.
 * - Forensic Ledger and deduction board quick-access tabs.
 * - Automatically evaluates platform visibility via UEdrathPlatformSubsystem.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UEdrathTouchControlsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** Evaluates whether touch controls should be visible on the current platform */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Touch")
    void EvaluatePlatformVisibility();

    /** Explicitly enables or disables touch overlay visibility */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Touch")
    void SetTouchControlsActive(bool bActive);

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Touch")
    bool IsTouchControlsActive() const { return bIsTouchActive; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Touch")
    FOnTouchActionTriggered OnAttackButtonPressed;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Touch")
    FOnTouchActionTriggered OnDodgeButtonPressed;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Touch")
    FOnTouchActionTriggered OnParryButtonPressed;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Touch")
    FOnTouchActionTriggered OnShiftTimelinePressed;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Touch")
    FOnTouchActionTriggered OnEchoStrikePressed;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Touch")
    FOnTouchActionTriggered OnToggleLedgerPressed;

protected:
    UFUNCTION()
    void HandleAttackClicked();

    UFUNCTION()
    void HandleDodgeClicked();

    UFUNCTION()
    void HandleParryClicked();

    UFUNCTION()
    void HandleShiftTimelineClicked();

    UFUNCTION()
    void HandleEchoStrikeClicked();

    UFUNCTION()
    void HandleToggleLedgerClicked();

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Touch")
    bool bIsTouchActive = false;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UButton> AttackButton;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UButton> DodgeButton;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UButton> ParryButton;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UButton> ShiftTimelineButton;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UButton> EchoStrikeButton;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UButton> ToggleLedgerButton;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Touch")
    TObjectPtr<UPanelWidget> ActionButtonsContainer;
};
