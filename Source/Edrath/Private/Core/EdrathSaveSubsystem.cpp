#include "Core/EdrathSaveSubsystem.h"
#include "Character/KaelCharacter.h"
#include "BlackClock/BlackClockComponent.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UEdrathSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ActiveSaveData = NewObject<UEdrathSaveGame>(this);
}

bool UEdrathSaveSubsystem::DoesSaveSlotExist(const FString& SlotName, int32 UserIndex) const
{
    return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

bool UEdrathSaveSubsystem::DeleteSaveSlot(const FString& SlotName, int32 UserIndex)
{
    return UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
}

bool UEdrathSaveSubsystem::SaveGameToSlot(const FString& SlotName, int32 UserIndex)
{
    UEdrathSaveGame* SaveObj = Cast<UEdrathSaveGame>(UGameplayStatics::CreateSaveGameObject(UEdrathSaveGame::StaticClass()));
    if (!SaveObj)
    {
        if (OnSaveGameCompleted.IsBound())
        {
            OnSaveGameCompleted.Broadcast(false, SlotName);
        }
        return false;
    }

    SaveObj->SaveSlotName = SlotName;
    SaveObj->UserIndex = UserIndex;
    SaveObj->SaveTimestamp = FDateTime::UtcNow();

    PopulateSaveFromWorld(SaveObj);

    const bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveObj, SlotName, UserIndex);
    if (bSuccess)
    {
        ActiveSaveData = SaveObj;
    }

    if (OnSaveGameCompleted.IsBound())
    {
        OnSaveGameCompleted.Broadcast(bSuccess, SlotName);
    }

    return bSuccess;
}

bool UEdrathSaveSubsystem::LoadGameFromSlot(const FString& SlotName, int32 UserIndex)
{
    if (!DoesSaveSlotExist(SlotName, UserIndex))
    {
        if (OnLoadGameCompleted.IsBound())
        {
            OnLoadGameCompleted.Broadcast(false, SlotName);
        }
        return false;
    }

    USaveGame* LoadedBase = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
    UEdrathSaveGame* LoadedSave = Cast<UEdrathSaveGame>(LoadedBase);
    if (!LoadedSave)
    {
        if (OnLoadGameCompleted.IsBound())
        {
            OnLoadGameCompleted.Broadcast(false, SlotName);
        }
        return false;
    }

    ActiveSaveData = LoadedSave;
    ApplySaveToWorld(LoadedSave);

    if (OnLoadGameCompleted.IsBound())
    {
        OnLoadGameCompleted.Broadcast(true, SlotName);
    }

    return true;
}

void UEdrathSaveSubsystem::PopulateSaveFromWorld(UEdrathSaveGame* SaveObject)
{
    if (!SaveObject)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (World)
    {
        if (AKaelCharacter* Kael = Cast<AKaelCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0)))
        {
            SaveObject->PlayerWorldPosition = Kael->GetActorLocation();
            SaveObject->PlayerWorldRotation = Kael->GetActorRotation();
            SaveObject->PlayerHealth = Kael->CurrentHealth;
            SaveObject->PlayerMaxHealth = Kael->MaxHealth;
            SaveObject->CurrentRegion = Kael->CurrentRegion;

            if (Kael->BlackClockComponent)
            {
                SaveObject->ClockStability = Kael->BlackClockComponent->GetClockStability();
            }
        }
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            SaveObject->CurrentSettlementLoopDay = Narrative->GetCurrentSettlementLoopDay();
            SaveObject->bAct1LoopBroken = Narrative->IsSettlementLoopBroken();
            SaveObject->RegionalInstabilityMap = Narrative->GetRegionalInstabilityMap();
            SaveObject->EvidenceRegistry = Narrative->GetAllEvidenceRecords();
            SaveObject->CompanionRelationshipMap = Narrative->GetCompanionStates();
            SaveObject->UnchosenPossibilityTags = Narrative->GetUnchosenPossibilityTags();
            SaveObject->ClockUsagePrunedFuturesCount = Narrative->GetPrunedFuturesCount();
            SaveObject->bUnlockedFourthPath = Narrative->HasUnlockedFourthPath();
        }
    }
}

void UEdrathSaveSubsystem::ApplySaveToWorld(const UEdrathSaveGame* SaveObject)
{
    if (!SaveObject)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (World)
    {
        if (AKaelCharacter* Kael = Cast<AKaelCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0)))
        {
            Kael->SetActorLocationAndRotation(SaveObject->PlayerWorldPosition, SaveObject->PlayerWorldRotation, false, nullptr, ETeleportType::TeleportPhysics);
            Kael->CurrentHealth = SaveObject->PlayerHealth;
            Kael->MaxHealth = SaveObject->PlayerMaxHealth;
            Kael->CurrentRegion = SaveObject->CurrentRegion;
        }
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RestoreSettlementLoopState(SaveObject->CurrentSettlementLoopDay, SaveObject->bAct1LoopBroken);
            Narrative->SetRegionalInstabilityMap(SaveObject->RegionalInstabilityMap);
            Narrative->RestoreEvidenceRecords(SaveObject->EvidenceRegistry);
            Narrative->SetCompanionStates(SaveObject->CompanionRelationshipMap);
        }
    }
}
