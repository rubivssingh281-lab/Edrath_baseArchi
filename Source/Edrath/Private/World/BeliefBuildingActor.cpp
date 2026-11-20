#include "World/BeliefBuildingActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

ABeliefBuildingActor::ABeliefBuildingActor()
{
    PrimaryActorTick.bCanEverTick = true;
    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    RootComponent = BuildingMesh;
}

void ABeliefBuildingActor::BeginPlay()
{
    Super::BeginPlay();

    if (BuildingMesh)
    {
        const int32 NumMats = BuildingMesh->GetNumMaterials();
        for (int32 i = 0; i < NumMats; ++i)
        {
            UMaterialInstanceDynamic* DynMat = BuildingMesh->CreateAndSetMaterialInstanceDynamic(i);
            if (DynMat)
            {
                DynamicMaterials.Add(DynMat);
                DynMat->SetScalarParameterValue(TEXT("DissolveAlpha"), CurrentDissolveAlpha);
                DynMat->SetScalarParameterValue(TEXT("EdgeWidth"), DissolveEdgeWidth);
                DynMat->SetVectorParameterValue(TEXT("EdgeColor"), DissolveEdgeColor);
            }
        }
    }

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
    if (ChangedTag != BuildingBeliefTag)
    {
        return;
    }

    const bool bNewManifested = (NewBeliefWeight >= ExistenceBeliefThreshold);
    TargetDissolveAlpha = bNewManifested ? 0.0f : 1.0f;

    if (bNewManifested != bIsManifested)
    {
        bIsManifested = bNewManifested;
        OnBuildingDissolveStateChanged.Broadcast(BuildingBeliefTag, !bIsManifested);
    }
}

void ABeliefBuildingActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!FMath::IsNearlyEqual(CurrentDissolveAlpha, TargetDissolveAlpha, 0.005f))
    {
        CurrentDissolveAlpha = FMath::FInterpTo(CurrentDissolveAlpha, TargetDissolveAlpha, DeltaTime, DissolveInterpSpeed);

        for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
        {
            if (DynMat)
            {
                DynMat->SetScalarParameterValue(TEXT("DissolveAlpha"), CurrentDissolveAlpha);
            }
        }

        if (BuildingMesh)
        {
            const bool bPhysicsActive = (CurrentDissolveAlpha < 0.5f);
            BuildingMesh->SetCollisionEnabled(bPhysicsActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

            const bool bVisible = (CurrentDissolveAlpha < 0.99f);
            BuildingMesh->SetVisibility(bVisible, true);
        }
    }
}
