#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "Act3HollowgreenDirector.generated.h"

class AKaelCharacter;
class AMemoryFruitTreeActor;
class AMemoryAnchorActor;

/**
 * Progression state machine for Act III: The Forest of Your Other Lives.
 */
UENUM(BlueprintType)
enum class EAct3HollowgreenState : uint8
{
    ArrivalAtHollowgreen,
    ExploringMemoryRealms,
    HealerEchoDiscovered,
    TyrantEchoDiscovered,
    FarmerEchoDiscovered,
    IlyraConfrontation,
    Act3Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct3EchoDiscoveredSignature, EKaelLifeEchoArchetype, EchoArchetype);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct3StateChangedSignature, EAct3HollowgreenState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIlyraLoopConfrontationSignature, bool, bLoopAttemptsRevealed);

/**
 * AAct3HollowgreenDirector
 * Master level director orchestrating Act III in the Hollowgreen Forest.
 * Manages physical memory fruit realm streaming, Kael's three alternate life echoes
 * (Plagued Healer, Tyrant Conqueror, Simple Farmer), memory anchor mutations, and the
 * confrontation with Ilyra (The Woman Who Remembers Killing You).
 */
UCLASS()
class EDRATH_API AAct3HollowgreenDirector : public AActor
{
    GENERATED_BODY()

public:
    AAct3HollowgreenDirector();

    /** Initializes the director with references to Kael and the memory tree. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act3")
    void InitializeAct3(AKaelCharacter* InKael, AMemoryFruitTreeActor* InMemoryTree);

    /** Registers discovery and exploration of one of Kael's alternate life echoes. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act3")
    bool RegisterEchoDiscovery(EKaelLifeEchoArchetype EchoArchetype);

    /** Executes the dramatic narrative confrontation with Ilyra once all 3 echoes are understood. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act3")
    bool ConfrontIlyra(bool bAcceptTruth);

    UFUNCTION(BlueprintPure, Category="Edrath|Act3")
    EAct3HollowgreenState GetAct3State() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act3")
    bool HasDiscoveredEcho(EKaelLifeEchoArchetype EchoArchetype) const;

    UFUNCTION(BlueprintPure, Category="Edrath|Act3")
    int32 GetDiscoveredEchoCount() const { return DiscoveredEchoes.Num(); }

    UFUNCTION(BlueprintPure, Category="Edrath|Act3")
    bool IsAct3Completed() const { return CurrentState == EAct3HollowgreenState::Act3Completed; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act3")
    FOnAct3EchoDiscoveredSignature OnEchoDiscovered;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act3")
    FOnAct3StateChangedSignature OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act3")
    FOnIlyraLoopConfrontationSignature OnIlyraConfrontationCompleted;

protected:
    void SetState(EAct3HollowgreenState NewState);

private:
    UPROPERTY()
    TObjectPtr<AKaelCharacter> KaelCharacter;

    UPROPERTY()
    TObjectPtr<AMemoryFruitTreeActor> ActiveMemoryTree;

    UPROPERTY()
    EAct3HollowgreenState CurrentState = EAct3HollowgreenState::ArrivalAtHollowgreen;

    UPROPERTY()
    TArray<EKaelLifeEchoArchetype> DiscoveredEchoes;

    bool bIlyraConfronted = false;
};
