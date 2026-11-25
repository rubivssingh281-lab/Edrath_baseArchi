#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Core/EdrathTypes.h"
#include "World/EdrathWeatherSubsystem.h"
#include "EdrathCompanionAIController.generated.h"

class AKaelCharacter;
class AEdrathCompanionCharacter;

UENUM(BlueprintType)
enum class ECompanionBehaviorMode : uint8
{
    FollowingHero,
    InvestigatingEvidence,
    CombatSupport,
    SuspiciousObservation,
    HostileDefection
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompanionBehaviorChangedSignature, ECompanionBehaviorMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionSuspicionEscalatedSignature, float, CurrentSuspicion, bool, bIsDefecting);

/**
 * AEdrathCompanionAIController
 * Autonomous AI controller governing active companions (Ilyra, Orren, Senn, Maevra).
 * Adapts follow behavior, combat support stances, investigative curiosity, and suspicion
 * escalation in response to Kael's Black Clock reality manipulations.
 */
UCLASS()
class EDRATH_API AEdrathCompanionAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEdrathCompanionAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|CompanionAI")
    void SetBehaviorMode(ECompanionBehaviorMode NewMode);

    /** Called when Kael activates the Black Clock, escalating companion suspicion. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CompanionAI")
    void ReactToBlackClockUsage(float SuspicionDelta = 8.0f);

    /** Called when Senn signals incoming attack foresight (0.8s reaction window). */
    UFUNCTION(BlueprintCallable, Category="Edrath|CompanionAI")
    void ReactToForesightWarning(float WarningSeconds);

    /** Capitalizes on a poise-shattered target by entering CombatSupport for a synchronized follow-up. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CompanionAI")
    void ReactToEnemyPoiseShatter(AActor* StaggeredTarget, float StaggerDuration);

    /** Tightens companion follow radius during low-visibility storms and possibility fog. */
    UFUNCTION(BlueprintCallable, Category="Edrath|CompanionAI")
    void AdaptFollowBehaviorToWeather(EEdrathWeatherType ActiveWeather, float VisibilityDistance);

    UFUNCTION(BlueprintPure, Category="Edrath|CompanionAI")
    AActor* GetAssistTargetActor() const { return AssistTargetActor.Get(); }

    UFUNCTION(BlueprintPure, Category="Edrath|CompanionAI")
    ECompanionBehaviorMode GetBehaviorMode() const { return CurrentMode; }

    UFUNCTION(BlueprintPure, Category="Edrath|CompanionAI")
    AEdrathCompanionCharacter* GetCompanionCharacter() const;

    UFUNCTION(BlueprintPure, Category="Edrath|CompanionAI")
    float GetCurrentSuspicion() const { return SuspicionLevel; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|CompanionAI")
    float FollowDistance = 280.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|CompanionAI")
    float MaxCombatAssistanceRadius = 750.0f;

    UPROPERTY(BlueprintAssignable, Category="Edrath|CompanionAI")
    FOnCompanionBehaviorChangedSignature OnBehaviorChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|CompanionAI")
    FOnCompanionSuspicionEscalatedSignature OnSuspicionEscalated;

protected:
    UPROPERTY()
    TObjectPtr<AEdrathCompanionCharacter> ControlledCompanion;

    UPROPERTY()
    TObjectPtr<AKaelCharacter> ObservedHero;

    ECompanionBehaviorMode CurrentMode = ECompanionBehaviorMode::FollowingHero;
    float SuspicionLevel = 10.0f;
    float ForesightAnticipationTimer = 0.0f;
    TWeakObjectPtr<AActor> AssistTargetActor;
};
