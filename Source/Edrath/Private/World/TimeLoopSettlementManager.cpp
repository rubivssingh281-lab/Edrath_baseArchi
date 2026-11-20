#include "World/TimeLoopSettlementManager.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

ATimeLoopSettlementManager::ATimeLoopSettlementManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATimeLoopSettlementManager::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        NarrativeSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>();
    }
}

void ATimeLoopSettlementManager::AdvanceSettlementDay()
{
    if (NarrativeSubsystem.IsValid())
    {
        NarrativeSubsystem->AdvanceSettlementDay();
    }
}

void ATimeLoopSettlementManager::BreakSettlementLoop()
{
    if (NarrativeSubsystem.IsValid())
    {
        NarrativeSubsystem->BreakAct1SettlementLoop();
    }
}

bool ATimeLoopSettlementManager::IsLoopBroken() const
{
    return NarrativeSubsystem.IsValid() && NarrativeSubsystem->IsSettlementLoopBroken();
}
