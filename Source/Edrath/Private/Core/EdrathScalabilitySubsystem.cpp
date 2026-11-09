#include "Core/EdrathScalabilitySubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMemory.h"
#include "Engine/Engine.h"
#include "RHI.h"

void UEdrathScalabilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    DetectHardwareCapabilities();
    AutoDetectPlatformAndApplyDefaults();
}

// ============================================================================
// Hardware Detection
// ============================================================================

void UEdrathScalabilitySubsystem::DetectHardwareCapabilities()
{
    // --- System RAM ---
    const FPlatformMemoryConstants& MemConstants = FPlatformMemory::GetConstants();
    DetectedSystemRAMMB = static_cast<int32>(MemConstants.TotalPhysical / (1024 * 1024));

    // --- GPU VRAM via RHI texture memory stats ---
    FTextureMemoryStats TextureMemStats;
    RHIGetTextureMemoryStats(TextureMemStats);
    if (TextureMemStats.DedicatedVideoMemory > 0)
    {
        DetectedVRAMMB = static_cast<int32>(TextureMemStats.DedicatedVideoMemory / (1024 * 1024));
    }

    UE_LOG(LogTemp, Log, TEXT("Edrath HW Detection: VRAM=%d MB, SystemRAM=%d MB"),
           DetectedVRAMMB, DetectedSystemRAMMB);
}

bool UEdrathScalabilitySubsystem::IsRunningOnMobile() const
{
#if PLATFORM_ANDROID || PLATFORM_IOS
    return true;
#else
    return false;
#endif
}

// ============================================================================
// Hardware-Aware Auto-Detection
// ============================================================================

void UEdrathScalabilitySubsystem::AutoDetectPlatformAndApplyDefaults()
{
    if (IsRunningOnMobile())
    {
        ApplyQualityPreset(EEdrathQualityPreset::Low_Mobile);
        SetTargetFrameRate(30);
        return;
    }

    // --- Desktop: select preset based on detected VRAM ---
    if (DetectedVRAMMB >= 12288)
    {
        // 12GB+ VRAM (RTX 4080, RTX 5080, etc.) — full Epic with HWRT
        ApplyQualityPreset(EEdrathQualityPreset::Epic_PC);
        SetTargetFrameRate(60);
    }
    else if (DetectedVRAMMB >= 6144)
    {
        // 6-12GB VRAM (RTX 5070 8GB lands here) — max quality via software Lumen
        ApplyQualityPreset(EEdrathQualityPreset::Development_Laptop);
        SetTargetFrameRate(60);
    }
    else if (DetectedVRAMMB >= 3072)
    {
        // 3-6GB VRAM — High preset
        ApplyQualityPreset(EEdrathQualityPreset::High);
        SetTargetFrameRate(60);
    }
    else if (DetectedVRAMMB >= 1024)
    {
        // 1-3GB VRAM — Medium preset
        ApplyQualityPreset(EEdrathQualityPreset::Medium);
        SetTargetFrameRate(30);
    }
    else
    {
        // VRAM detection failed — use system RAM as proxy
        if (DetectedSystemRAMMB >= 16384)
        {
            ApplyQualityPreset(EEdrathQualityPreset::Development_Laptop);
        }
        else if (DetectedSystemRAMMB >= 8192)
        {
            ApplyQualityPreset(EEdrathQualityPreset::High);
        }
        else
        {
            ApplyQualityPreset(EEdrathQualityPreset::Medium);
        }
        SetTargetFrameRate(60);
    }
}

// ============================================================================
// Preset Application
// ============================================================================

void UEdrathScalabilitySubsystem::ApplyQualityPreset(EEdrathQualityPreset Preset)
{
    CurrentQualityPreset = Preset;
    int32 ScalabilityLevel = 3;

    switch (Preset)
    {
    case EEdrathQualityPreset::Low_Mobile:
        ScalabilityLevel = 0;
        bLumenHardwareRayTracing = false;
        bDynamicResolutionEnabled = true;
        break;
    case EEdrathQualityPreset::Medium:
        ScalabilityLevel = 1;
        bLumenHardwareRayTracing = false;
        bDynamicResolutionEnabled = true;
        break;
    case EEdrathQualityPreset::High:
        ScalabilityLevel = 2;
        bLumenHardwareRayTracing = false;
        bDynamicResolutionEnabled = true;
        break;
    case EEdrathQualityPreset::Development_Laptop:
        // Scalability level 3 (highest quality groups) with software Lumen.
        // The VRAM freed by disabling HWRT is redirected to maximize
        // Nanite, textures, VSM, and reflection quality.
        ScalabilityLevel = 3;
        bLumenHardwareRayTracing = false;
        bDynamicResolutionEnabled = true;
        break;
    case EEdrathQualityPreset::Epic_PC:
    default:
        // 12GB+ VRAM — enough for hardware ray tracing
        ScalabilityLevel = 3;
        bLumenHardwareRayTracing = true;
        bDynamicResolutionEnabled = false;
        break;
    }

    ApplyInternalQualityGroupSettings(ScalabilityLevel);

    if (Preset == EEdrathQualityPreset::Development_Laptop)
    {
        ApplyLaptopDevelopmentOverrides();
    }

    SetDynamicResolutionEnabled(bDynamicResolutionEnabled);

    if (OnScalabilityPresetChanged.IsBound())
    {
        OnScalabilityPresetChanged.Broadcast(CurrentQualityPreset);
    }

    UE_LOG(LogTemp, Log,
           TEXT("Edrath Scalability: Preset=%d, Level=%d, HWRT=%s, DynRes=%s"),
           static_cast<int32>(Preset), ScalabilityLevel,
           bLumenHardwareRayTracing ? TEXT("ON") : TEXT("OFF"),
           bDynamicResolutionEnabled ? TEXT("ON") : TEXT("OFF"));
}

