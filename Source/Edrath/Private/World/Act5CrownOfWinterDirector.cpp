#include "World/Act5CrownOfWinterDirector.h"
#include "Character/KaelCharacter.h"
#include "Camera/EdrathInteriorTransitionVolume.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AAct5CrownOfWinterDirector::AAct5CrownOfWinterDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAct5CrownOfWinterDirector::InitializeAct5(AKaelCharacter* InKael, AEdrathInteriorTransitionVolume* InChamberVolume)
{
    KaelCharacter = InKael;
    TimeDilatedChamber = InChamberVolume;

    if (KaelCharacter)
    {
        KaelCharacter->CurrentRegion = EEdrathRegion::CrownOfWinter;
        KaelCharacter->EquippedLifeEchoArchetype = EKaelLifeEchoArchetype::TimeArchivist_Act5;
    }

    SetState(EAct5CrownOfWinterState::AscentToCrownOfWinter);

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct5ArchiveReached,
                FText::FromString(TEXT("Kael ascended into The Crown of Winter, where time dilates and ancient watchers wait.")),
                false
            );
        }
    }
}

int32 AAct5CrownOfWinterDirector::SimulateInteriorTimeDilation(float SecondsIndoors)
{
    const float Multiplier = TimeDilatedChamber ? TimeDilatedChamber->SimulatedDaysPerInteriorSecond : 180.0f;
    const int32 SimulatedDays = FMath::Max(1, FMath::RoundToInt(SecondsIndoors * Multiplier));

    TotalElapsedExteriorDays += SimulatedDays;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AdvanceExteriorTimeDays(SimulatedDays);
        }
    }

    SetState(EAct5CrownOfWinterState::TimeDilatedChamberTraversed);
    OnTimeDilationSimulated.Broadcast(SecondsIndoors, SimulatedDays);
    return SimulatedDays;
}

bool AAct5CrownOfWinterDirector::InfiltrateConsciousArchive()
{
    if (bArchiveInfiltrated)
    {
        return false;
    }

    bArchiveInfiltrated = true;
    SetState(EAct5CrownOfWinterState::ConsciousArchiveInfiltrated);
    return true;
}

bool AAct5CrownOfWinterDirector::ConductArchiveDialogue(bool bAcknowledgeRequest)
{
    if (bArchiveRequestAcknowledged)
    {
        return false;
    }

    bArchiveRequestAcknowledged = true;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            NarrativeSys->AddOrMutateEvidence(
                EdrathGameplayTags::StoryAct5ArchiveRequest,
                FText::FromString(TEXT("Twist 5 Revealed: The Conscious Archive has been shrinking reality to prevent ontological collapse, and asks Kael to end its existence.")),
                true // Core Twist 5
            );
        }
    }

    SetState(EAct5CrownOfWinterState::ArchiveDialogueConducted);
    OnArchiveRequestReceived.Broadcast(bAcknowledgeRequest);
    return true;
}

bool AAct5CrownOfWinterDirector::CommitOntologicalChoice(bool bAgreeToDestroyArchive)
{
    if (bChoiceCommitted)
    {
        return false;
    }

    bChoiceCommitted = true;
    SetState(EAct5CrownOfWinterState::Act5Completed);
    return true;
}

void AAct5CrownOfWinterDirector::SetState(EAct5CrownOfWinterState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnAct5StateChanged.Broadcast(CurrentState);
    }
}
