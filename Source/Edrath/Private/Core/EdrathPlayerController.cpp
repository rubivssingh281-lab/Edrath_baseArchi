#include "Core/EdrathPlayerController.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Character/KaelCharacter.h"
#include "Character/KaelInteractionScannerComponent.h"
#include "UI/EdrathHUD.h"
#include "UI/EdrathTouchControlsWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

AEdrathPlayerController::AEdrathPlayerController()
{
    bShowMouseCursor = false;
}

void AEdrathPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (AEdrathHUD* EdrathHUD = Cast<AEdrathHUD>(GetHUD()))
    {
        if (UEdrathTouchControlsWidget* TouchWidget = EdrathHUD->GetTouchControlsWidget())
        {
            BindTouchControlsWidget(TouchWidget);
        }
    }
}

void AEdrathPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindTouchControlsWidget();
    Super::EndPlay(EndPlayReason);
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
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEdrathPlayerController::HandleInputMove);
        }
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEdrathPlayerController::HandleInputLook);
        }
        if (ZoomAction)
        {
            EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AEdrathPlayerController::HandleInputZoom);
        }
        if (SnapYawLeftAction)
        {
            EnhancedInputComponent->BindAction(SnapYawLeftAction, ETriggerEvent::Started, this, &AEdrathPlayerController::SnapCameraYawLeft);
        }
        if (SnapYawRightAction)
        {
            EnhancedInputComponent->BindAction(SnapYawRightAction, ETriggerEvent::Started, this, &AEdrathPlayerController::SnapCameraYawRight);
        }
        if (PrimaryAttackAction)
        {
            EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &AEdrathPlayerController::HandlePrimaryAttack);
        }
        if (DodgeRollAction)
        {
            EnhancedInputComponent->BindAction(DodgeRollAction, ETriggerEvent::Started, this, &AEdrathPlayerController::HandleDodgeRoll);
        }
        if (ParryAction)
        {
            EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &AEdrathPlayerController::HandleParry);
        }
        if (EchoStrikeAction)
        {
            EnhancedInputComponent->BindAction(EchoStrikeAction, ETriggerEvent::Started, this, &AEdrathPlayerController::HandleEchoStrike);
        }
        if (PossibilityShiftAction)
        {
            EnhancedInputComponent->BindAction(PossibilityShiftAction, ETriggerEvent::Started, this, &AEdrathPlayerController::HandlePossibilityShift);
        }
        if (InteractAction)
        {
            EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AEdrathPlayerController::HandleInteractInputWrapper);
        }
        if (ToggleLedgerAction)
        {
            EnhancedInputComponent->BindAction(ToggleLedgerAction, ETriggerEvent::Started, this, &AEdrathPlayerController::ToggleForensicLedger);
        }
        if (ToggleContradictionAction)
        {
            EnhancedInputComponent->BindAction(ToggleContradictionAction, ETriggerEvent::Started, this, &AEdrathPlayerController::ToggleContradictionBoard);
        }
    }
}

AKaelCharacter* AEdrathPlayerController::GetControlledKael() const
{
    return Cast<AKaelCharacter>(GetPawn());
}

void AEdrathPlayerController::HandleInputMove(const FInputActionValue& Value)
{
    ProcessMoveInput(Value.Get<FVector2D>());
}

void AEdrathPlayerController::HandleInputLook(const FInputActionValue& Value)
{
    ProcessLookInput(Value.Get<FVector2D>());
}

void AEdrathPlayerController::HandleInputZoom(const FInputActionValue& Value)
{
    ProcessZoomInput(Value.Get<float>());
}

void AEdrathPlayerController::HandleInteractInputWrapper()
{
    HandleInteract();
}

void AEdrathPlayerController::ProcessMoveInput(const FVector2D& MovementVector)
{
    AKaelCharacter* Kael = GetControlledKael();
    if (!Kael || MovementVector.IsNearlyZero())
    {
        return;
    }

    float BasisYaw = GetControlRotation().Yaw;
    if (Kael->DualCameraComponent &&
        Kael->DualCameraComponent->GetCurrentCameraMode() == EEdrathCameraMode::ExteriorBirdsEye)
    {
        // In isometric exploration, align WASD planar directions to the orbiting exterior camera yaw
        BasisYaw = Kael->DualCameraComponent->GetExteriorYawDegrees();
    }

    const FRotator PlanarRotation(0.0f, BasisYaw, 0.0f);
    const FVector ForwardDir = FRotationMatrix(PlanarRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(PlanarRotation).GetUnitAxis(EAxis::Y);

    Kael->AddMovementInput(ForwardDir, MovementVector.Y);
    Kael->AddMovementInput(RightDir, MovementVector.X);
}

void AEdrathPlayerController::ProcessLookInput(const FVector2D& LookVector)
{
    const AKaelCharacter* Kael = GetControlledKael();
    if (Kael && Kael->DualCameraComponent &&
        Kael->DualCameraComponent->GetCurrentCameraMode() == EEdrathCameraMode::InteriorThirdPerson)
    {
        AddYawInput(LookVector.X);
        AddPitchInput(LookVector.Y);
    }
}

void AEdrathPlayerController::ProcessZoomInput(float AxisValue)
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        if (Kael->DualCameraComponent)
        {
            Kael->DualCameraComponent->AdjustExteriorZoom(AxisValue);
        }
    }
}

