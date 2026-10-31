#include "Character/KaelCharacter.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/EchoCombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

AKaelCharacter::AKaelCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    SternumBlackClockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SternumBlackClockMesh"));
    SternumBlackClockMesh->SetupAttachment(GetMesh(), TEXT("bone_chest_clock"));
    DualCameraComponent = CreateDefaultSubobject<UEdrathDualCameraComponent>(TEXT("DualCameraComponent"));
    BlackClockComponent = CreateDefaultSubobject<UBlackClockComponent>(TEXT("BlackClockComponent"));
    EchoCombatComponent = CreateDefaultSubobject<UEdrathEchoCombatComponent>(TEXT("EchoCombatComponent"));
}

void AKaelCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (DualCameraComponent)
    {
        DualCameraComponent->InitializeCameraRig(CameraBoom, FollowCamera, this);
    }
}

void AKaelCharacter::ApplyDamage(float DamageAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(0.0f, DamageAmount), 0.0f, MaxHealth);
}

void AKaelCharacter::RestoreHealth(float HealAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth + FMath::Max(0.0f, HealAmount), 0.0f, MaxHealth);
}

bool AKaelCharacter::ConsumeStamina(float StaminaAmount)
{
    if (CurrentStamina >= StaminaAmount)
    {
        CurrentStamina -= StaminaAmount;
        return true;
    }
    return false;
}

void AKaelCharacter::PerformPrimaryAttack()
{
    if (EchoCombatComponent)
    {
        EchoCombatComponent->ExecuteAttackCombo(1, 35.0f);
    }
    else if (BlackClockComponent)
    {
        BlackClockComponent->RecordCombatFrame(TEXT("Slash_Combo_01"), 35.0f);
    }
}

bool AKaelCharacter::PerformDodge()
{
    return EchoCombatComponent && EchoCombatComponent->ExecuteDodgeRoll(0.45f);
}

bool AKaelCharacter::PerformParry()
{
    return EchoCombatComponent && EchoCombatComponent->ExecuteParry(0.25f);
}

bool AKaelCharacter::PerformEchoStrike(float SecondsAgo)
{
    if (!BlackClockComponent)
    {
        return false;
    }

    FEchoActionFrame EchoFrame;
    if (BlackClockComponent->TriggerEchoStrike(SecondsAgo, CurrentRegion, EchoFrame))
    {
        if (EchoCombatComponent)
        {
            EchoCombatComponent->SpawnVisualGhostStrike(EchoFrame);
        }
        return true;
    }
    return false;
}

bool AKaelCharacter::ActivateBlackClockShift()
{
    return BlackClockComponent && BlackClockComponent->TogglePossibilityShift(CurrentRegion);
}
