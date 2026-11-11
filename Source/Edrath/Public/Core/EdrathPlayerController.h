#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EdrathPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * AEdrathPlayerController handles player input routing, HUD interaction toggles (Forensic Ledger, Black Clock focus),
 * and camera perspective control inputs for Kael Veyr.
 */
UCLASS()
class EDRATH_API AEdrathPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AEdrathPlayerController();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input")
    void ToggleForensicLedger();

protected:
    virtual void SetupInputComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> ToggleLedgerAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Contexts")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
};
