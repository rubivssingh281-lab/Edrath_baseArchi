#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "Act2VeylCourtDirector.generated.h"

class AKaelCharacter;
class ABeliefBuildingActor;

/**
 * The three impossible murder victims sharing the exact same face in Veyl.
 */
UENUM(BlueprintType)
enum class EVeylMurderVictim : uint8
{
    None,
    Beggar,
    General,
    UnbornChild
};

/**
 * Progression state machine for Act II: The Kingdom of Borrowed Faces.
 */
UENUM(BlueprintType)
enum class EAct2VeylState : uint8
{
    ArrivalAtVeylGates,
    QueenSerathAccusation,
    InvestigationOfThreeMurders,
    CourtConspiracyConfrontation,
    PrinceMemoryReconstruction,
    Act2Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct2MurderInspectedSignature, EVeylMurderVictim, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct2StateChangedSignature, EAct2VeylState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrinceTruthRevealedSignature);

/**
 * AAct2VeylCourtDirector
 * Master level director orchestrating Act II in the Kingdom of Veyl.
 * Manages Queen Serath's royal court confrontation, the 3 identical-face murder scene
 * forensic deductions, collective citizen belief modulation, and the Prince's erased memory echo.
 */
UCLASS()
class EDRATH_API AAct2VeylCourtDirector : public AActor
{
    GENERATED_BODY()

public:
    AAct2VeylCourtDirector();

    /** Initializes Act 2 director with references to player and key Veyl actors. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act2")
    void InitializeAct2(AKaelCharacter* InKael, ABeliefBuildingActor* InCathedral);

    /** Registers inspection of one of the three impossible murder scenes. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act2")
    bool InspectMurderScene(EVeylMurderVictim Victim);

    /** Accepts Queen Serath's investigation bargain to clear Kael's name. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act2")
    bool AcceptQueenBargain();

    /** Triggers the memory reconstruction anchor revealing the Prince is still alive. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act2")
    bool RevealPrinceTruth();

    /** Returns whether all three identical-face victims have been forensically examined. */
    UFUNCTION(BlueprintPure, Category="Edrath|Act2")
    bool AreAllMurdersInvestigated() const;

    /** Returns current Act 2 quest progression state. */
    UFUNCTION(BlueprintPure, Category="Edrath|Act2")
    EAct2VeylState GetAct2State() const { return CurrentState; }

    /** Returns whether Act 2 has completed successfully. */
    UFUNCTION(BlueprintPure, Category="Edrath|Act2")
    bool IsAct2Completed() const { return CurrentState == EAct2VeylState::Act2Completed; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Act2|Clues")
    FGameplayTag BeggarClueTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Act2|Clues")
    FGameplayTag GeneralClueTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Act2|Clues")
    FGameplayTag UnbornChildClueTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Act2|Clues")
    FGameplayTag PrinceMemoryTag;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act2")
    FOnAct2MurderInspectedSignature OnMurderInspected;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act2")
    FOnAct2StateChangedSignature OnAct2StateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act2")
    FOnPrinceTruthRevealedSignature OnPrinceTruthRevealed;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<AKaelCharacter> KaelCharacter;

    UPROPERTY()
    TObjectPtr<ABeliefBuildingActor> GrandCathedral;

    EAct2VeylState CurrentState = EAct2VeylState::ArrivalAtVeylGates;
    bool bBeggarInspected = false;
    bool bGeneralInspected = false;
    bool bUnbornChildInspected = false;
    bool bPrinceTruthRevealed = false;
};
