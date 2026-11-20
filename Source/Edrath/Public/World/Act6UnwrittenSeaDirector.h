#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "Act6UnwrittenSeaDirector.generated.h"

class AKaelCharacter;
class AUnwrittenIslandSpawner;
class AFirstArchivistBossCharacter;

/**
 * Progression state machine for Act VI: The World That Forgot Tomorrow.
 */
UENUM(BlueprintType)
enum class EAct6UnwrittenSeaState : uint8
{
    SailingSkyOcean,
    ExploringUnchosenIslands,
    ConfrontingFormerSelves,
    FirstArchivistBattle,
    DecidingFinalPhilosophy,
    Act6Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAct6StateChangedSignature, EAct6UnwrittenSeaState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchivistDefeatedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinalEndingResolvedSignature, EEdrathEndingType, FinalEnding);

/**
 * AAct6UnwrittenSeaDirector
 * Master level director orchestrating Act VI across The Unwritten Sea.
 * Governs aerial sky-ocean navigation, dynamic island materialization of unchosen
 * choices via AUnwrittenIslandSpawner, confrontation with Kael's former selves, the
 * climactic boss encounter against The First Archivist, and the resolution of the Four Endings.
 */
UCLASS()
class EDRATH_API AAct6UnwrittenSeaDirector : public AActor
{
    GENERATED_BODY()

public:
    AAct6UnwrittenSeaDirector();

    /** Initializes the Act VI director with references to Kael and the island spawner. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act6")
    void InitializeAct6(AKaelCharacter* InKael, AUnwrittenIslandSpawner* InIslandSpawner);

    /** Confronts an alternate former self encountered on an unwritten island. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act6")
    bool ConfrontFormerSelf(EKaelLifeEchoArchetype EchoArchetype);

    /** Initiates the climactic boss confrontation against The First Archivist. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act6")
    bool InitiateFirstArchivistBattle(AFirstArchivistBossCharacter* InBoss);

    /** Records the defeat of The First Archivist, dismantling reality-pruning wards. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act6")
    bool RecordArchivistDefeated();

    /** Evaluates and commits the player's final choice among the four philosophical endings. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act6")
    EEdrathEndingType SelectFinalEnding(EEdrathEndingType SelectedPhilosophy);

    UFUNCTION(BlueprintPure, Category="Edrath|Act6")
    EAct6UnwrittenSeaState GetAct6State() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act6")
    bool IsAct6Completed() const { return CurrentState == EAct6UnwrittenSeaState::Act6Completed; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act6")
    bool IsArchivistDefeated() const { return bArchivistDefeated; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act6")
    EEdrathEndingType GetResolvedEnding() const { return ResolvedEnding; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act6")
    int32 GetConfrontedSelvesCount() const { return ConfrontedFormerSelves.Num(); }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act6")
    FOnAct6StateChangedSignature OnAct6StateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act6")
    FOnArchivistDefeatedSignature OnArchivistDefeated;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act6")
    FOnFinalEndingResolvedSignature OnFinalEndingResolved;

protected:
    void SetState(EAct6UnwrittenSeaState NewState);

private:
    UPROPERTY()
    TObjectPtr<AKaelCharacter> KaelCharacter;

    UPROPERTY()
    TObjectPtr<AUnwrittenIslandSpawner> IslandSpawner;

    UPROPERTY()
    TObjectPtr<AFirstArchivistBossCharacter> BossArchivist;

    UPROPERTY()
    EAct6UnwrittenSeaState CurrentState = EAct6UnwrittenSeaState::SailingSkyOcean;

    UPROPERTY()
    TArray<EKaelLifeEchoArchetype> ConfrontedFormerSelves;

    EEdrathEndingType ResolvedEnding = EEdrathEndingType::None;
    bool bArchivistDefeated = false;
};
