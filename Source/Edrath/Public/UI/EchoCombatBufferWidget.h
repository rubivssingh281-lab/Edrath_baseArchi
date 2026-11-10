#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/EdrathTypes.h"
#include "EchoCombatBufferWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UPanelWidget;
class UBlackClockComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEchoBufferUpdated, int32, StoredFrameCount, float, BufferFillRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEchoStrikeCooldownChanged, float, RemainingCooldown, bool, bIsReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionalResonanceChanged, EEdrathRegion, NewRegion);

/**
 * UEchoCombatBufferWidget
 * Displays the real-time Echo Combat timeline buffer, action history,
 * strike projection delay, and regional echo resonance visual states.
 */
UCLASS(BlueprintType, Blueprintable)
class EDRATH_API UEchoCombatBufferWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** Binds to a character's Black Clock component to listen for combat buffer updates */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void BindToBlackClock(UBlackClockComponent* InBlackClock);

    /** Updates the circular buffer fullness metrics (stored frames up to 120 max) */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void UpdateBufferMetrics(int32 InStoredFrames, int32 InMaxFrames = 120);

    /** Pushes the latest recorded combat action montage section name into the visual timeline */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void PushActionRecord(FName MontageSectionName, float ActionDamage);

    /** Configures the Echo Strike delay target (default 1.2s in Act 1) */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void SetStrikeDelay(float InDelaySeconds);

    /** Updates the cooldown state for manifesting past Echo combat strikes */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void UpdateCooldown(float RemainingTime, float TotalCooldownDuration);

    /** Sets the active regional resonance theme for the Echo hologram */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void SetRegionalResonance(EEdrathRegion InRegion);

    /** Toggles the timeline distortion warning flash when regional instability is high */
    UFUNCTION(BlueprintCallable, Category="Edrath|UI|EchoCombat")
    void SetInstabilityWarning(bool bShowWarning, float InstabilityLevel);

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    float GetBufferFillRatio() const { return BufferFillRatio; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    int32 GetStoredFrames() const { return StoredFrames; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    float GetStrikeDelay() const { return StrikeDelaySeconds; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    bool IsEchoStrikeReady() const { return bIsStrikeReady; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    float GetCooldownPercent() const { return CooldownPercent; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    EEdrathRegion GetActiveRegionalResonance() const { return ActiveRegion; }

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    FLinearColor GetResonanceColorForRegion(EEdrathRegion InRegion) const;

    UFUNCTION(BlueprintPure, Category="Edrath|UI|EchoCombat")
    const TArray<FName>& GetRecentActionHistory() const { return RecentActions; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|EchoCombat")
    FOnEchoBufferUpdated OnEchoBufferUpdated;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|EchoCombat")
    FOnEchoStrikeCooldownChanged OnEchoStrikeCooldownChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|UI|EchoCombat")
    FOnRegionalResonanceChanged OnRegionalResonanceChanged;

protected:
    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    int32 StoredFrames = 0;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    int32 MaxFramesCapacity = 120;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    float BufferFillRatio = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    float StrikeDelaySeconds = 1.2f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    bool bIsStrikeReady = true;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    float CooldownRemaining = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    float CooldownTotal = 5.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    float CooldownPercent = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    EEdrathRegion ActiveRegion = EEdrathRegion::Veyl;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    bool bInstabilityWarningActive = false;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    float CurrentInstability = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TArray<FName> RecentActions;

    UPROPERTY(EditDefaultsOnly, Category="Edrath|UI|EchoCombat")
    int32 MaxRecentActionsDisplayed = 5;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UProgressBar> BufferProgressBar;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UProgressBar> CooldownProgressBar;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UTextBlock> StoredFramesLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UTextBlock> StrikeDelayLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UTextBlock> RegionalResonanceLabel;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UImage> ResonanceGlowImage;

    UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly, Category="Edrath|UI|EchoCombat")
    TObjectPtr<UImage> InstabilityWarningBadge;
};
