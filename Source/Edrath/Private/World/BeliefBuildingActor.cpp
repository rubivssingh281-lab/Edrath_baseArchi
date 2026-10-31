#include "World/BeliefBuildingActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

ABeliefBuildingActor::ABeliefBuildingActor()
{
    PrimaryActorTick.bCanEverTick = false;
    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    RootComponent = BuildingMesh;
}

void ABeliefBuildingActor::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->OnBeliefWeightChanged.AddDynamic(this, &ABeliefBuildingActor::EvaluateBeliefExistence);
            EvaluateBeliefExistence(BuildingBeliefTag, Narrative->GetCitizenBeliefWeight(BuildingBeliefTag));
        }
    }
}

void ABeliefBuildingActor::EvaluateBeliefExistence(FGameplayTag ChangedTag, float NewBeliefWeight)
{
    if (ChangedTag != BuildingBeliefTag || !BuildingMesh)
    {
        return;
    }
    const bool bExists = NewBeliefWeight >= ExistenceBeliefThreshold;
    BuildingMesh->SetVisibility(bExists, true);
    BuildingMesh->SetCollisionEnabled(bExists ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}
