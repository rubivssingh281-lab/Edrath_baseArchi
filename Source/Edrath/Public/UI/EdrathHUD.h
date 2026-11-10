#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/EdrathTypes.h"
#include "EdrathHUD.generated.h"

class UBlackClockWidget;
class UForensicLedgerWidget;
class UMemoryDialogueWidget;
class UEdrathReticleWidget;
class UEchoCombatBufferWidget;
class UForensicContradictionWidget;
class UEdrathTouchControlsWidget;

/**
 * AEdrathHUD manages the viewport UI elements for Edrath:
 * - Spawns and manages the persistent Black Clock status widget.
 * - Manages the dynamic Dual-Perspective Reticle & Inspection Prompt HUD.
 * - Displays the Echo Combat 120-frame circular buffer and action history.
 * - Toggles the Forensic Ledger inspection panel and Contradiction Board.
 * - Handles Memory Echo conversation and investigation overlays.
 * - Hosts mobile/touch virtual control overlays.
 */
UCLASS()
class EDRATH_API AEdrathHUD : public AHUD
{
    GENERATED_BODY()

public:
    AEdrathHUD();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void ToggleForensicLedger();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void OpenForensicLedger();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void CloseForensicLedger();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void ToggleContradictionBoard();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void OpenContradictionBoard();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void CloseContradictionBoard();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void OpenMemoryDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& Choices);

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void CloseMemoryDialogue();

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void SetReticlePerspectiveMode(EEdrathCameraMode NewCameraMode);

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    bool IsForensicLedgerOpen() const { return bIsLedgerOpen; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    bool IsContradictionBoardOpen() const { return bIsContradictionBoardOpen; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UBlackClockWidget* GetBlackClockWidget() const { return BlackClockWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UForensicLedgerWidget* GetForensicLedgerWidget() const { return ForensicLedgerWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UMemoryDialogueWidget* GetMemoryDialogueWidget() const { return MemoryDialogueWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UEdrathReticleWidget* GetReticleWidget() const { return ReticleWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UEchoCombatBufferWidget* GetEchoBufferWidget() const { return EchoBufferWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UForensicContradictionWidget* GetContradictionWidget() const { return ContradictionWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UEdrathTouchControlsWidget* GetTouchControlsWidget() const { return TouchControlsWidget; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UBlackClockWidget> BlackClockWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UForensicLedgerWidget> ForensicLedgerWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UMemoryDialogueWidget> MemoryDialogueWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UEdrathReticleWidget> ReticleWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UEchoCombatBufferWidget> EchoBufferWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UForensicContradictionWidget> ContradictionWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UEdrathTouchControlsWidget> TouchControlsWidgetClass;

protected:
    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UBlackClockWidget> BlackClockWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UForensicLedgerWidget> ForensicLedgerWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UMemoryDialogueWidget> MemoryDialogueWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UEdrathReticleWidget> ReticleWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UEchoCombatBufferWidget> EchoBufferWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UForensicContradictionWidget> ContradictionWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UEdrathTouchControlsWidget> TouchControlsWidget;

private:
    bool bIsLedgerOpen = false;
    bool bIsContradictionBoardOpen = false;
};