void AEdrathPlayerController::SnapCameraYawLeft()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        if (Kael->DualCameraComponent)
        {
            Kael->DualCameraComponent->RotateExteriorYaw(-CameraYawSnapStepDegrees);
        }
    }
}

void AEdrathPlayerController::SnapCameraYawRight()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        if (Kael->DualCameraComponent)
        {
            Kael->DualCameraComponent->RotateExteriorYaw(CameraYawSnapStepDegrees);
        }
    }
}

void AEdrathPlayerController::HandlePrimaryAttack()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        Kael->PerformPrimaryAttack();
    }
}

void AEdrathPlayerController::HandleDodgeRoll()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        Kael->PerformDodge();
    }
}

void AEdrathPlayerController::HandleParry()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        Kael->PerformParry();
    }
}

void AEdrathPlayerController::HandleEchoStrike()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        Kael->PerformEchoStrike(DefaultEchoStrikeLookbackSeconds);
    }
}

void AEdrathPlayerController::HandlePossibilityShift()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        Kael->ActivateBlackClockShift();
    }
}

bool AEdrathPlayerController::HandleInteract()
{
    if (AKaelCharacter* Kael = GetControlledKael())
    {
        if (Kael->InteractionScannerComponent)
        {
            return Kael->InteractionScannerComponent->TryInteractWithFocusedTarget();
        }
    }
    return false;
}

void AEdrathPlayerController::ToggleForensicLedger()
{
    if (AEdrathHUD* EdrathHUD = Cast<AEdrathHUD>(GetHUD()))
    {
        EdrathHUD->ToggleForensicLedger();
    }
}

void AEdrathPlayerController::ToggleContradictionBoard()
{
    if (AEdrathHUD* EdrathHUD = Cast<AEdrathHUD>(GetHUD()))
    {
        EdrathHUD->ToggleContradictionBoard();
    }
}

void AEdrathPlayerController::BindTouchControlsWidget(UEdrathTouchControlsWidget* TouchWidget)
{
    if (!TouchWidget)
    {
        return;
    }

    UnbindTouchControlsWidget();
    BoundTouchControls = TouchWidget;

    TouchWidget->OnAttackButtonPressed.AddUniqueDynamic(this, &AEdrathPlayerController::HandlePrimaryAttack);
    TouchWidget->OnDodgeButtonPressed.AddUniqueDynamic(this, &AEdrathPlayerController::HandleDodgeRoll);
    TouchWidget->OnParryButtonPressed.AddUniqueDynamic(this, &AEdrathPlayerController::HandleParry);
    TouchWidget->OnShiftTimelinePressed.AddUniqueDynamic(this, &AEdrathPlayerController::HandlePossibilityShift);
    TouchWidget->OnEchoStrikePressed.AddUniqueDynamic(this, &AEdrathPlayerController::HandleEchoStrike);
    TouchWidget->OnToggleLedgerPressed.AddUniqueDynamic(this, &AEdrathPlayerController::ToggleForensicLedger);
}

void AEdrathPlayerController::UnbindTouchControlsWidget()
{
    if (BoundTouchControls.IsValid())
    {
        UEdrathTouchControlsWidget* TouchWidget = BoundTouchControls.Get();
        TouchWidget->OnAttackButtonPressed.RemoveDynamic(this, &AEdrathPlayerController::HandlePrimaryAttack);
        TouchWidget->OnDodgeButtonPressed.RemoveDynamic(this, &AEdrathPlayerController::HandleDodgeRoll);
        TouchWidget->OnParryButtonPressed.RemoveDynamic(this, &AEdrathPlayerController::HandleParry);
        TouchWidget->OnShiftTimelinePressed.RemoveDynamic(this, &AEdrathPlayerController::HandlePossibilityShift);
        TouchWidget->OnEchoStrikePressed.RemoveDynamic(this, &AEdrathPlayerController::HandleEchoStrike);
        TouchWidget->OnToggleLedgerPressed.RemoveDynamic(this, &AEdrathPlayerController::ToggleForensicLedger);
        BoundTouchControls.Reset();
    }
}
