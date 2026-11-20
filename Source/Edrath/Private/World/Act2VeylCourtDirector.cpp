#include "World/Act2VeylCourtDirector.h"
#include "Character/KaelCharacter.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "World/BeliefBuildingActor.h"

AAct2VeylCourtDirector::AAct2VeylCourtDirector()
{
    PrimaryActorTick.bCanEverTick = false;
    BeggarClueTag = EdrathGameplayTags::Twist1;
    GeneralClueTag = EdrathGameplayTags::Twist2;
    UnbornChildClueTag = EdrathGameplayTags::Twist3;
    PrinceMemoryTag = EdrathGameplayTags::Act2;
}

void AAct2VeylCourtDirector::BeginPlay()
{
    Super::BeginPlay();
}

void AAct2VeylCourtDirector::InitializeAct2(AKaelCharacter* InKael, ABeliefBuildingActor* InCathedral)
{
    KaelCharacter = InKael;
    GrandCathedral = InCathedral;
    CurrentState = EAct2VeylState::QueenSerathAccusation;
    OnAct2StateChanged.Broadcast(CurrentState);
}

bool AAct2VeylCourtDirector::AcceptQueenBargain()
{
    if (CurrentState != EAct2VeylState::QueenSerathAccusation)
    {
        return false;
    }

    CurrentState = EAct2VeylState::InvestigationOfThreeMurders;
    OnAct2StateChanged.Broadcast(CurrentState);
    return true;
}

bool AAct2VeylCourtDirector::InspectMurderScene(EVeylMurderVictim Victim)
{
    if (CurrentState != EAct2VeylState::InvestigationOfThreeMurders)
    {
        return false;
    }

    switch (Victim)
    {
        case EVeylMurderVictim::Beggar:
            bBeggarInspected = true;
            break;
        case EVeylMurderVictim::General:
            bGeneralInspected = true;
            break;
        case EVeylMurderVictim::UnbornChild:
            bUnbornChildInspected = true;
            break;
        default:
            return false;
    }

    OnMurderInspected.Broadcast(Victim);

    // Register forensic evidence clue with Narrative Subsystem
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            const FGameplayTag TagToMutate = (Victim == EVeylMurderVictim::Beggar) ? BeggarClueTag :
                                             (Victim == EVeylMurderVictim::General) ? GeneralClueTag : UnbornChildClueTag;

            Narrative->AddOrMutateEvidence(
                TagToMutate,
                FText::FromString(TEXT("Forensic examination confirms identical facial bone structure across impossible victims.")),
                true
            );
        }
    }

    if (AreAllMurdersInvestigated())
    {
        CurrentState = EAct2VeylState::CourtConspiracyConfrontation;
        OnAct2StateChanged.Broadcast(CurrentState);
    }

    return true;
}

bool AAct2VeylCourtDirector::AreAllMurdersInvestigated() const
{
    return bBeggarInspected && bGeneralInspected && bUnbornChildInspected;
}

bool AAct2VeylCourtDirector::RevealPrinceTruth()
{
    if (CurrentState != EAct2VeylState::CourtConspiracyConfrontation)
    {
        return false;
    }

    bPrinceTruthRevealed = true;
    CurrentState = EAct2VeylState::Act2Completed;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RecordRecoveredMemory(PrinceMemoryTag, FName(TEXT("Prince_Aemon")));
            Narrative->AddOrMutateEvidence(
                EdrathGameplayTags::Twist2,
                FText::FromString(TEXT("Prince Aemon was never murdered. Queen Serath ordered his memory erased.")),
                true
            );
        }
    }

    OnPrinceTruthRevealed.Broadcast();
    OnAct2StateChanged.Broadcast(CurrentState);
    return true;
}
