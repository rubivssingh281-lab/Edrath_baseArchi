#include "World/TombSarcophagusActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

ATombSarcophagusActor::ATombSarcophagusActor()
{
    PrimaryActorTick.bCanEverTick = false;
    SarcophagusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SarcophagusMesh"));
    RootComponent = SarcophagusMesh;
}

void ATombSarcophagusActor::BeginPlay()
{
    Super::BeginPlay();
    CurrentLivingNpcDialogue = FText::GetEmpty();
    if (GetWorld() && GetWorld()->GetGameInstance() && LivingNpcId.IsValid())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RegisterLivingNpcMemory(LivingNpcId, FText::FromString(TEXT("The living NPC remembers an ordinary life.")));
        }
    }
}

bool ATombSarcophagusActor::BindLivingNpc(AActor* InLivingNpc, FGuid InLivingNpcId)
{
    if (!InLivingNpc || !InLivingNpcId.IsValid())
    {
        return false;
    }
    LivingNpc = InLivingNpc;
    LivingNpcId = InLivingNpcId;
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RegisterLivingNpcMemory(LivingNpcId, FText::FromString(TEXT("The living NPC remembers an ordinary life.")));
        }
    }
    return true;
}

bool ATombSarcophagusActor::DisturbTomb(AActor* DisturbingActor)
{
    if (bHasBeenDisturbed || !DisturbingActor || !LivingNpcId.IsValid() || !GetWorld())
    {
        return false;
    }

    UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()
        ? GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>()
        : nullptr;
    if (!Narrative)
    {
        return false;
    }

    bHasBeenDisturbed = true;
    CurrentLivingNpcDialogue = DisturbedDialogue;
    Narrative->MutateLivingNpcMemory(LivingNpcId, DisturbedMemoryDescription);
    OnTombDisturbed.Broadcast(LivingNpcId);
    return true;
}
