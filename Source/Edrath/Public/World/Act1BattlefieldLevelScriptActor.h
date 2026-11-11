#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Core/EdrathTypes.h"
#include "Act1BattlefieldLevelScriptActor.generated.h"

class AAct1PreAlphaSliceDirector;
class AKaelCharacter;
class ADyingSoldierActor;
class ABattlefieldWraithCharacter;
class AAct1GorgeBridgeActor;
class ASettlementVillagerCharacter;
class ABeliefBuildingActor;
class AKaelHouseInteriorActor;
class APriestCorpseActor;
class APossibilityShiftActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelZoneTransitionSignature, EEdrathRegion, CurrentRegion);

/**
 * Act 1 Battlefield Level Script Actor
 * Master level orchestration actor for L_PreAlpha_Act1_BattlefieldAndCrypt.
 * Coordinates all five vertical slice zones, dynamic World Partition Data Layers,
 * and reality shifting parameters across the pre-alpha playable slice.
 */
UCLASS()
class EDRATH_API AAct1BattlefieldLevelScriptActor : public ALevelScriptActor
{
    GENERATED_BODY()

public:
    AAct1BattlefieldLevelScriptActor();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Director")
    TObjectPtr<AAct1PreAlphaSliceDirector> SliceDirector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Actors")
    TObjectPtr<AKaelCharacter> PlayerCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone1")
    TObjectPtr<ADyingSoldierActor> DyingSoldier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone1")
    TArray<TObjectPtr<ABattlefieldWraithCharacter>> BattlefieldWraiths;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone2")
    TObjectPtr<AAct1GorgeBridgeActor> GorgeBridge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone3")
    TObjectPtr<ASettlementVillagerCharacter> SettlementVillager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone3")
    TObjectPtr<ABeliefBuildingActor> SettlementShrine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone4")
    TObjectPtr<AKaelHouseInteriorActor> KaelHouse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone5")
    TObjectPtr<APossibilityShiftActor> CryptPortcullis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|PreAlpha|Zone5")
    TObjectPtr<APriestCorpseActor> PriestCorpse;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|PreAlpha|State")
    bool bSliceInitialized = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|PreAlpha|State")
    bool bAct1Completed = false;

    UFUNCTION(BlueprintCallable, Category = "Edrath|PreAlpha")
    void AssembleVerticalSlice();

    UFUNCTION(BlueprintPure, Category = "Edrath|PreAlpha")
    bool IsSliceComplete() const { return bAct1Completed; }

protected:
    UFUNCTION()
    void HandleAct1Completed();
};
