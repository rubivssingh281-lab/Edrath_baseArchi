// Edrath: The World That Forgot Tomorrow
// Subsystem: UEdrathPlatformSubsystem
// Purpose: Cross-platform hardware detection, Mobile-First vs PC scalability management, and thermal/battery optimization.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EdrathPlatformSubsystem.generated.h"

UENUM(BlueprintType)
enum class EEdrathDeviceTier : uint8
{
    Mobile_Low UMETA(DisplayName = "Mobile Low"),
    Mobile_Mid UMETA(DisplayName = "Mobile Mid (Vulkan)"),
    Mobile_High UMETA(DisplayName = "Mobile High"),
    Laptop_Battery UMETA(DisplayName = "Laptop Battery (Eco)"),
    Laptop_RTX5070 UMETA(DisplayName = "Laptop RTX 5070 (Balanced)"),
    PC_Ultra UMETA(DisplayName = "Desktop PC Ultra")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlatformTierChanged, EEdrathDeviceTier, NewTier);

/**
 * UEdrathPlatformSubsystem
 * Automatically configures rendering, input, and thermal profiles
 * for target hardware (RTX 5070 8GB / Ryzen 250 laptop and Android/iOS Mobile).
 */
UCLASS()
class EDRATH_API UEdrathPlatformSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Checks if the current target is a mobile device (Android or iOS) */
    UFUNCTION(BlueprintPure, Category = "Edrath|Platform")
    bool IsMobilePlatform() const;

    /** Returns active hardware scalability tier */
    UFUNCTION(BlueprintPure, Category = "Edrath|Platform")
    EEdrathDeviceTier GetDeviceTier() const { return CurrentTier; }

    /** Applies a specific hardware tier profile at runtime */
    UFUNCTION(BlueprintCallable, Category = "Edrath|Platform")
    void ApplyDeviceTier(EEdrathDeviceTier InTier);

    /** Configures touch interface visibility based on platform and input mode */
    UFUNCTION(BlueprintCallable, Category = "Edrath|Platform")
    void SetTouchControlsEnabled(bool bEnable);

    /** Called when the hardware scalability profile changes */
    UPROPERTY(BlueprintAssignable, Category = "Edrath|Platform")
    FOnPlatformTierChanged OnPlatformTierChanged;

protected:
    void DetectAndApplyOptimalSettings();
    void ApplyMobileOptimizations();
    void ApplyRTX5070LaptopOptimizations();

private:
    UPROPERTY()
    EEdrathDeviceTier CurrentTier = EEdrathDeviceTier::Laptop_RTX5070;

    bool bIsMobile = false;
};
