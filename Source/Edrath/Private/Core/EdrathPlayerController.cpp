#include "Core/EdrathPlayerController.h"
#include "UI/EdrathHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AEdrathPlayerController::AEdrathPlayerController()
{
    bShowMouseCursor = false;
}

void AEdrathPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (ToggleLedgerAction)
        {
            EnhancedInputComponent->BindAction(ToggleLedgerAction, ETriggerEvent::Started, this, &AEdrathPlayerController::ToggleForensicLedger);
        }
    }
}

void AEdrathPlayerController::ToggleForensicLedger()
{
    if (AEdrathHUD* EdrathHUD = Cast<AEdrathHUD>(GetHUD()))
    {
        EdrathHUD->ToggleForensicLedger();
    }
}
