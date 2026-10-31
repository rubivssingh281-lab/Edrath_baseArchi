#include "World/UnwrittenIslandSpawner.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AUnwrittenIslandSpawner::AUnwrittenIslandSpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    IslandInstanceComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("IslandInstances"));
    RootComponent = IslandInstanceComponent;
    IslandInstanceComponent->SetCullDistances(0, 6500);
}

void AUnwrittenIslandSpawner::BeginPlay()
{
    Super::BeginPlay();
    if (!CullingReferenceActor && GetWorld())
    {
        CullingReferenceActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    }
    MaterializeUnchosenPossibilities();
}

int32 AUnwrittenIslandSpawner::MaterializeUnchosenPossibilities()
{
    if (!GetWorld() || !GetWorld()->GetGameInstance() || !MaterializedIslandClass)
    {
        return 0;
    }

    UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>();
    if (!Narrative)
    {
        return 0;
    }

    const TArray<FGameplayTag> PossibilityTags = Narrative->GetUnchosenPossibilityTags();
    for (int32 Index = MaterializedIslands.Num(); Index < PossibilityTags.Num(); ++Index)
    {
        const FGameplayTag PossibilityTag = PossibilityTags[Index];
        const FVector Location = GetActorLocation() + FVector((Index + 1) * IslandSpacing, 0.0f, 0.0f);
        AActor* Island = GetWorld()->SpawnActor<AActor>(MaterializedIslandClass, Location, GetActorRotation());
        if (Island)
        {
            Island->Tags.AddUnique(FName(TEXT("UnchosenPossibilityIsland")));
            Island->Tags.AddUnique(PossibilityTag.GetTagName());
            FUnwrittenIslandInstance Instance;
            Instance.SourcePossibilityTag = PossibilityTag;
            Instance.IslandActor = Island;
            Instance.WorldLocation = Location;
            MaterializedIslands.Add(Instance);
            SpawnedIslandActors.Add(Island);
        }
    }

    RefreshDistanceCulling();
    return MaterializedIslands.Num();
}

void AUnwrittenIslandSpawner::RefreshDistanceCulling()
{
    const FVector ReferenceLocation = CullingReferenceActor
        ? CullingReferenceActor->GetActorLocation()
        : GetActorLocation();
    TArray<int32> SortedIndices;
    for (int32 Index = 0; Index < MaterializedIslands.Num(); ++Index)
    {
        SortedIndices.Add(Index);
    }
    SortedIndices.Sort([this, &ReferenceLocation](int32 Left, int32 Right)
    {
        return FVector::DistSquared(MaterializedIslands[Left].WorldLocation, ReferenceLocation) <
            FVector::DistSquared(MaterializedIslands[Right].WorldLocation, ReferenceLocation);
    });

    TSet<int32> VisibleIndices;
    for (int32 Rank = 0; Rank < SortedIndices.Num() && Rank < FMath::Max(0, MaxVisibleIslands); ++Rank)
    {
        const int32 Index = SortedIndices[Rank];
        if (FVector::DistSquared(MaterializedIslands[Index].WorldLocation, ReferenceLocation) <=
            FMath::Square(IslandCullDistance))
        {
            VisibleIndices.Add(Index);
        }
    }

    for (int32 Index = 0; Index < MaterializedIslands.Num(); ++Index)
    {
        if (AActor* Island = MaterializedIslands[Index].IslandActor)
        {
            Island->SetActorHiddenInGame(!VisibleIndices.Contains(Index));
            Island->SetActorEnableCollision(VisibleIndices.Contains(Index));
        }
    }
}

void AUnwrittenIslandSpawner::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    RefreshDistanceCulling();
}
