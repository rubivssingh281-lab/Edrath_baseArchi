#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "HAL/IConsoleManager.h"

void UEdrathHardwareOptimizationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ApplyBaseHardwareConfiguration();
    ApplyCameraModeOptimization(EEdrathCameraMode::ExteriorBirdsEye);
    OptimizeTimelineMemoryFootprint(ETimelineState::PresentTimeline);
}

void UEdrathHardwareOptimizationSubsystem::SetConsoleVariable(const TCHAR* VariableName, int32 Value) const
{
    if (IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(VariableName))
    {
        Variable->Set(Value, ECVF_SetByCode);
    }
}

void UEdrathHardwareOptimizationSubsystem::SetConsoleVariable(const TCHAR* VariableName, float Value) const
{
    if (IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(VariableName))
    {
        Variable->Set(Value, ECVF_SetByCode);
    }
}

void UEdrathHardwareOptimizationSubsystem::ApplyBaseHardwareConfiguration()
{
    SetConsoleVariable(TEXT("r.Streaming.PoolSize"), 3072);
    SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 256);
    SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.5f);
    SetConsoleVariable(TEXT("r.Lumen.HardwareRayTracing"), 1);
    SetConsoleVariable(TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor"), 16);
    SetConsoleVariable(TEXT("r.Lumen.Reflections.DownsampleFactor"), 2);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.Enable"), 1);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.MaxPhysicalPages"), 2048);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasLocal"), 0.5f);
    SetConsoleVariable(TEXT("r.TSR.History.ScreenPercentage"), 100);
    SetConsoleVariable(TEXT("r.ScreenPercentage"), 75);
    SetConsoleVariable(TEXT("r.ShaderDevelopmentMode"), 0);
    SetConsoleVariable(TEXT("r.ShaderCompiler.MaxShaderJobs"), 8);
    SetConsoleVariable(TEXT("r.ShaderCompiler.MaxShaderMemory"), 2048);
    SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.0f);
}

void UEdrathHardwareOptimizationSubsystem::ApplyCameraModeOptimization(EEdrathCameraMode CameraMode)
{
    OptimizedCameraMode = CameraMode;
    if (CameraMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        SetConsoleVariable(TEXT("r.ViewDistanceScale"), 1.25f);
        SetConsoleVariable(TEXT("r.StaticMeshLODDistanceScale"), 0.75f);
        SetConsoleVariable(TEXT("r.Shadow.Virtual.MaxPhysicalPages"), 1536);
        SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasLocal"), 0.75f);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.5f);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.25f);
    }
    else
    {
        SetConsoleVariable(TEXT("r.ViewDistanceScale"), 0.75f);
        SetConsoleVariable(TEXT("r.StaticMeshLODDistanceScale"), 0.5f);
        SetConsoleVariable(TEXT("r.Shadow.Virtual.MaxPhysicalPages"), 2048);
        SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasLocal"), 0.0f);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 0.75f);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.0f);
    }
}

void UEdrathHardwareOptimizationSubsystem::OptimizeTimelineMemoryFootprint(ETimelineState ActiveState)
{
    OptimizedTimelineState = ActiveState;
    if (ActiveState == ETimelineState::RejectedTimeline)
    {
        SetConsoleVariable(TEXT("r.Streaming.PoolSize"), 2816);
        SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 128);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.0f);
        SetConsoleVariable(TEXT("r.Streaming.FullyLoadUsedTextures"), 0);
        SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 2);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.5f);
    }
    else if (ActiveState == ETimelineState::MemoryReconstruction)
    {
        SetConsoleVariable(TEXT("r.Streaming.PoolSize"), 2560);
        SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 128);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.0f);
        SetConsoleVariable(TEXT("r.Streaming.FullyLoadUsedTextures"), 0);
        SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 1);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.25f);
    }
    else
    {
        SetConsoleVariable(TEXT("r.Streaming.PoolSize"), 3072);
        SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 256);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.5f);
        SetConsoleVariable(TEXT("r.Streaming.FullyLoadUsedTextures"), 0);
        SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 4);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), OptimizedCameraMode == EEdrathCameraMode::InteriorThirdPerson ? 0.75f : 1.5f);
    }
}
