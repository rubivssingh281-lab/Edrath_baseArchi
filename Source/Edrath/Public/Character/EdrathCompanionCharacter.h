#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "EdrathCompanionCharacter.generated.h"

UENUM(BlueprintType)
enum class EIlyraResolution : uint8
{
    None,
    Reconciliation,
    Imprisonment,
    LethalConfrontation
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIlyraResolutionChanged, EIlyraResolution, Resolution);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSennForesightWarning, float, WarningSeconds);

UCLASS()
class EDRATH_API AEdrathCompanionCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEdrathCompanionCharacter();

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion")
    void SetAutonomousState(float NewLoyalty, float NewSuspicion, bool bNewWithholdingTruth);

    UFUNCTION(BlueprintPure, Category="Edrath|Companion")
    FCompanionRelationshipState GetRelationshipState() const { return RelationshipState; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Ilyra")
    bool ResolveIlyraOutcome(EIlyraResolution NewResolution);

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Ilyra")
    bool GenerateIlyraBladeNotchClue();

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Ilyra")
    EIlyraResolution GetIlyraResolution() const { return IlyraResolutionState; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Orren")
    bool AssistMemoryReconstruction(FGameplayTag MemoryTag, FText OverwrittenDescription);

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Orren")
    bool HasOverwrittenHistoricalEvidence() const { return bHasOverwrittenHistoricalEvidence; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Senn")
    void TriggerForesightWarning(float WarningSeconds = 0.8f);

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Senn")
    float GetLastForesightWarningSeconds() const { return LastForesightWarningSeconds; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Senn")
    void SetActVIChoiceKnowledge(FText ChoiceKnowledge);

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Senn")
    bool HasActVIChoiceKnowledge() const { return bKnowsActVIChoice; }

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Senn")
    FText GetActVIChoiceKnowledge() const { return ActVIChoiceKnowledge; }

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Maevra")
    void SetHostilePursuer(bool bNewHostilePursuer);

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Maevra")
    bool RecruitMaevraToParty();

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion|Maevra")
    void SetErasedMarriageRestorationProgress(int32 RecoveredMemoryCount, int32 RequiredMemoryCount = 4);

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Maevra")
    bool IsHostilePursuer() const { return bIsHostilePursuer; }

    UFUNCTION(BlueprintPure, Category="Edrath|Companion|Maevra")
    bool IsPartyMember() const { return bIsPartyMember; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Companion|Ilyra")
    FOnIlyraResolutionChanged OnIlyraResolutionChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Companion|Senn")
    FOnSennForesightWarning OnSennForesightWarning;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companion")
    FName CompanionID = NAME_None;

protected:
    virtual void BeginPlay() override;

private:
    FCompanionRelationshipState RelationshipState;
    EIlyraResolution IlyraResolutionState = EIlyraResolution::None;
    bool bHasOverwrittenHistoricalEvidence = false;
    float LastForesightWarningSeconds = 0.0f;
    bool bKnowsActVIChoice = false;
    FText ActVIChoiceKnowledge;
    bool bIsHostilePursuer = false;
    bool bIsPartyMember = false;
    int32 RecoveredMarriageMemoryCount = 0;
    int32 RequiredMarriageMemoryCount = 4;
};
