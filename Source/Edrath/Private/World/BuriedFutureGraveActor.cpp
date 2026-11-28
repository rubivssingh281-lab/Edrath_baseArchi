#include "World/BuriedFutureGraveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Narrative/EdrathQuestSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

ABuriedFutureGraveActor::ABuriedFutureGraveActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    GraveMoundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GraveMoundMesh"));
    GraveMoundMesh->SetupAttachment(SceneRoot);

    FutureCorpseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FutureCorpseMesh"));
    FutureCorpseMesh->SetupAttachment(SceneRoot);
    FutureCorpseMesh->SetVisibility(false);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(SceneRoot);
    InteractionSphere->SetSphereRadius(200.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

bool ABuriedFutureGraveActor::ExhumeFutureGrave(bool bAnchorLovingFatherMemory)
{
    if (bIsExhumed)
    {
        return false;
    }

    bIsExhumed = true;
    bLovingFatherAnchor = bAnchorLovingFatherMemory;

    // Unearth the future corpse visually
    FutureCorpseMesh->SetVisibility(true);

    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathQuestSubsystem* QuestSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathQuestSubsystem>())
        {
            QuestSys->ResolveBuriedFuture(bAnchorLovingFatherMemory);
        }
    }

    OnGraveExhumed.Broadcast(bAnchorLovingFatherMemory);
    return true;
}

bool ABuriedFutureGraveActor::CanInteract_Implementation(AActor* Interactor) const
{
    return !bIsExhumed && IsValid(Interactor);
}

FText ABuriedFutureGraveActor::GetInteractionPrompt_Implementation(AActor*) const
{
    return FText::FromString(TEXT("[E] Exhume Future Grave"));
}

FText ABuriedFutureGraveActor::GetInteractionTargetName_Implementation() const
{
    return FText::FromString(TEXT("Future-Dated Grave Mound"));
}

bool ABuriedFutureGraveActor::ExecuteInteraction_Implementation(AActor*)
{
    return ExhumeFutureGrave(true);
}
