#include "World/NhalFactionManager.h"
#include "BlackClock/BlackClockComponent.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ANhalFactionManager::ANhalFactionManager()
{
    PrimaryActorTick.bCanEverTick = false;
    RestorationistsFactionTag = EdrathGameplayTags::FactionNhalRestorationists;
    RevanchistsFactionTag = EdrathGameplayTags::FactionNhalRevanchists;
}

void ANhalFactionManager::SetFactionReputation(FGameplayTag FactionTag, int32 NewReputation)
{
    if (FactionTag.IsValid())
    {
        FactionReputation.Add(FactionTag, FMath::Clamp(NewReputation, -100, 100));
    }
}

void ANhalFactionManager::AdjustFactionReputation(FGameplayTag FactionTag, int32 Delta)
{
    SetFactionReputation(FactionTag, GetFactionReputation(FactionTag) + Delta);
}

int32 ANhalFactionManager::GetFactionReputation(FGameplayTag FactionTag) const
{
    return FactionReputation.FindRef(FactionTag);
}

AActor* ANhalFactionManager::SpawnFutureDeadCompanionEcho(FName CompanionId, FVector SpawnLocation)
{
    if (!GetWorld() || !FutureDeadCompanionEchoClass || CompanionId.IsNone())
    {
        return nullptr;
    }

    AActor* Echo = GetWorld()->SpawnActor<AActor>(FutureDeadCompanionEchoClass, SpawnLocation, FRotator::ZeroRotator);
    if (Echo)
    {
        Echo->Tags.AddUnique(FName(TEXT("FutureDeadEcho")));
        Echo->Tags.AddUnique(CompanionId);
        SpawnedCompanionEchoes.Add(Echo);
    }
    return Echo;
}

int32 ANhalFactionManager::SpawnActiveCompanionEchoes(const TArray<FName>& CompanionIds, FVector Origin, float Spacing)
{
    int32 SpawnedCount = 0;
    for (int32 Index = 0; Index < CompanionIds.Num(); ++Index)
    {
        if (SpawnFutureDeadCompanionEcho(CompanionIds[Index], Origin + FVector(Index * Spacing, 0.0f, 0.0f)))
        {
            ++SpawnedCount;
        }
    }
    return SpawnedCount;
}

bool ANhalFactionManager::RestoreNhalCitizen(FName CitizenId)
{
    if (CitizenId.IsNone() || RestoredCitizens.Contains(CitizenId))
    {
        return false;
    }

    RestoredCitizens.Add(CitizenId);
    ++RestoredCitizenCount;
    if (!bTwistThreeTriggered)
    {
        bTwistThreeTriggered = true;
        for (AActor* FamilyActor : LivingVeylFamilyActors)
        {
            if (FamilyActor)
            {
                FamilyActor->Destroy();
            }
        }
        LivingVeylFamilyActors.Reset();
    }
    return true;
}

bool ANhalFactionManager::RestoreNhalCitizenViaBlackClock(FName CitizenId, AActor* Restorer)
{
    if (!Restorer)
    {
        return false;
    }
    if (UBlackClockComponent* BlackClock = Restorer->FindComponentByClass<UBlackClockComponent>())
    {
        if (BlackClock->GetActiveTimelineState() == ETimelineState::MemoryReconstruction)
        {
            return RestoreNhalCitizen(CitizenId);
        }
    }
    return false;
}