void UEdrathScalabilitySubsystem::ApplyInternalQualityGroupSettings(int32 QualityLevel)
{
    if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
    {
        UserSettings->SetOverallScalabilityLevel(QualityLevel);
        UserSettings->ApplySettings(false);
    }

    if (IConsoleVariable* HWRTCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.HardwareRayTracing")))
    {
        HWRTCVar->Set(bLumenHardwareRayTracing ? 1 : 0, ECVF_SetByCode);
    }
}

void UEdrathScalabilitySubsystem::ApplyLaptopDevelopmentOverrides()
{
    // Helper lambdas for clean CVar access
    auto SetCVarFloat = [](const TCHAR* Name, float Value)
    {
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
        {
            CVar->Set(Value, ECVF_SetByCode);
        }
    };

    auto SetCVarInt = [](const TCHAR* Name, int32 Value)
    {
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
        {
            CVar->Set(Value, ECVF_SetByCode);
        }
    };

    // --- Lumen GI: maximum software probe density ---
    SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor"), 8);
    SetCVarInt(TEXT("r.Lumen.TraceMeshSDFs"), 1);
    SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.Temporal.MaxFramesAccumulated"), 8);
    SetCVarInt(TEXT("r.Lumen.FinalGather.Temporal.MaxFramesAccumulated"), 8);

    // --- Lumen Reflections: full resolution ---
    SetCVarFloat(TEXT("r.Lumen.Reflections.DownsampleFactor"), 1.0f);

    // --- TSR: high quality reconstruction ---
    SetCVarInt(TEXT("r.TSR.History.ScreenPercentage"), 100);
    SetCVarFloat(TEXT("r.ScreenPercentage"), 77.0f);

    // --- Dynamic Resolution: 60-100% targeting 60 FPS ---
    SetCVarFloat(TEXT("r.DynamicRes.MinScreenPercentage"), 60.0f);
    SetCVarFloat(TEXT("r.DynamicRes.MaxScreenPercentage"), 100.0f);
}

// ============================================================================
// Individual Setting Controls
// ============================================================================

void UEdrathScalabilitySubsystem::SetTargetFrameRate(int32 TargetFPS)
{
    CurrentTargetFrameRate = TargetFPS;
    if (GEngine)
    {
        GEngine->SetMaxFPS(static_cast<float>(TargetFPS));
    }
    if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
    {
        UserSettings->SetFrameRateLimit(static_cast<float>(TargetFPS));
        UserSettings->ApplySettings(false);
    }
}

void UEdrathScalabilitySubsystem::SetResolutionScale(float NewScale)
{
    const float ClampedScale = FMath::Clamp(NewScale, 0.5f, 1.0f);
    if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
    {
        UserSettings->SetResolutionScaleNormalized(ClampedScale);
        UserSettings->ApplySettings(false);
    }
}

float UEdrathScalabilitySubsystem::GetResolutionScale() const
{
    if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
    {
        return UserSettings->GetResolutionScaleNormalized();
    }
    return 1.0f;
}

void UEdrathScalabilitySubsystem::SetDynamicResolutionEnabled(bool bEnable)
{
    bDynamicResolutionEnabled = bEnable;
    if (IConsoleVariable* DynResCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicRes.OperationMode")))
    {
        DynResCVar->Set(bEnable ? 2 : 0, ECVF_SetByCode);
    }
}

void UEdrathScalabilitySubsystem::SetLumenHardwareRayTracingEnabled(bool bEnable)
{
    bLumenHardwareRayTracing = bEnable;
    if (IConsoleVariable* HWRTCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.HardwareRayTracing")))
    {
        HWRTCVar->Set(bEnable ? 1 : 0, ECVF_SetByCode);
    }
}
