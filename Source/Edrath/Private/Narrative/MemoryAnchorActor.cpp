#include "Narrative/MemoryAnchorActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AMemoryAnchorActor::AMemoryAnchorActor()
{
    PrimaryActorTick.bCanEverTick = false;
    AnchorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorMesh"));
    RootComponent = AnchorMesh;
    CurrentMemoryDescription = FText::FromString(TEXT("Unresolved temporal memory echo."));
}

bool AMemoryAnchorActor::AlterAnchor(AActor* InstigatorActor, FText NewDescription)
{
    if (bIsAnchorAltered || !InstigatorActor || !GetWorld())
    {
        return false;
    }

    bIsAnchorAltered = true;
    CurrentMemoryDescription = MoveTemp(NewDescription);

    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            if (AnchorTag.IsValid())
            {
                Narrative->RecordForestMutation(AnchorTag, CurrentMemoryDescription);
            }
        }
    }

    OnMemoryAnchorAltered.Broadcast(AnchorTag, CurrentMemoryDescription, InstigatorActor);
    return true;
}
