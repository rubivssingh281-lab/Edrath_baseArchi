#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "Act4NhalDirector.generated.h"

class AKaelCharacter;
class ANhalFactionManager;
class AEdrathCompanionCharacter;

/**
 * Progression state machine for Act IV: The Dead Who Remember Tomorrow.
 */
UENUM(BlueprintType)
enum class EAct4NhalState : uint8
{
    ArrivalAtSubterraneanNhal,
    InvestigatingFactionStrife,
    DeadChildProphecyReceived,
    CompanionBetrayalAverted,
    ArchiveLedgerTwistUncovered,
    Act4Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct4StateChangedSignature, EAct4NhalState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadChildProphecySignature, const FText&, ProphecyText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionBetrayalResolvedSignature, FName, CompanionId, bool, bReconciled);

/**
 * AAct4NhalDirector
 * Master level director orchestrating Act IV in the subterranean city of Nhal.
 * Governs the multi-tiered cave world partition grid, the war between Restorationists
 * and Revanchists, future-dead companion echo manifestations, the dead child prophecy,
 * ally hostility defusal, and Twist 3 (erased citizens being warmongers whose resurrection
 * despawns living families in Veyl).
 */
UCLASS()
class EDRATH_API AAct4NhalDirector : public AActor
{
    GENERATED_BODY()

public:
    AAct4NhalDirector();

    /** Initializes the Act IV director with references to Kael and Nhal faction manager. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act4")
    void InitializeAct4(AKaelCharacter* InKael, ANhalFactionManager* InFactionManager);

    /** Triggers the Dead Child encounter where a future-dead child reveals how Kael's journey ends. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act4")
    bool TriggerDeadChildProphecy(const FText& ProphecyText);

    /** Resolves the crisis where an ally threatens hostility because her future memory shows Kael betraying her. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act4")
    bool ResolveCompanionBetrayal(FName CompanionId, bool bReconcileWithAlly);

    /** Uncovers Twist 3 through the Nhal Archive ledger: restored citizens despawn living families. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act4")
    bool UncoverArchiveLedgerTwist();

    /** Adjusts faction balance between Restorationists and Revanchists. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act4")
    void AdjustFactionStanding(FGameplayTag FactionTag, int32 Delta);

    UFUNCTION(BlueprintPure, Category="Edrath|Act4")
    EAct4NhalState GetAct4State() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act4")
    bool IsAct4Completed() const { return CurrentState == EAct4NhalState::Act4Completed; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act4")
    bool HasReceivedProphecy() const { return bProphecyReceived; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act4")
    bool IsBetrayalAverted() const { return bBetrayalAverted; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act4")
    bool IsLedgerTwistRevealed() const { return bLedgerTwistRevealed; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act4")
    FOnAct4StateChangedSignature OnAct4StateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act4")
    FOnDeadChildProphecySignature OnDeadChildProphecyReceived;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act4")
    FOnCompanionBetrayalResolvedSignature OnCompanionBetrayalResolved;

protected:
    void SetState(EAct4NhalState NewState);

private:
    UPROPERTY()
    TObjectPtr<AKaelCharacter> KaelCharacter;

    UPROPERTY()
    TObjectPtr<ANhalFactionManager> FactionManager;

    UPROPERTY()
    EAct4NhalState CurrentState = EAct4NhalState::ArrivalAtSubterraneanNhal;

    FText CachedProphecyText;
    bool bProphecyReceived = false;
    bool bBetrayalAverted = false;
    bool bLedgerTwistRevealed = false;
};
