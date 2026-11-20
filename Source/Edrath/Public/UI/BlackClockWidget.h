#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/EdrathTypes.h"
#include "BlackClockWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UBlackClockComponent;
class UEdrathNarrativeSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIClockTimelineChanged, ETimelineState, NewState);

/**
 * UBlackClockWidget handles HUD visualization for the protagonist's ticking Black Clock:
 * - Real-time Black Clock stability percentage.
 * - Active timeline perspective indicator (Present vs. Rejected vs. Memory Reconstruction).
 * - Regional reality instability metric queried from the Narrative Subsystem.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UBlackClockWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|BlackClock")
    void BindToClockComponent(UBlackClockComponent* InClockComponent);

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|BlackClock")
    void UpdateClockStability(float CurrentStability, float MaxStability);

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|BlackClock")
    void UpdateTimelineState(ETimelineState NewTimelineState);

    UFUNCTION(BlueprintCallable, Category="Edrath|UI|BlackClock")
    void UpdateRegionalInstability(float InstabilityScore);

    UFUNCTION(BlueprintPure, Category="Edrath|UI|BlackClock")
    float GetDisplayedStabilityPercent() const { return DisplayedStabilityPercent; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|BlackClock")
    ETimelineState GetDisplayedTimelineState() const { return DisplayedTimelineState; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|BlackClock")
    float GetDisplayedRegionalInstability() const { return DisplayedRegionalInstability; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|BlackClock")
    FText GetTimelineStateDisplayName(ETimelineState State) const;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|BlackClock")
    FOnUIClockTimelineChanged OnTimelineVisualChanged;

protected:
    UFUNCTION()
    void HandleClockTimelineShifted(ETimelineState NewState, FVector Epicenter, float WaveRadius);

    /** Designer hook to drive dynamic Slate animations and color grade shifts based on timeline */
    UFUNCTION(BlueprintImplementableEvent, Category="Edrath|UI|BlackClock")
    void OnTimelineThemeChanged(ETimelineState NewState);

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|BlackClock")
    TObjectPtr<UProgressBar> StabilityProgressBar;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|BlackClock")
    TObjectPtr<UTextBlock> StabilityPercentLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|BlackClock")
    TObjectPtr<UTextBlock> TimelineStateLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|BlackClock")
    TObjectPtr<UTextBlock> RegionalInstabilityLabel;

private:
    UPROPERTY()
    TWeakObjectPtr<UBlackClockComponent> BoundClockComponent;

    float DisplayedStabilityPercent = 100.0f;
    ETimelineState DisplayedTimelineState = ETimelineState::PresentTimeline;
    float DisplayedRegionalInstability = 0.0f;
};
