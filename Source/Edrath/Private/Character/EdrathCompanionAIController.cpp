#include "Character/EdrathCompanionAIController.h"
#include "Character/EdrathCompanionCharacter.h"
#include "Character/KaelCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEdrathCompanionAIController::AEdrathCompanionAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    bWantsPlayerState = false;
}

void AEdrathCompanionAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ControlledCompanion = Cast<AEdrathCompanionCharacter>(InPawn);

    if (GetWorld())
    {
        ObservedHero = Cast<AKaelCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    }
}

void AEdrathCompanionAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ForesightAnticipationTimer > 0.0f)
    {
        ForesightAnticipationTimer = FMath::Max(0.0f, ForesightAnticipationTimer - DeltaTime);
        if (ForesightAnticipationTimer <= 0.0f && CurrentMode == ECompanionBehaviorMode::CombatSupport)
        {
            SetBehaviorMode(ECompanionBehaviorMode::FollowingHero);
        }
    }
}

void AEdrathCompanionAIController::SetBehaviorMode(ECompanionBehaviorMode NewMode)
{
    if (CurrentMode != NewMode)
    {
        CurrentMode = NewMode;
        OnBehaviorChanged.Broadcast(CurrentMode);
    }
}

void AEdrathCompanionAIController::ReactToBlackClockUsage(float SuspicionDelta)
{
    SuspicionLevel = FMath::Clamp(SuspicionLevel + SuspicionDelta, 0.0f, 100.0f);
    const bool bDefecting = (SuspicionLevel >= 85.0f);

    if (ControlledCompanion)
    {
        const FCompanionRelationshipState Rel = ControlledCompanion->GetRelationshipState();
        ControlledCompanion->SetAutonomousState(Rel.Loyalty, SuspicionLevel, Rel.bWithholdingCriticalTruth);
    }

    if (bDefecting)
    {
        SetBehaviorMode(ECompanionBehaviorMode::HostileDefection);
    }
    else if (SuspicionLevel >= 50.0f && CurrentMode == ECompanionBehaviorMode::FollowingHero)
    {
        SetBehaviorMode(ECompanionBehaviorMode::SuspiciousObservation);
    }

    OnSuspicionEscalated.Broadcast(SuspicionLevel, bDefecting);
}

void AEdrathCompanionAIController::ReactToForesightWarning(float WarningSeconds)
{
    ForesightAnticipationTimer = WarningSeconds;
    SetBehaviorMode(ECompanionBehaviorMode::CombatSupport);
}

void AEdrathCompanionAIController::ReactToEnemyPoiseShatter(AActor* StaggeredTarget, float StaggerDuration)
{
    if (CurrentMode == ECompanionBehaviorMode::HostileDefection)
    {
        return;
    }

    AssistTargetActor = StaggeredTarget;
    ForesightAnticipationTimer = FMath::Max(ForesightAnticipationTimer, StaggerDuration);
    SetBehaviorMode(ECompanionBehaviorMode::CombatSupport);
}

void AEdrathCompanionAIController::AdaptFollowBehaviorToWeather(EEdrathWeatherType ActiveWeather, float VisibilityDistance)
{
    const bool bLowVisibilityWeather =
        ActiveWeather == EEdrathWeatherType::CrownBlizzard ||
        ActiveWeather == EEdrathWeatherType::VeylPossibilityFog ||
        ActiveWeather == EEdrathWeatherType::AshenChronalStorm ||
        VisibilityDistance < 3600.0f;

    FollowDistance = bLowVisibilityWeather ? 160.0f : 280.0f;
}

AEdrathCompanionCharacter* AEdrathCompanionAIController::GetCompanionCharacter() const
{
    return ControlledCompanion;
}
