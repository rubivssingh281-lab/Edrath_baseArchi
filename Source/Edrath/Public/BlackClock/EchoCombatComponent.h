#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "EchoCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualGhostStrikeRequested, const FEchoActionFrame&, EchoFrame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatActionStateChanged, ECombatActionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuccessfulParry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEchoGhostSweepHit, AActor*, HitActor, float, DamageDealt);

UCLASS(ClassGroup=(EdrathBlackClock), meta=(BlueprintSpawnableComponent))
class EDRATH_API UEdrathEchoCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEdrathEchoCombatComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|Combat")
    bool ExecuteAttackCombo(int32 ComboIndex, float BaseDamage);

    UFUNCTION(BlueprintCallable, Category="Edrath|Combat")
    bool ExecuteDodgeRoll(float InDodgeDuration = 0.45f);

    UFUNCTION(BlueprintCallable, Category="Edrath|Combat")
    bool ExecuteParry(float InParryWindow = 0.25f);

    UFUNCTION(BlueprintCallable, Category="Edrath|Combat")
    bool CheckAndResolveIncomingHit(float IncomingDamage, AActor* Attacker);

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock|EchoCombat")
    void SpawnVisualGhostStrike(const FEchoActionFrame& EchoFrame);

    /** Performs multi-hit sphere sweep trace for the temporal ghost strike, dealing echo damage */
    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock|EchoCombat")
    int32 PerformGhostStrikePhysicsSweep(const FEchoActionFrame& EchoFrame, float SweepRadius = 90.0f, float ReachDistance = 140.0f);

    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock|EchoCombat")
    int32 GetLastGhostHitCount() const { return LastGhostHitCount; }

    UFUNCTION(BlueprintPure, Category="Edrath|Combat")
    ECombatActionState GetCurrentCombatActionState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category="Edrath|Combat")
    bool IsInvulnerable() const { return CurrentState == ECombatActionState::Dodging; }

    UFUNCTION(BlueprintPure, Category="Edrath|Combat")
    bool IsInParryWindow() const { return CurrentState == ECombatActionState::Parrying; }

    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock|EchoCombat")
    const FEchoActionFrame& GetLastGhostStrike() const { return LastGhostStrike; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|BlackClock|EchoCombat")
    FOnVisualGhostStrikeRequested OnVisualGhostStrikeRequested;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Combat")
    FOnCombatActionStateChanged OnCombatActionStateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Combat")
    FOnSuccessfulParry OnSuccessfulParry;

    UPROPERTY(BlueprintAssignable, Category="Edrath|BlackClock|EchoCombat")
    FOnEchoGhostSweepHit OnEchoGhostSweepHit;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Combat")
    float DodgeStaminaCost = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Combat")
    float AttackStaminaCost = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Combat")
    float ParryStabilityReward = 15.0f;

private:
    ECombatActionState CurrentState = ECombatActionState::Idle;
    float StateTimeRemaining = 0.0f;
    FEchoActionFrame LastGhostStrike;
    int32 LastGhostHitCount = 0;

    void SetCombatState(ECombatActionState NewState, float Duration);
};
