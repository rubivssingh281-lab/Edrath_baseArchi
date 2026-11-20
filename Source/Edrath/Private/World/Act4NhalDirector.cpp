#include "World/Act4NhalDirector.h"
#include "Character/KaelCharacter.h"
#include "World/NhalFactionManager.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AAct4NhalDirector::AAct4NhalDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAct4NhalDirector::InitializeAct4(AKaelCharacter* InKael, ANhalFactionManager* InFactionManager)
{
    KaelCharacter = InKael;
    FactionManager = InFactionManager;

    if (KaelCharacter)
    {
        KaelCharacter->CurrentRegion = EEdrathRegion::Nhal;
    }

    SetState(EAct4NhalState::InvestigatingFactionStrife);

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct4NhalDiscovered,
                FText::FromString(TEXT("Kael penetrated the subterranean metropolis of Nhal, where the dead remember unlived futures.")),
                false
            );
        }
    }
}

bool AAct4NhalDirector::TriggerDeadChildProphecy(const FText& ProphecyText)
{
    if (bProphecyReceived)
    {
        return false;
    }

    CachedProphecyText = ProphecyText;
    bProphecyReceived = true;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct4DeadChildProphecy,
                CachedProphecyText,
                true // Twist foreshadowing clue
            );
        }
    }

    SetState(EAct4NhalState::DeadChildProphecyReceived);
    OnDeadChildProphecyReceived.Broadcast(CachedProphecyText);
    return true;
}

bool AAct4NhalDirector::ResolveCompanionBetrayal(FName CompanionId, bool bReconcileWithAlly)
{
    if (bBetrayalAverted)
    {
        return false;
    }

    bBetrayalAverted = bReconcileWithAlly;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            const FString Desc = bReconcileWithAlly
                ? FString::Printf(TEXT("Kael proved his integrity to %s, preventing a future betrayal seen in her memories."), *CompanionId.ToString())
                : FString::Printf(TEXT("Tensions with %s severed trust, confirming the prophecy of betrayal."), *CompanionId.ToString());

            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct4CompanionBetrayalAverted,
                FText::FromString(Desc),
                true
            );
        }
    }

    SetState(EAct4NhalState::CompanionBetrayalAverted);
    OnCompanionBetrayalResolved.Broadcast(CompanionId, bReconcileWithAlly);
    return true;
}

bool AAct4NhalDirector::UncoverArchiveLedgerTwist()
{
    if (bLedgerTwistRevealed)
    {
        return false;
    }

    bLedgerTwistRevealed = true;

    if (FactionManager)
    {
        // Trigger the Twist 3 logic on the faction manager
        FactionManager->RestoreNhalCitizen(TEXT("Citizen_Archivist_Echo"));
    }

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::Twist3,
                FText::FromString(TEXT("Twist 3 Revealed: The erased citizens of Nhal were warmongers; restoring them retroactively despawns living families in Veyl.")),
                true // Core Twist 3
            );
        }
    }

    SetState(EAct4NhalState::Act4Completed);
    return true;
}

void AAct4NhalDirector::AdjustFactionStanding(FGameplayTag FactionTag, int32 Delta)
{
    if (FactionManager)
    {
        FactionManager->AdjustFactionReputation(FactionTag, Delta);
    }
}

void AAct4NhalDirector::SetState(EAct4NhalState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnAct4StateChanged.Broadcast(CurrentState);
    }
}
