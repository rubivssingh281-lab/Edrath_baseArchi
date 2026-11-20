#include "World/Act6UnwrittenSeaDirector.h"
#include "Character/KaelCharacter.h"
#include "Character/FirstArchivistBossCharacter.h"
#include "World/UnwrittenIslandSpawner.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AAct6UnwrittenSeaDirector::AAct6UnwrittenSeaDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAct6UnwrittenSeaDirector::InitializeAct6(AKaelCharacter* InKael, AUnwrittenIslandSpawner* InIslandSpawner)
{
    KaelCharacter = InKael;
    IslandSpawner = InIslandSpawner;

    if (KaelCharacter)
    {
        KaelCharacter->CurrentRegion = EEdrathRegion::UnwrittenSea;
    }

    SetState(EAct6UnwrittenSeaState::SailingSkyOcean);

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct6UnwrittenSeaSailed,
                FText::FromString(TEXT("Kael embarked upon The Unwritten Sea, navigating islands materialized from paths never taken.")),
                false
            );
        }
    }
}

bool AAct6UnwrittenSeaDirector::ConfrontFormerSelf(EKaelLifeEchoArchetype EchoArchetype)
{
    if (ConfrontedFormerSelves.Contains(EchoArchetype))
    {
        return false;
    }

    ConfrontedFormerSelves.Add(EchoArchetype);

    if (IslandSpawner)
    {
        IslandSpawner->RefreshDistanceCulling();
    }

    SetState(EAct6UnwrittenSeaState::ConfrontingFormerSelves);
    return true;
}

bool AAct6UnwrittenSeaDirector::InitiateFirstArchivistBattle(AFirstArchivistBossCharacter* InBoss)
{
    if (!InBoss)
    {
        return false;
    }

    BossArchivist = InBoss;
    SetState(EAct6UnwrittenSeaState::FirstArchivistBattle);
    return true;
}

bool AAct6UnwrittenSeaDirector::RecordArchivistDefeated()
{
    if (bArchivistDefeated)
    {
        return false;
    }

    bArchivistDefeated = true;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct6FirstArchivistDefeated,
                FText::FromString(TEXT("The First Archivist has been defeated. Reality-pruning safeguards dissolve in real time.")),
                true // Core Twist 4
            );

            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct6TrueIdentityRevealed,
                FText::FromString(TEXT("Twist 6 Revealed: Kael was the architect of the Conscious Archive's failsafe before awakening on the battlefield.")),
                true // Core Twist 6
            );
        }
    }

    SetState(EAct6UnwrittenSeaState::DecidingFinalPhilosophy);
    OnArchivistDefeated.Broadcast();
    return true;
}

EEdrathEndingType AAct6UnwrittenSeaDirector::SelectFinalEnding(EEdrathEndingType SelectedPhilosophy)
{
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            ResolvedEnding = NarrativeSys->EvaluateFinalEnding(SelectedPhilosophy);
        }
        else
        {
            ResolvedEnding = SelectedPhilosophy;
        }
    }
    else
    {
        ResolvedEnding = SelectedPhilosophy;
    }

    SetState(EAct6UnwrittenSeaState::Act6Completed);
    OnFinalEndingResolved.Broadcast(ResolvedEnding);
    return ResolvedEnding;
}

void AAct6UnwrittenSeaDirector::SetState(EAct6UnwrittenSeaState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnAct6StateChanged.Broadcast(CurrentState);
    }
}
