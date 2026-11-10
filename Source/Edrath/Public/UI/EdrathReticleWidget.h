#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/EdrathTypes.h"
#include "EdrathReticleWidget.generated.h"

class UTextBlock;
class UImage;
class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReticleModeChanged, EEdrathReticleMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInspectionFocusChanged, bool, bIsFocused, const FText&, ClueName);

/**
 * UEdrathReticleWidget
 * Manages dual-perspective targeting and inspection HUD reticles:
 * - Isometric mode: Dynamic ground targeting circle, range telegraphing, and cursor projection.
 * - Third-person mode: Over-the-shoulder crosshair, forensic focus detection, and inspection prompts.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UEdrathReticleWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** Updates the active reticle mode (Isometric, ThirdPerson, ForensicFocus, Hidden) */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void SetReticleMode(EEdrathReticleMode NewMode);

    /** Adapts reticle configuration automatically based on the camera perspective mode */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void SetCameraPerspectiveMode(EEdrathCameraMode NewCameraMode);

    /** Sets the current screen position and 3D world hit point of the targeting marker */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void UpdateReticlePosition(const FVector2D& InScreenPosition, const FVector& InWorldLocation);

    /** Displays an interactable contextual prompt (e.g., "[E] Inspect Handwriting", "[E] Examine Priest Corpse") */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void SetInspectionPrompt(const FText& InActionPrompt, const FText& InTargetName, bool bInteractable = true);

    /** Clears the current contextual inspection prompt */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void ClearInspectionPrompt();

    /** Configures isometric ability radius telegraphing */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void SetIsometricTelegraph(float InRadius, bool bVisible);

    /** Updates forensic focus when aiming at an evidence anchor in Third-Person mode */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void SetForensicFocus(bool bInFocus, const FText& ClueDescription);

    /** Adjusts reticle spread (used during combat attacks or sprint) */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|Reticle")
    void SetReticleSpread(float InSpread);

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    EEdrathReticleMode GetReticleMode() const { return CurrentMode; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    bool HasActiveInspectionPrompt() const { return bHasInspectionPrompt; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    FText GetInspectionActionText() const { return CurrentActionPrompt; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    FText GetInspectionTargetName() const { return CurrentTargetName; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    bool IsForensicFocusActive() const { return bIsForensicFocused; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    float GetIsometricTelegraphRadius() const { return TelegraphRadius; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    float GetReticleSpread() const { return CurrentSpread; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|Reticle")
    FVector2D GetReticleScreenPosition() const { return TargetScreenPosition; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Reticle")
    FOnReticleModeChanged OnReticleModeChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|Reticle")
    FOnInspectionFocusChanged OnInspectionFocusChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|UI|Reticle")
    EEdrathReticleMode CurrentMode = EEdrathReticleMode::IsometricCursor;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    FVector2D TargetScreenPosition = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    FVector TargetWorldLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    bool bHasInspectionPrompt = false;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    bool bCanInteract = false;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    FText CurrentActionPrompt;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    FText CurrentTargetName;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    bool bIsForensicFocused = false;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    FText FocusedClueDescription;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    bool bTelegraphVisible = false;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    float TelegraphRadius = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|Reticle")
    float CurrentSpread = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category="Edrath|UI|Reticle")
    float ReticleInterpSpeed = 15.0f;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Reticle")
    TObjectPtr<UTextBlock> ActionPromptLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Reticle")
    TObjectPtr<UTextBlock> TargetNameLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Reticle")
    TObjectPtr<UPanelWidget> PromptContainer;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Reticle")
    TObjectPtr<UImage> CrosshairCenterDot;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|Reticle")
    TObjectPtr<UImage> ForensicFocusRing;
};
