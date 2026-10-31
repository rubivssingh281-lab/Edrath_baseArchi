#include "Core/EdrathScalabilitySubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/IConsoleManager.h"
#include "Engine/Engine.h"

void UEdrathScalabilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    AutoDetectPlatformAndApplyDefaults();
}

bool UEdrathScalabilitySubsystem::IsRunningOnMobile() const
{
#if PLATFORM_ANDROID || PLATFORM_IOS
    return true;
#else
    return false;
#endif
}

void UEdrathScalabilitySubsystem::AutoDetectPlatformAndApplyDefaults()
{
    if (IsRunningOnMobile())
    {
        ApplyQualityPreset(EEdrathQualityPreset::Low_Mobile);
        SetTargetFrameRate(30);
        SetDynamicResolutionEnabled(true);
        SetLumenHardwareRayTracingEnabled(false);
    }
    else
    {
        ApplyQualityPreset(EEdrathQualityPreset::Epic_PC);
        SetTargetFrameRate(60);
        SetDynamicResolutionEnabled(false);
        SetLumenHardwareRayTracingEnabled(true);
    }
}

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
        bDynamicResolutionEnabled = false;
        break;
    case EEdrathQualityPreset::Epic_PC:
    default:
        ScalabilityLevel = 3;
        bLumenHardwareRayTracing = true;
        bDynamicResolutionEnabled = false;
        break;
    }

    ApplyInternalQualityGroupSettings(ScalabilityLevel);

    if (OnScalabilityPresetChanged.IsBound())
    {
        OnScalabilityPresetChanged.Broadcast(CurrentQualityPreset);
    }
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
