#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "Act5CrownOfWinterDirector.generated.h"

class AKaelCharacter;
class AEdrathInteriorTransitionVolume;

/**
 * Progression state machine for Act V: The God Who Asked to Die.
 */
UENUM(BlueprintType)
enum class EAct5CrownOfWinterState : uint8
{
    AscentToCrownOfWinter,
    TimeDilatedChamberTraversed,
    ConsciousArchiveInfiltrated,
    ArchiveDialogueConducted,
    OntologicalChoicePresented,
    Act5Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct5StateChangedSignature, EAct5CrownOfWinterState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeDilationSimulatedSignature, float, SecondsIndoors, int32, ElapsedExteriorDays);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArchiveRequestSignature, bool, bAgreesWithRequest);

/**
 * AAct5CrownOfWinterDirector
 * Master level director orchestrating Act V in The Crown of Winter (The Place Outside Time).
 * Governs the frozen mountain traversal, exterior time-dilation chambers (seconds inside
 * equal years outside), entry into the Conscious Archive, dialogue with the world's conscious
 * memory machine, and the ontological choice to preserve or dismantle reality pruning.
 */
UCLASS()
class EDRATH_API AAct5CrownOfWinterDirector : public AActor
{
    GENERATED_BODY()

public:
    AAct5CrownOfWinterDirector();

    /** Initializes the Act V director with references to Kael and the time dilated interior volume. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act5")
    void InitializeAct5(AKaelCharacter* InKael, AEdrathInteriorTransitionVolume* InChamberVolume);

    /** Simulates the passage of exterior time when Kael enters a time-dilated Crown of Winter interior. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act5")
    int32 SimulateInteriorTimeDilation(float SecondsIndoors);

    /** Marks the entry into the sanctum of the Conscious Archive. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act5")
    bool InfiltrateConsciousArchive();

    /** Conducts the conversation with the Conscious Archive where it asks Kael to end its existence. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act5")
    bool ConductArchiveDialogue(bool bAcknowledgeRequest);

    /** Commits the player's philosophical stance toward the Archive before sailing into Act VI. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act5")
    bool CommitOntologicalChoice(bool bAgreeToDestroyArchive);

    UFUNCTION(BlueprintPure, Category="Edrath|Act5")
    EAct5CrownOfWinterState GetAct5State() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act5")
    bool IsAct5Completed() const { return CurrentState == EAct5CrownOfWinterState::Act5Completed; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act5")
    bool HasAcknowledgedArchiveRequest() const { return bArchiveRequestAcknowledged; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act5")
    int32 GetTotalElapsedExteriorDays() const { return TotalElapsedExteriorDays; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act5")
    FOnAct5StateChangedSignature OnAct5StateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act5")
    FOnTimeDilationSimulatedSignature OnTimeDilationSimulated;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act5")
    FOnArchiveRequestSignature OnArchiveRequestReceived;

protected:
    void SetState(EAct5CrownOfWinterState NewState);

private:
    UPROPERTY()
    TObjectPtr<AKaelCharacter> KaelCharacter;

    UPROPERTY()
    TObjectPtr<AEdrathInteriorTransitionVolume> TimeDilatedChamber;

    UPROPERTY()
    EAct5CrownOfWinterState CurrentState = EAct5CrownOfWinterState::AscentToCrownOfWinter;

    int32 TotalElapsedExteriorDays = 0;
    bool bArchiveInfiltrated = false;
    bool bArchiveRequestAcknowledged = false;
    bool bChoiceCommitted = false;
};
