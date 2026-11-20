#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/EdrathTypes.h"
#include "Act1PreAlphaSliceDirector.generated.h"

class AActor;
class AForensicClueActor;
class ABeliefBuildingActor;
class AKaelCharacter;
class APossibilityShiftActor;
class UBlackClockComponent;
class UEdrathNarrativeSubsystem;

UENUM(BlueprintType)
enum class EPreAlphaZoneState : uint8
{
    Zone1_BattlefieldAwakening,
    Zone2_CollapsedGorgeBridge,
    Zone3_LoopingSettlementExterior,
    Zone4_KaelHouseForensics,
    Zone5_CryptPriestTwist,
    PreAlphaCompleted
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAct1PreAlphaCompleted);

UCLASS()
class EDRATH_API AAct1PreAlphaSliceDirector : public AActor
{
    GENERATED_BODY()

public:
    AAct1PreAlphaSliceDirector();
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    TObjectPtr<AKaelCharacter> KaelCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    TObjectPtr<APossibilityShiftActor> GorgeBridge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    TObjectPtr<APossibilityShiftActor> CryptPortcullis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    TObjectPtr<ABeliefBuildingActor> VillageShrine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    TSubclassOf<AActor> DyingSoldierDialogueTriggerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Dialogue")
    FText DyingSoldierDialogueLine = FText::FromString(TEXT("Do not let my daughter be born"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    TSubclassOf<AActor> BattlefieldWraithClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    int32 BattlefieldWraithCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    FGameplayTag PriestMemoryAnchorTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Actors")
    FName PriestMemoryOwner = TEXT("SettlementPriest");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Evidence")
    FGameplayTag HandwritingEvidenceTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Evidence")
    FGameplayTag ScratchedPortraitEvidenceTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|PreAlpha|Evidence")
    FGameplayTag PriestEvidenceTag;

    UPROPERTY(BlueprintAssignable, Category="Edrath|PreAlpha")
    FOnAct1PreAlphaCompleted OnAct1PreAlphaCompleted;

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void InitializeSlice(AKaelCharacter* InKael, APossibilityShiftActor* InGorgeBridge, ABeliefBuildingActor* InVillageShrine);

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void RegisterDyingSoldierDialogueTriggered();

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void RegisterBattlefieldWraithDefeated();

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    bool RegisterBattlefieldWraithDefeatedUsingEchoCombat(float SecondsAgo = 1.2f);

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    bool AdvanceSettlementDay();

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void RegisterHouseForensicClue(AForensicClueActor* ForensicClue);

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void RegisterCryptPortcullisShift();

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void RegisterPriestCorpseInspected();

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    bool CompletePriestMemoryReconstruction();

    UFUNCTION(BlueprintCallable, Category="Edrath|PreAlpha")
    void RefreshZoneState();

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    EPreAlphaZoneState GetCurrentZone() const { return CurrentZone; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    int32 GetDefeatedBattlefieldWraithCount() const { return DefeatedBattlefieldWraithCount; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    int32 GetAdvancedSettlementDayCount() const { return AdvancedSettlementDayCount; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool HasDyingSoldierDialogueTriggered() const { return bDyingSoldierDialogueTriggered; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool HasCryptPortcullisShifted() const { return bCryptPortcullisShifted; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool HasPriestCorpseBeenInspected() const { return bPriestCorpseInspected; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool HasPriestMemoryBeenReconstructed() const { return bPriestMemoryReconstructed; }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool IsZone1Cleared() const { return bDyingSoldierDialogueTriggered && DefeatedBattlefieldWraithCount >= FMath::Max(0, BattlefieldWraithCount); }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool IsZone2Cleared() const { return static_cast<uint8>(CurrentZone) > static_cast<uint8>(EPreAlphaZoneState::Zone2_CollapsedGorgeBridge); }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool IsZone3Cleared() const { return static_cast<uint8>(CurrentZone) > static_cast<uint8>(EPreAlphaZoneState::Zone3_LoopingSettlementExterior); }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool IsZone4Cleared() const { return static_cast<uint8>(CurrentZone) > static_cast<uint8>(EPreAlphaZoneState::Zone4_KaelHouseForensics); }

    UFUNCTION(BlueprintPure, Category="Edrath|PreAlpha")
    bool IsPreAlphaCompleted() const { return CurrentZone == EPreAlphaZoneState::PreAlphaCompleted; }

protected:
    UFUNCTION()
    void HandleBeliefWeightChanged(FGameplayTag BuildingTag, float NewBeliefWeight);

    UFUNCTION()
    void HandleForensicClueInspected(FGameplayTag EvidenceTag, AActor* Inspector);

    UFUNCTION()
    void HandleCameraPerspectiveChanged(EEdrathCameraMode NewMode, AActor* InteriorVolumeActor);

    virtual void Tick(float DeltaSeconds) override;

private:
    void SpawnZoneOneActors();
    void EvaluateZoneProgression();
    void EnterZone(EPreAlphaZoneState NewZone);
    UEdrathNarrativeSubsystem* GetNarrativeSubsystem() const;
    UBlackClockComponent* GetBlackClockComponent() const;

    EPreAlphaZoneState CurrentZone = EPreAlphaZoneState::Zone1_BattlefieldAwakening;
    int32 DefeatedBattlefieldWraithCount = 0;
    int32 AdvancedSettlementDayCount = 0;
    bool bDyingSoldierDialogueTriggered = false;
    bool bShrineObserved = false;
    bool bHandwritingClueInspected = false;
    bool bScratchedPortraitInspected = false;
    bool bCryptPortcullisShifted = false;
    bool bPriestCorpseInspected = false;
    bool bPriestMemoryReconstructed = false;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedZoneOneActors;

    UPROPERTY()
    TArray<TObjectPtr<AForensicClueActor>> RegisteredHouseClues;
};
