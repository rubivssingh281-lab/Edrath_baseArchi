#include "Character/KaelCharacter.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/EchoCombatComponent.h"
#include "BlackClock/EdrathCombatFeedbackSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Character/KaelModularRigComponent.h"
#include "Character/KaelTerrainAdaptationComponent.h"
#include "Character/KaelInteractionScannerComponent.h"
#include "Core/EdrathAudioSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
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
    ModularRigComponent = CreateDefaultSubobject<UKaelModularRigComponent>(TEXT("ModularRigComponent"));
    TerrainAdaptationComponent = CreateDefaultSubobject<UKaelTerrainAdaptationComponent>(TEXT("TerrainAdaptationComponent"));
    InteractionScannerComponent = CreateDefaultSubobject<UKaelInteractionScannerComponent>(TEXT("InteractionScannerComponent"));
}

void AKaelCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (DualCameraComponent)
    {
        DualCameraComponent->InitializeCameraRig(CameraBoom, FollowCamera, this);
    }
    if (ModularRigComponent)
    {
        ModularRigComponent->InitializeModularRig(GetMesh());
    }
    if (TerrainAdaptationComponent)
    {
        TerrainAdaptationComponent->InitializeTerrainAdaptation(this, GetMesh());
    }
}

void AKaelCharacter::ApplyDamage(float DamageAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(0.0f, DamageAmount), 0.0f, MaxHealth);
}

bool AKaelCharacter::ApplyPoiseDamage(float PoiseDamageAmount, bool bFromEchoStrike)
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UEdrathCombatFeedbackSubsystem* Feedback = GI->GetSubsystem<UEdrathCombatFeedbackSubsystem>())
            {
                const FEdrathPoiseEvaluationResult Result = Feedback->EvaluatePoiseBreak(
                    CurrentPoise,
                    PoiseDamageAmount,
                    false,
                    bFromEchoStrike,
                    World->GetTimeSeconds());

                CurrentPoise = Result.RemainingPoise;
                bIsStaggered = Result.bPoiseBroken;
                return bIsStaggered;
            }
        }
    }

    CurrentPoise = FMath::Clamp(CurrentPoise - FMath::Max(0.0f, PoiseDamageAmount), 0.0f, MaxPoise);
    bIsStaggered = (CurrentPoise <= 0.0f);
    return bIsStaggered;
}

void AKaelCharacter::RestorePoise(float Amount)
{
    CurrentPoise = FMath::Clamp(CurrentPoise + FMath::Max(0.0f, Amount), 0.0f, MaxPoise);
    if (CurrentPoise > 0.0f)
    {
        bIsStaggered = false;
    }
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
    const bool bDodged = EchoCombatComponent && EchoCombatComponent->ExecuteDodgeRoll(0.45f);
    if (bDodged)
    {
        if (UWorld* World = GetWorld())
        {
            if (UGameInstance* GI = World->GetGameInstance())
            {
                if (UEdrathAudioSubsystem* Audio = GI->GetSubsystem<UEdrathAudioSubsystem>())
                {
                    const EEdrathSurfaceType Surface = TerrainAdaptationComponent
                        ? TerrainAdaptationComponent->GetCurrentSurfaceType()
                        : EEdrathSurfaceType::Stone;
                    const float GroundSpeed = FMath::Max(GetVelocity().Size2D(), 800.0f);
                    Audio->ProcessLocomotionFootstepAudio(
                        Surface,
                        GetActorLocation(),
                        true,
                        GroundSpeed,
                        EEdrathLocomotionGait::DodgeRoll,
                        World->GetTimeSeconds());
                }
            }
        }
    }
    return bDodged;
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
