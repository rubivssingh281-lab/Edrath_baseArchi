// Edrath: The World That Forgot Tomorrow
// Implementation: UEdrathPlatformSubsystem

#include "EdrathPlatformSubsystem.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"

void UEdrathPlatformSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const FString PlatformName = UGameplayStatics::GetPlatformName();
    bIsMobile = (PlatformName == TEXT("Android") || PlatformName == TEXT("IOS"));

    DetectAndApplyOptimalSettings();
}

void UEdrathPlatformSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

bool UEdrathPlatformSubsystem::IsMobilePlatform() const
{
    return bIsMobile;
}

void UEdrathPlatformSubsystem::DetectAndApplyOptimalSettings()
{
    if (bIsMobile)
    {
        ApplyDeviceTier(EEdrathDeviceTier::Mobile_Mid);
    }
    else
    {
        // Default target laptop: RTX 5070 8GB / Ryzen 250 / 16GB RAM
        ApplyDeviceTier(EEdrathDeviceTier::Laptop_RTX5070);
    }
}

void UEdrathPlatformSubsystem::ApplyDeviceTier(EEdrathDeviceTier InTier)
{
    CurrentTier = InTier;

    UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;

    switch (CurrentTier)
    {
    case EEdrathDeviceTier::Mobile_Low:
    case EEdrathDeviceTier::Mobile_Mid:
    case EEdrathDeviceTier::Mobile_High:
        ApplyMobileOptimizations();
        break;

    case EEdrathDeviceTier::Laptop_Battery:
        if (GEngine)
        {
            GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 60"));
            GEngine->Exec(GetWorld(), TEXT("r.Lumen.HardwareRayTracing 0"));
            GEngine->Exec(GetWorld(), TEXT("r.Shadow.Virtual.Enable 0"));
            GEngine->Exec(GetWorld(), TEXT("sg.ShadowQuality 1"));
            GEngine->Exec(GetWorld(), TEXT("sg.GlobalIlluminationQuality 1"));
        }
        break;

    case EEdrathDeviceTier::Laptop_RTX5070:
        ApplyRTX5070LaptopOptimizations();
        break;

    case EEdrathDeviceTier::PC_Ultra:
        if (GEngine)
        {
            GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 120"));
            GEngine->Exec(GetWorld(), TEXT("r.Shadow.Virtual.Enable 1"));
            GEngine->Exec(GetWorld(), TEXT("r.Nanite 1"));
            GEngine->Exec(GetWorld(), TEXT("sg.GlobalIlluminationQuality 3"));
        }
        break;
    }

    if (UserSettings)
    {
        UserSettings->ApplySettings(false);
    }

    SetTouchControlsEnabled(bIsMobile);
    OnPlatformTierChanged.Broadcast(CurrentTier);
}

void UEdrathPlatformSubsystem::ApplyMobileOptimizations()
{
    if (!GEngine) return;

    // Mobile rendering pipeline optimizations:
    // 1. Disable Lumen and Virtual Shadow Maps (unsupported/heavy on mobile TBDR)
    // 2. Clamp frame rate to 60 FPS (or 30 on low) for thermal/battery stability
    // 3. Keep texture streaming pool within mobile unified memory limits
    GEngine->Exec(GetWorld(), TEXT("r.DynamicGlobalIlluminationMethod 0"));
    GEngine->Exec(GetWorld(), TEXT("r.ReflectionMethod 0"));
    GEngine->Exec(GetWorld(), TEXT("r.Nanite 0"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.Virtual.Enable 0"));
    GEngine->Exec(GetWorld(), TEXT("r.ShadowQuality 1"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.CSM.MaxCascades 2"));
    GEngine->Exec(GetWorld(), TEXT("r.Streaming.PoolSize 640"));
    GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 60"));
    GEngine->Exec(GetWorld(), TEXT("r.MobileContentScaleFactor 1.0"));
}

void UEdrathPlatformSubsystem::ApplyRTX5070LaptopOptimizations()
{
    if (!GEngine) return;

    // RTX 5070 Laptop (8GB VRAM) & Ryzen 250 (16GB RAM) optimizations:
    // 1. Cap VRAM allocations: Texture pool 2.5GB, VSM max physical pages 1536
    // 2. Enable TSR at 75% render scale for stable 90 FPS
    // 3. Software Ray Tracing Lumen with downsampled probes to prevent VRAM spikes
    GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 90"));
    GEngine->Exec(GetWorld(), TEXT("r.ScreenPercentage 75"));
    GEngine->Exec(GetWorld(), TEXT("r.DynamicRes.OperationMode 2"));
    GEngine->Exec(GetWorld(), TEXT("r.Streaming.PoolSize 2560"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.Virtual.Enable 1"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.Virtual.MaxPhysicalPages 1536"));
    GEngine->Exec(GetWorld(), TEXT("r.Lumen.HardwareRayTracing 0"));
    GEngine->Exec(GetWorld(), TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor 16"));
    GEngine->Exec(GetWorld(), TEXT("r.Lumen.Reflections.DownsampleFactor 2"));
    GEngine->Exec(GetWorld(), TEXT("r.Nanite 1"));
    GEngine->Exec(GetWorld(), TEXT("r.Nanite.Streaming.PoolSize 768"));
}

void UEdrathPlatformSubsystem::SetTouchControlsEnabled(bool bEnable)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        if (bEnable)
        {
            PC->ActivateTouchInterface(nullptr); // Uses DefaultTouchInterface from InputSettings
        }
    }
}
