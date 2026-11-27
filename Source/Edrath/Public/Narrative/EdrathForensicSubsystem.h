#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Core/EdrathTypes.h"
#include "Core/EdrathSaveGame.h"
#include "EdrathForensicSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClueMatrixDiscovered, FGameplayTag, ClueTag, EEdrathRegion, Region);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBreakthroughMatrixUnlocked, FGameplayTag, BreakthroughTag, FGameplayTag, ClueA, FGameplayTag, ClueB);

/**
 * UEdrathForensicSubsystem
 * GameInstance subsystem managing global forensic clue states,
 * cross-region contradiction resolution matrices, and save game persistence.
 */
UCLASS()
class EDRATH_API UEdrathForensicSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Registers a newly examined forensic clue into the global matrix. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    bool RegisterDiscoveredClue(FGameplayTag ClueTag, EEdrathRegion Region, const FText& ObservationNotes);

    /** Transitions a clue to analyzed status after player inspects it in the Forensic Ledger. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    bool MarkClueAnalyzed(FGameplayTag ClueTag);

    /** Commits a validated contradiction breakthrough between two conflicting clues. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    bool CommitBreakthrough(FGameplayTag BreakthroughTag, FGameplayTag ClueA, FGameplayTag ClueB);

    /** Checks whether a specific narrative contradiction breakthrough has been unlocked. */
    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    bool IsBreakthroughUnlocked(FGameplayTag BreakthroughTag) const;

    /** Retrieves the current investigation status of a clue. */
    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    EEdrathClueInvestigationStatus GetClueStatus(FGameplayTag ClueTag) const;

    /** Returns all discovered clues associated with a specific region. */
    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    TArray<FEdrathClueMatrixNode> GetCluesForRegion(EEdrathRegion Region) const;

    /** Returns total count of all discovered clues across all acts. */
    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    int32 GetDiscoveredClueCount() const { return ClueMatrix.Num(); }

    /** Returns total count of unlocked narrative breakthroughs. */
    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    int32 GetUnlockedBreakthroughCount() const { return UnlockedBreakthroughs.Num(); }

    /** Serializes all discovered clues and breakthroughs into a SaveGame object. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    void SerializeToSaveGame(UEdrathSaveGame* SaveGame) const;

    /** Restores forensic matrix state from a loaded SaveGame object. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    void DeserializeFromSaveGame(const UEdrathSaveGame* SaveGame);

    UPROPERTY(BlueprintAssignable, Category="Edrath|Forensics")
    FOnClueMatrixDiscovered OnClueDiscovered;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Forensics")
    FOnBreakthroughMatrixUnlocked OnBreakthroughUnlocked;

private:
    UPROPERTY()
    TMap<FGameplayTag, FEdrathClueMatrixNode> ClueMatrix;

    UPROPERTY()
    TArray<FGameplayTag> UnlockedBreakthroughs;
};
