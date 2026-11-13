#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "BlackClockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTimelineStateShifted, ETimelineState, NewState, FVector, Epicenter, float, WaveRadius);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEchoGhostSpawned, const FEchoActionFrame&, EchoFrame);

UCLASS(ClassGroup=(EdrathBlackClock), meta=(BlueprintSpawnableComponent))
class EDRATH_API UBlackClockComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBlackClockComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock")
    bool TogglePossibilityShift(EEdrathRegion CurrentRegion);

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock")
    bool EnterMemoryReconstruction(FGameplayTag MemoryAnchorTag, FName OriginalMemoryOwnerNPC);

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock|EchoCombat")
    void RecordCombatFrame(FName MontageSection, float BaseDamage);

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock|EchoCombat")
    bool TriggerEchoStrike(float SecondsAgo, EEdrathRegion CurrentRegion, FEchoActionFrame& OutEchoFrame);

    /** Sub-frame linear/Hermite interpolation between recorded frames based on continuous target timestamp. */
    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock|EchoCombat")
    bool InterpolateFrameAtTime(float TargetTimestamp, FEchoActionFrame& OutFrame) const;

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock")
    void RestoreStability(float Amount);

    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock|EchoCombat")
    int32 GetActionHistoryBufferSize() const { return ActionHistoryBuffer.Num(); }

    const TArray<FEchoActionFrame>& GetActionHistoryBuffer() const { return ActionHistoryBuffer; }

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock|EchoCombat")
    void ClearActionHistoryBuffer() { ActionHistoryBuffer.Empty(); }

    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock")
    ETimelineState GetActiveTimelineState() const { return ActiveTimelineState; }

    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock")
    float GetClockStability() const { return CurrentClockStability; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|BlackClock")
    FOnTimelineStateShifted OnTimelineStateShifted;

    UPROPERTY(BlueprintAssignable, Category="Edrath|BlackClock")
    FOnEchoGhostSpawned OnEchoGhostSpawned;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|BlackClock")
    float MaxClockStability = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|BlackClock")
    float CurrentClockStability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|BlackClock")
    float StabilityRegenPerSecond = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|BlackClock")
    float PossibilityShiftStabilityCost = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|BlackClock")
    float EchoStrikeStabilityCost = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|BlackClock")
    float PossibilityWaveMaxRadius = 1800.0f;

private:
    ETimelineState ActiveTimelineState = ETimelineState::PresentTimeline;
    TArray<FEchoActionFrame> ActionHistoryBuffer;
    int32 MaxBufferFrames = 120;
};
