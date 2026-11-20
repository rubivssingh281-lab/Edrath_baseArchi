#include "World/MemoryFruitTreeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AMemoryFruitTreeActor::AMemoryFruitTreeActor()
{
    PrimaryActorTick.bCanEverTick = false;
    TreeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeMesh"));
    RootComponent = TreeMesh;
}

AActor* AMemoryFruitTreeActor::HarvestMemoryFruit(AActor* Harvester)
{
    if (bHasBeenHarvested || !Harvester || !GetWorld())
    {
        return nullptr;
    }

    bHasBeenHarvested = true;
    TreeMesh->SetVisibility(false, true);
    TreeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (MemoryAreaActorClass)
    {
        const FTransform SpawnTransform(GetActorRotation(), GetActorLocation() + FVector(0.0f, 0.0f, 120.0f));
        SpawnedMemoryArea = GetWorld()->SpawnActorDeferred<AActor>(MemoryAreaActorClass, SpawnTransform);
        if (SpawnedMemoryArea)
        {
            SpawnedMemoryArea->FinishSpawning(SpawnTransform);
        }
    }
    else if (!MemoryLevelPackage.IsNone())
    {
        bool bLoaded = false;
        StreamedMemoryLevel = ULevelStreamingDynamic::LoadLevelInstance(
            GetWorld(), MemoryLevelPackage.ToString(), GetActorLocation(), GetActorRotation(), bLoaded);
        if (!bLoaded)
        {
            StreamedMemoryLevel = nullptr;
        }
    }

    OnMemoryFruitHarvested.Broadcast(MemoryEchoArchetype, SpawnedMemoryArea);
    return SpawnedMemoryArea;
}

void AMemoryFruitTreeActor::ModifyMemoryAnchor(FGameplayTag AnchorTag, FText MutationDescription)
{
    if (!AnchorTag.IsValid() || !GetWorld() || !GetWorld()->GetGameInstance())
    {
        return;
    }

    if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
    {
        Narrative->RecordForestMutation(AnchorTag, MoveTemp(MutationDescription));
        if (ForestMutationTag.IsValid() && ForestMutationTag != AnchorTag)
        {
            Narrative->RecordForestMutation(ForestMutationTag, FText::FromString(TEXT("A Hollowgreen memory anchor altered the parent forest.")));
        }
    }
}
