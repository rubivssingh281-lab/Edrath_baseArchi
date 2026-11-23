#include "World/DragonMemoryHoardActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Narrative/EdrathQuestSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

ADragonMemoryHoardActor::ADragonMemoryHoardActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    CrystalHoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrystalHoardMesh"));
    CrystalHoardMesh->SetupAttachment(SceneRoot);

    DragonActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DragonActorMesh"));
    DragonActorMesh->SetupAttachment(SceneRoot);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(SceneRoot);
    InteractionSphere->SetSphereRadius(350.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

bool ADragonMemoryHoardActor::ResolveHoardChoice(bool bReturnMemoriesToCitizens)
{
    if (bIsResolved)
    {
        return false;
    }

    bIsResolved = true;
    bMemoriesReturned = bReturnMemoriesToCitizens;
    bDragonApocalyptic = bReturnMemoriesToCitizens;

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathQuestSubsystem* QuestSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathQuestSubsystem>())
        {
            QuestSys->ResolveDragonNoFire(bReturnMemoriesToCitizens);
        }
    }

    OnDragonHoardResolved.Broadcast(bReturnMemoriesToCitizens);
    return true;
}
