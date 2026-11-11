#include "World/Act1BattlefieldLevelScriptActor.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "Character/KaelCharacter.h"
#include "World/DyingSoldierActor.h"
#include "Character/BattlefieldWraithCharacter.h"
#include "World/Act1GorgeBridgeActor.h"
#include "Character/SettlementVillagerCharacter.h"
#include "World/BeliefBuildingActor.h"
#include "World/KaelHouseInteriorActor.h"
#include "World/PriestCorpseActor.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AAct1BattlefieldLevelScriptActor::AAct1BattlefieldLevelScriptActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bSliceInitialized = false;
    bAct1Completed = false;
}

void AAct1BattlefieldLevelScriptActor::BeginPlay()
{
    Super::BeginPlay();
    AssembleVerticalSlice();
}

void AAct1BattlefieldLevelScriptActor::AssembleVerticalSlice()
{
    if (bSliceInitialized || !GetWorld())
    {
        return;
    }

    // 1. Locate or validate Kael
    if (!PlayerCharacter)
    {
        PlayerCharacter = Cast<AKaelCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    }

    // 2. Locate or spawn Director
    if (!SliceDirector)
    {
        AActor* FoundDirector = UGameplayStatics::GetActorOfClass(GetWorld(), AAct1PreAlphaSliceDirector::StaticClass());
        if (FoundDirector)
        {
            SliceDirector = Cast<AAct1PreAlphaSliceDirector>(FoundDirector);
        }
        else
        {
            SliceDirector = GetWorld()->SpawnActor<AAct1PreAlphaSliceDirector>();
        }
    }

    if (SliceDirector)
    {
        SliceDirector->OnAct1PreAlphaCompleted.AddDynamic(this, &AAct1BattlefieldLevelScriptActor::HandleAct1Completed);

        // Bind Zone 1
        if (DyingSoldier)
        {
            DyingSoldier->BindToDirector(SliceDirector);
        }

        for (ABattlefieldWraithCharacter* Wraith : BattlefieldWraiths)
        {
            if (Wraith)
            {
                Wraith->BindToDirector(SliceDirector);
            }
        }

        // Bind Zone 2
        if (GorgeBridge)
        {
            GorgeBridge->BindToDirector(SliceDirector);
            SliceDirector->GorgeBridge = GorgeBridge;
        }

        // Bind Zone 3
        if (SettlementShrine)
        {
            SliceDirector->VillageShrine = SettlementShrine;
        }

        // Bind Zone 4
        if (KaelHouse)
        {
            KaelHouse->BindToDirector(SliceDirector);
        }

        // Bind Zone 5
        if (CryptPortcullis)
        {
            SliceDirector->CryptPortcullis = CryptPortcullis;
        }

        if (PriestCorpse)
        {
            PriestCorpse->BindToDirector(SliceDirector);
        }

        SliceDirector->InitializeSlice(PlayerCharacter, GorgeBridge, SettlementShrine);
    }

    bSliceInitialized = true;
}

void AAct1BattlefieldLevelScriptActor::HandleAct1Completed()
{
    bAct1Completed = true;
}
