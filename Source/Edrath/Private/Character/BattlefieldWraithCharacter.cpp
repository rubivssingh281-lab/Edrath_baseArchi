#include "Character/BattlefieldWraithCharacter.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Core/EdrathGameplayTags.h"

ABattlefieldWraithCharacter::ABattlefieldWraithCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentHealth = MaxHealth;
    bIsStaggered = false;
    bIsDefeated = false;
    ActiveTimelinePhase = ETimelineState::PresentTimeline;
    bTimelinePhasedShieldActive = true;
    DissolveProgress = 0.0f;

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
        GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    }

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = 350.0f;
        GetCharacterMovement()->bOrientRotationToMovement = true;
    }
}

void ABattlefieldWraithCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;

    if (!BoundDirector.IsValid() && GetWorld())
    {
        AActor* FoundDirector = UGameplayStatics::GetActorOfClass(GetWorld(), AAct1PreAlphaSliceDirector::StaticClass());
        if (FoundDirector)
        {
            BindToDirector(Cast<AAct1PreAlphaSliceDirector>(FoundDirector));
        }
    }
}

void ABattlefieldWraithCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsStaggered)
    {
        StaggerRemainingTime -= DeltaSeconds;
        if (StaggerRemainingTime <= 0.0f)
        {
            bIsStaggered = false;
            bTimelinePhasedShieldActive = true;
        }
    }

    if (bIsDefeated && DissolveProgress < 1.0f)
    {
        DissolveProgress = FMath::Clamp(DissolveProgress + DeltaSeconds * 1.5f, 0.0f, 1.0f);
        if (DissolveProgress >= 1.0f)
        {
            SetActorHiddenInGame(true);
            SetActorEnableCollision(false);
        }
    }
}

bool ABattlefieldWraithCharacter::TakeSlashDamage(float InDamage, AActor* DamageCauser)
{
    if (bIsDefeated)
    {
        return false;
    }

    float EffectiveDamage = InDamage * BaseSlashVulnerabilityMultiplier;
    if (bTimelinePhasedShieldActive && !bIsStaggered)
    {
        // Shielded wraiths resist 60% of conventional direct slashes
        EffectiveDamage *= 0.4f;
    }

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - EffectiveDamage);

    if (CurrentHealth <= 0.0f)
    {
        bIsDefeated = true;
        bIsStaggered = false;
        if (GetCapsuleComponent())
        {
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        OnWraithDefeated.Broadcast(this);
        if (BoundDirector.IsValid())
        {
            BoundDirector->RegisterBattlefieldWraithDefeated();
        }
    }

    return true;
}

bool ABattlefieldWraithCharacter::TakeEchoStrikeDamage(float InDamage, AActor* DamageCauser, float ProjectionDelay)
{
    if (bIsDefeated)
    {
        return false;
    }

    // Echo Strikes bypass the timeline phase shield and shatter wraith defenses
    const float CriticalDamage = InDamage * EchoStrikeVulnerabilityMultiplier;
    bTimelinePhasedShieldActive = false;
    bIsStaggered = true;
    StaggerRemainingTime = 2.5f;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - CriticalDamage);
    OnWraithStaggered.Broadcast(this, StaggerRemainingTime);

    if (CurrentHealth <= 0.0f)
    {
        bIsDefeated = true;
        bIsStaggered = false;
        if (GetCapsuleComponent())
        {
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        OnWraithDefeated.Broadcast(this);
        if (BoundDirector.IsValid())
        {
            BoundDirector->RegisterBattlefieldWraithDefeatedUsingEchoCombat(ProjectionDelay);
        }
    }

    return true;
}

void ABattlefieldWraithCharacter::ApplyTimelinePhaseShift(ETimelineState NewPhase)
{
    ActiveTimelinePhase = NewPhase;
    bTimelinePhasedShieldActive = (NewPhase != ETimelineState::MemoryReconstruction);
}

void ABattlefieldWraithCharacter::BindToDirector(AAct1PreAlphaSliceDirector* InDirector)
{
    BoundDirector = InDirector;
}
