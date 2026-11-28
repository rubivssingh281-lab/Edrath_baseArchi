#include "Character/FirstArchivistBossCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

AFirstArchivistBossCharacter::AFirstArchivistBossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    TimeArchiveCrownMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TimeArchiveCrownMesh"));
    TimeArchiveCrownMesh->SetupAttachment(GetMesh(), TEXT("head"));

    ChronoBladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChronoBladeMesh"));
    ChronoBladeMesh->SetupAttachment(GetMesh(), TEXT("hand_r_weapon"));
}

void AFirstArchivistBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    ShieldStability = 100.0f;
    CurrentPhase = EArchivistBossPhase::Phase1_EchoProjection;
}

float AFirstArchivistBossCharacter::ApplyBossDamage(float DamageAmount, bool bIsEchoStrike)
{
    if (CurrentPhase == EArchivistBossPhase::Defeated)
    {
        return 0.0f;
    }

    // Phase 2: Crystalline Shield absorbs normal damage; requires Echo Strikes to break
    if (CurrentPhase == EArchivistBossPhase::Phase2_CrystallineShield && ShieldStability > 0.0f)
    {
        if (bIsEchoStrike)
        {
            ShatterCrystallineShield(DamageAmount * 1.5f);
            return DamageAmount * 0.5f;
        }
        return 0.0f; // Immune to standard attacks while shield is intact
    }

    const float ActualDamage = FMath::Min(CurrentHealth, DamageAmount);
    CurrentHealth -= ActualDamage;

    // Check Phase Transitions
    const float HealthRatio = GetHealthRatio();
    if (CurrentHealth <= 0.0f)
    {
        AdvancePhase(EArchivistBossPhase::Defeated);
    }
    else if (HealthRatio <= 0.35f && CurrentPhase == EArchivistBossPhase::Phase2_CrystallineShield)
    {
        AdvancePhase(EArchivistBossPhase::Phase3_PossibilityCollapse);
    }
    else if (HealthRatio <= 0.70f && CurrentPhase == EArchivistBossPhase::Phase1_EchoProjection)
    {
        AdvancePhase(EArchivistBossPhase::Phase2_CrystallineShield);
    }

    return ActualDamage;
}

bool AFirstArchivistBossCharacter::ShatterCrystallineShield(float StaggerValue)
{
    if (CurrentPhase != EArchivistBossPhase::Phase2_CrystallineShield || ShieldStability <= 0.0f)
    {
        return false;
    }

    ShieldStability = FMath::Max(0.0f, ShieldStability - StaggerValue);
    if (ShieldStability <= 0.0f)
    {
        OnShieldBroken.Broadcast();
    }
    return true;
}

void AFirstArchivistBossCharacter::AdvancePhase(EArchivistBossPhase NewPhase)
{
    if (CurrentPhase != NewPhase)
    {
        CurrentPhase = NewPhase;
        if (CurrentPhase == EArchivistBossPhase::Phase2_CrystallineShield)
        {
            ShieldStability = 100.0f;
        }
        else if (CurrentPhase == EArchivistBossPhase::Defeated)
        {
            CurrentHealth = 0.0f;
            OnBossDefeated.Broadcast();
        }
        OnPhaseChanged.Broadcast(CurrentPhase);
    }
}

float AFirstArchivistBossCharacter::ReceiveChronalHit_Implementation(
    float DamageAmount,
    float,
    bool bIsEchoStrike,
    AActor*,
    const FVector&)
{
    return ApplyBossDamage(DamageAmount, bIsEchoStrike);
}
