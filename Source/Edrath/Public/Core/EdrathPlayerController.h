#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EdrathPlayerController.generated.h"

class AKaelCharacter;
class UEdrathTouchControlsWidget;
class UInputAction;
class UInputMappingContext;

/**
 * AEdrathPlayerController
 * Governs all desktop/gamepad Enhanced Input and mobile touch control bindings for Kael Veyr:
 * - Perspective-aware planar movement (45-deg isometric yaw vs. free third-person OTS)
 * - Exterior zoom & 45-degree camera snap rotation
 * - Sword slash, dodge roll, chronal parry, Echo Strike, Possibility Shift, and [E] Interaction
 * - Forensic Ledger & Contradiction Board HUD overlays
 */
UCLASS()
class EDRATH_API AEdrathPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AEdrathPlayerController();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Applies perspective-normalized planar movement from keyboard/stick axes. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Movement")
    void ProcessMoveInput(const FVector2D& MovementVector);

    /** Applies free-look camera rotation when in InteriorThirdPerson mode. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Camera")
    void ProcessLookInput(const FVector2D& LookVector);

    /** Adjusts ExteriorBirdsEye arm length via scroll wheel or triggers. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Camera")
    void ProcessZoomInput(float AxisValue);

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Camera")
    void SnapCameraYawLeft();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Camera")
    void SnapCameraYawRight();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Combat")
    void HandlePrimaryAttack();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Combat")
    void HandleDodgeRoll();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Combat")
    void HandleParry();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|BlackClock")
    void HandleEchoStrike();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|BlackClock")
    void HandlePossibilityShift();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Interaction")
    bool HandleInteract();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|UI")
    void ToggleForensicLedger();

    UFUNCTION(BlueprintCallable, Category="Edrath|Input|UI")
    void ToggleContradictionBoard();

    /** Connects virtual mobile touch overlay delegates cleanly to controller action handlers. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Touch")
    void BindTouchControlsWidget(UEdrathTouchControlsWidget* TouchWidget);

    /** Unbinds virtual mobile touch overlay delegates safely. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Input|Touch")
    void UnbindTouchControlsWidget();

protected:
    virtual void SetupInputComponent() override;

    void HandleInputMove(const FInputActionValue& Value);
    void HandleInputLook(const FInputActionValue& Value);
    void HandleInputZoom(const FInputActionValue& Value);

    AKaelCharacter* GetControlledKael() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Contexts")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> ZoomAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> SnapYawLeftAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> SnapYawRightAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> PrimaryAttackAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> DodgeRollAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> ParryAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> EchoStrikeAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> PossibilityShiftAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> ToggleLedgerAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Actions")
    TObjectPtr<UInputAction> ToggleContradictionAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|BlackClock")
    float DefaultEchoStrikeLookbackSeconds = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Input|Camera")
    float CameraYawSnapStepDegrees = 45.0f;

private:
    void HandleInteractInputWrapper();

    TWeakObjectPtr<UEdrathTouchControlsWidget> BoundTouchControls;
};
