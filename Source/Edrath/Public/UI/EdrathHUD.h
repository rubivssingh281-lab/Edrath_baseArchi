#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EdrathHUD.generated.h"

class UBlackClockWidget;
class UForensicLedgerWidget;
class UMemoryDialogueWidget;

/**
 * AEdrathHUD manages the viewport UI elements for Edrath:
 * - Spawns and manages the persistent Black Clock status widget.
 * - Toggles the Forensic Ledger inspection panel.
 * - Handles Memory Echo conversation and investigation overlays.
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
    void OpenMemoryDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& Choices);

    UFUNCTION(BlueprintCallable, Category="Edrath|HUD")
    void CloseMemoryDialogue();

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    bool IsForensicLedgerOpen() const { return bIsLedgerOpen; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UBlackClockWidget* GetBlackClockWidget() const { return BlackClockWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UForensicLedgerWidget* GetForensicLedgerWidget() const { return ForensicLedgerWidget; }

    UFUNCTION(BlueprintPure, Category="Edrath|HUD")
    UMemoryDialogueWidget* GetMemoryDialogueWidget() const { return MemoryDialogueWidget; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UBlackClockWidget> BlackClockWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UForensicLedgerWidget> ForensicLedgerWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|HUD|Classes")
    TSubclassOf<UMemoryDialogueWidget> MemoryDialogueWidgetClass;

protected:
    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UBlackClockWidget> BlackClockWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UForensicLedgerWidget> ForensicLedgerWidget;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|HUD")
    TObjectPtr<UMemoryDialogueWidget> MemoryDialogueWidget;

private:
    bool bIsLedgerOpen = false;
};
