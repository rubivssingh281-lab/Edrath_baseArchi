#include "Character/MaevraPursuerBossCharacter.h"
#include "BlackClock/EdrathCombatFeedbackSubsystem.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Core/EdrathTelemetrySubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AMaevraPursuerBossCharacter::AMaevraPursuerBossCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AlchemicalAlembicMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AlchemicalAlembicMesh"));
    AlchemicalAlembicMesh->SetupAttachment(GetMesh(), TEXT("hand_l_relic"));
}

void AMaevraPursuerBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    CurrentPoise = MaxPoise;
    bAlchemicalBarrierActive = true;
    EncounterState = EMaevraEncounterState::HostilePursuer;
}

void AMaevraPursuerBossCharacter::TransitionEncounterState(EMaevraEncounterState NewState)
{
    if (EncounterState != NewState)
    {
        EncounterState = NewState;
        OnMaevraStateChanged.Broadcast(EncounterState);
    }
}

float AMaevraPursuerBossCharacter::ReceiveChronalHit_Implementation(
    float DamageAmount,
    float PoiseDamage,
    bool bIsEchoStrike,
    AActor* InstigatorActor,
    const FVector& HitLocation)
{
    if (EncounterState == EMaevraEncounterState::RecruitedAlly || EncounterState == EMaevraEncounterState::Defeated)
    {
        return 0.0f;
    }

    // Echo Strikes bypass and fracture Maevra's alchemical potion barrier cleanly
    float EffectiveDamage = FMath::Max(0.0f, DamageAmount);
    if (bAlchemicalBarrierActive)
    {
        if (bIsEchoStrike)
        {
            bAlchemicalBarrierActive = false;
            EffectiveDamage *= 1.4f;
        }
        else
        {
            EffectiveDamage *= 0.45f;
        }
    }

    // Evaluate poise shatter via combat feedback subsystem if running in a live GameInstance
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UEdrathCombatFeedbackSubsystem* Feedback = GI->GetSubsystem<UEdrathCombatFeedbackSubsystem>())
            {
                const FEdrathPoiseEvaluationResult PoiseResult = Feedback->EvaluatePoiseBreak(
                    CurrentPoise, PoiseDamage, false, bIsEchoStrike, World->GetTimeSeconds());
                CurrentPoise = PoiseResult.RemainingPoise;
                if (PoiseResult.bPoiseBroken)
                {
                    bAlchemicalBarrierActive = false;
                }
            }
            else
            {
                CurrentPoise = FMath::Max(0.0f, CurrentPoise - FMath::Max(0.0f, PoiseDamage));
            }
        }
    }

    const float AppliedDamage = FMath::Min(CurrentHealth, EffectiveDamage);
    CurrentHealth -= AppliedDamage;

    const float Ratio = GetHealthRatio();
    if (CurrentHealth <= 0.0f)
    {
        DefeatMaevra();
    }
    else if (Ratio <= 0.35f && EncounterState == EMaevraEncounterState::HostilePursuer)
    {
        // Kneel and offer the Black Clock marriage restoration pact
        TransitionEncounterState(EMaevraEncounterState::ClockBargainOffered);
    }

    return AppliedDamage;
}

bool AMaevraPursuerBossCharacter::RecruitMaevraViaClockBargain(AActor* Recruiter)
{
    if (EncounterState != EMaevraEncounterState::ClockBargainOffered &&
        EncounterState != EMaevraEncounterState::ShieldShattered_Kneeling)
    {
        return false;
    }

    TransitionEncounterState(EMaevraEncounterState::RecruitedAlly);

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (UEdrathNarrativeSubsystem* Narrative = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                TMap<FName, FCompanionRelationshipState> CompanionMap = Narrative->GetCompanionStates();
                FCompanionRelationshipState& MaevraRel = CompanionMap.FindOrAdd(FName(TEXT("Maevra")));
                MaevraRel.CompanionID = FName(TEXT("Maevra"));
                MaevraRel.Loyalty = FMath::Max(MaevraRel.Loyalty, 45.0f);
                MaevraRel.SuspicionOfKael = 30.0f;
                MaevraRel.bWithholdingCriticalTruth = false;
                Narrative->SetCompanionStates(CompanionMap);
            }

            if (UEdrathTelemetrySubsystem* Telemetry = GI->GetSubsystem<UEdrathTelemetrySubsystem>())
            {
                Telemetry->RecordBreadcrumb(TEXT("Act2"), TEXT("Maevra recruited via Black Clock spouse bargain."));
            }
        }
    }

    OnMaevraRecruited.Broadcast(Recruiter);
    return true;
}

void AMaevraPursuerBossCharacter::DefeatMaevra()
{
    CurrentHealth = 0.0f;
    bAlchemicalBarrierActive = false;
    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    TransitionEncounterState(EMaevraEncounterState::Defeated);
}

bool AMaevraPursuerBossCharacter::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor) &&
           (EncounterState == EMaevraEncounterState::ClockBargainOffered ||
            EncounterState == EMaevraEncounterState::ShieldShattered_Kneeling);
}

FText AMaevraPursuerBossCharacter::GetInteractionPrompt_Implementation(AActor*) const
{
    return FText::FromString(TEXT("[E] Pact of the Black Clock: Recruit Maevra"));
}

FText AMaevraPursuerBossCharacter::GetInteractionTargetName_Implementation() const
{
    return FText::FromString(TEXT("Maevra, Royal Alchemist"));
}

bool AMaevraPursuerBossCharacter::ExecuteInteraction_Implementation(AActor* Interactor)
{
    return RecruitMaevraViaClockBargain(Interactor);
}
