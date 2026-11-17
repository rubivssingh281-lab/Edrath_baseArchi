#include "World/Act3HollowgreenDirector.h"
#include "Character/KaelCharacter.h"
#include "Character/KaelModularRigComponent.h"
#include "World/MemoryFruitTreeActor.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

AAct3HollowgreenDirector::AAct3HollowgreenDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAct3HollowgreenDirector::InitializeAct3(AKaelCharacter* InKael, AMemoryFruitTreeActor* InMemoryTree)
{
    KaelCharacter = InKael;
    ActiveMemoryTree = InMemoryTree;

    SetState(EAct3HollowgreenState::ExploringMemoryRealms);

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RegisterBlackClockActivation(EEdrathRegion::Hollowgreen, 5.0f);
        }
    }
}

bool AAct3HollowgreenDirector::RegisterEchoDiscovery(EKaelLifeEchoArchetype EchoArchetype)
{
    if (CurrentState == EAct3HollowgreenState::ArrivalAtHollowgreen ||
        CurrentState == EAct3HollowgreenState::Act3Completed)
    {
        return false;
    }

    // Verify it is one of the three canonical Act 3 alternate life archetypes
    if (EchoArchetype != EKaelLifeEchoArchetype::PlaguedHealer_Act3 &&
        EchoArchetype != EKaelLifeEchoArchetype::TyrantConqueror_Act3 &&
        EchoArchetype != EKaelLifeEchoArchetype::SimpleFarmer_Act3)
    {
        return false;
    }

    if (!DiscoveredEchoes.Contains(EchoArchetype))
    {
        DiscoveredEchoes.Add(EchoArchetype);
        OnEchoDiscovered.Broadcast(EchoArchetype);

        // Equip matching modular outfit on Kael if rig component is available
        if (KaelCharacter)
        {
            if (UKaelModularRigComponent* RigComp = KaelCharacter->FindComponentByClass<UKaelModularRigComponent>())
            {
                RigComp->EquipOutfitPreset(EchoArchetype);
            }
        }

        // Advance state tracking
        if (EchoArchetype == EKaelLifeEchoArchetype::PlaguedHealer_Act3)
        {
            SetState(EAct3HollowgreenState::HealerEchoDiscovered);
        }
        else if (EchoArchetype == EKaelLifeEchoArchetype::TyrantConqueror_Act3)
        {
            SetState(EAct3HollowgreenState::TyrantEchoDiscovered);
        }
        else if (EchoArchetype == EKaelLifeEchoArchetype::SimpleFarmer_Act3)
        {
            SetState(EAct3HollowgreenState::FarmerEchoDiscovered);
        }

        // Once all 3 alternate life echoes are understood, trigger the Ilyra confrontation
        if (DiscoveredEchoes.Num() >= 3)
        {
            SetState(EAct3HollowgreenState::IlyraConfrontation);
        }

        return true;
    }

    return false;
}

bool AAct3HollowgreenDirector::ConfrontIlyra(bool bAcceptTruth)
{
    if (CurrentState != EAct3HollowgreenState::IlyraConfrontation || bIlyraConfronted)
    {
        return false;
    }

    bIlyraConfronted = true;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            const FText ClueDescription = bAcceptTruth
                ? FText::FromString(TEXT("Kael's alternate lives were sequential attempts to stop the catastrophe. Ilyra killed Kael in Hollowgreen to prevent repeating his greatest mistake."))
                : FText::FromString(TEXT("Kael resists the truth of the loop iterations, escalating narrative instability across Hollowgreen."));

            Narrative->AddOrMutateEvidence(EdrathGameplayTags::Twist3, ClueDescription, true);
            Narrative->RegisterBlackClockActivation(EEdrathRegion::Hollowgreen, 15.0f);
        }
    }

    OnIlyraConfrontationCompleted.Broadcast(bAcceptTruth);
    SetState(EAct3HollowgreenState::Act3Completed);
    return true;
}

bool AAct3HollowgreenDirector::HasDiscoveredEcho(EKaelLifeEchoArchetype EchoArchetype) const
{
    return DiscoveredEchoes.Contains(EchoArchetype);
}

void AAct3HollowgreenDirector::SetState(EAct3HollowgreenState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnStateChanged.Broadcast(NewState);
    }
}
