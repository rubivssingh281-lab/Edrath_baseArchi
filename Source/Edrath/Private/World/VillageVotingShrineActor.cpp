#include "World/VillageVotingShrineActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Narrative/EdrathQuestSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AVillageVotingShrineActor::AVillageVotingShrineActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    BallotUrnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallotUrnMesh"));
    BallotUrnMesh->SetupAttachment(SceneRoot);

    LedgerPedestalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LedgerPedestalMesh"));
    LedgerPedestalMesh->SetupAttachment(SceneRoot);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(SceneRoot);
    InteractionSphere->SetSphereRadius(220.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

bool AVillageVotingShrineActor::InspectVotingLedger()
{
    bLedgerInspected = true;
    return true;
}

bool AVillageVotingShrineActor::CastFinalVote(bool bResurrectDeceased, bool bExposeMayorCorruption)
{
    if (bVoteResolved)
    {
        return false;
    }

    bVoteResolved = true;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathQuestSubsystem* QuestSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathQuestSubsystem>())
        {
            QuestSys->ResolveVillageVotesOnDead(bResurrectDeceased, bExposeMayorCorruption);
        }
    }

    OnVoteResolved.Broadcast(bResurrectDeceased, bExposeMayorCorruption);
    return true;
}
