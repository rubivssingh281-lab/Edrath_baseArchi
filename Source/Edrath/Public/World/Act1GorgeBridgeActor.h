#pragma once

#include "CoreMinimal.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Act1GorgeBridgeActor.generated.h"

class UBoxComponent;
class AAct1PreAlphaSliceDirector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGorgeBridgeTimelineShiftedSignature, class AAct1GorgeBridgeActor*, BridgeActor, ETimelineState, NewState);

/**
 * Act 1 Collapsed Gorge Bridge - Zone 2 Possibility Shift Actor
 * A shattered stone chasm bridge blocking the road to the 7-day looping settlement.
 * Activating the Black Clock Possibility Shift toggles the intact bridge mesh and collision,
 * allowing Kael to cross into the settlement.
 */
UCLASS()
class EDRATH_API AAct1GorgeBridgeActor : public APossibilityShiftActor
{
    GENERATED_BODY()

public:
    AAct1GorgeBridgeActor();

    virtual void BeginPlay() override;
    virtual void ApplyTimelineState(ETimelineState NewState) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UBoxComponent> ProximityTrigger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Prompt")
    FText ShiftPromptText = FText::FromString(TEXT("The gorge bridge is shattered. Activate the Black Clock to restore its rejected timeline."));

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|State")
    bool bPlayerNearChasm = false;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Events")
    FOnGorgeBridgeTimelineShiftedSignature OnBridgeTimelineShifted;

    UFUNCTION(BlueprintPure, Category = "Edrath|Bridge")
    bool IsPassable() const { return GetCurrentActorTimelineState() == ETimelineState::RejectedTimeline; }

    UFUNCTION(BlueprintCallable, Category = "Edrath|Bridge")
    void BindToDirector(AAct1PreAlphaSliceDirector* InDirector);

protected:
    UFUNCTION()
    void HandleProximityBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void HandleProximityEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

private:
    UPROPERTY()
    TWeakObjectPtr<AAct1PreAlphaSliceDirector> BoundDirector;
};
