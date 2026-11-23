#include "World/WeddingAltarActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Narrative/EdrathQuestSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

AWeddingAltarActor::AWeddingAltarActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
    AltarMesh->SetupAttachment(SceneRoot);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(SceneRoot);
    InteractionSphere->SetSphereRadius(250.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

bool AWeddingAltarActor::ExecuteWeddingChoice(bool bMergeTimelines)
{
    if (bIsResolved)
    {
        return false;
    }

    bIsResolved = true;
    bMergedTimelines = bMergeTimelines;

    if (bMergeTimelines)
    {
        // When timelines merge, the separate families vanish from reality
        for (AActor* FamilyMember : CurrentLivingFamilyActors)
        {
            if (FamilyMember)
            {
                FamilyMember->SetActorHiddenInGame(true);
                FamilyMember->SetActorEnableCollision(false);
            }
        }
    }

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathQuestSubsystem* QuestSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathQuestSubsystem>())
        {
            QuestSys->ResolveWeddingNotHappened(bMergeTimelines);
        }
    }

    OnWeddingTimelineResolved.Broadcast(bMergeTimelines);
    return true;
}
