#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "KaelCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UEdrathDualCameraComponent;
class UBlackClockComponent;
class UEdrathEchoCombatComponent;

UCLASS()
class EDRATH_API AKaelCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AKaelCharacter();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael")
    TObjectPtr<UStaticMeshComponent> SternumBlackClockMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael")
    TObjectPtr<UEdrathDualCameraComponent> DualCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael")
    TObjectPtr<UBlackClockComponent> BlackClockComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael")
    TObjectPtr<UEdrathEchoCombatComponent> EchoCombatComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Kael")
    EEdrathRegion CurrentRegion = EEdrathRegion::Veyl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Kael")
    EKaelLifeEchoArchetype EquippedLifeEchoArchetype = EKaelLifeEchoArchetype::ForgottenSoldier_Act1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Kael|Attributes")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael|Attributes")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Kael|Attributes")
    float MaxStamina = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Kael|Attributes")
    float CurrentStamina = 100.0f;

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael|Attributes")
    void ApplyDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael|Attributes")
    void RestoreHealth(float HealAmount);

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael|Attributes")
    bool ConsumeStamina(float StaminaAmount);

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael")
    void PerformPrimaryAttack();

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael|Combat")
    bool PerformDodge();

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael|Combat")
    bool PerformParry();

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael|Combat")
    bool PerformEchoStrike(float SecondsAgo = 3.0f);

    UFUNCTION(BlueprintCallable, Category="Edrath|Kael")
    bool ActivateBlackClockShift();
};
