#include "World/LastHonestKingMonumentActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Narrative/EdrathQuestSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

ALastHonestKingMonumentActor::ALastHonestKingMonumentActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    MonumentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonumentMesh"));
    MonumentMesh->SetupAttachment(SceneRoot);

    ConfessionPlaqueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConfessionPlaqueMesh"));
    ConfessionPlaqueMesh->SetupAttachment(SceneRoot);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(SceneRoot);
    InteractionSphere->SetSphereRadius(240.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

bool ALastHonestKingMonumentActor::InspectConfessionPlaque()
{
    bPlaqueInspected = true;
    return true;
}

bool ALastHonestKingMonumentActor::ResolveRoyalConfession(bool bExposeTrueCriminal)
{
    if (bIsResolved)
    {
        return false;
    }

    bIsResolved = true;
    bTrueCriminalExposed = bExposeTrueCriminal;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathQuestSubsystem* QuestSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathQuestSubsystem>())
        {
            QuestSys->ResolveLastHonestKing(bExposeTrueCriminal);
        }
    }

    OnMonumentResolved.Broadcast(bExposeTrueCriminal);
    return true;
}
