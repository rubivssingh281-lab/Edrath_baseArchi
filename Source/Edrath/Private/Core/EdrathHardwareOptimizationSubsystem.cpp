#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformMisc.h"
#include "RHI.h"

// ============================================================================
// Initialization
// ============================================================================

void UEdrathHardwareOptimizationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    DetectHardware();
    CalculateVRAMCeiling();
    ApplyBaseHardwareConfiguration();
    ApplyCameraModeOptimization(EEdrathCameraMode::ExteriorBirdsEye);
    OptimizeTimelineMemoryFootprint(ETimelineState::PresentTimeline);
}

// ============================================================================
// Console Variable Helpers
// ============================================================================

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

// ============================================================================
// Hardware Detection
// ============================================================================

void UEdrathHardwareOptimizationSubsystem::DetectHardware()
{
    // --- System RAM ---
    const FPlatformMemoryConstants& MemConstants = FPlatformMemory::GetConstants();
    HardwareProfile.SystemRAMMB = static_cast<int32>(MemConstants.TotalPhysical / (1024 * 1024));

    // --- CPU Cores ---
    HardwareProfile.LogicalCores = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

    // --- GPU VRAM ---
    FTextureMemoryStats TextureMemStats;
    RHIGetTextureMemoryStats(TextureMemStats);
    if (TextureMemStats.DedicatedVideoMemory > 0)
    {
        HardwareProfile.DedicatedVRAMMB = static_cast<int32>(
            TextureMemStats.DedicatedVideoMemory / (1024 * 1024));
    }

    // --- GPU Adapter Name ---
    HardwareProfile.GPUAdapterName = GRHIAdapterName;

    // --- Hardware Ray Tracing Support ---
    HardwareProfile.bSupportsHardwareRayTracing = GRHISupportsRayTracing;

    UE_LOG(LogTemp, Log,
           TEXT("Edrath HW Profile: GPU=[%s] VRAM=%d MB, RAM=%d MB, Cores=%d, HWRT=%s"),
           *HardwareProfile.GPUAdapterName,
           HardwareProfile.DedicatedVRAMMB,
           HardwareProfile.SystemRAMMB,
           HardwareProfile.LogicalCores,
           HardwareProfile.bSupportsHardwareRayTracing ? TEXT("Supported") : TEXT("Not Supported"));
}

void UEdrathHardwareOptimizationSubsystem::CalculateVRAMCeiling()
{
    if (HardwareProfile.DedicatedVRAMMB > 0)
    {
        // Reserve 512MB for GPU driver, OS compositor, and safety headroom
        VRAMCeilingMB = HardwareProfile.DedicatedVRAMMB - 512;
        VRAMCeilingMB = FMath::Max(VRAMCeilingMB, 2048);
    }
    else
    {
        // Detection failed — assume 8GB GPU (safe default for development target)
        VRAMCeilingMB = 7680;
    }

    UE_LOG(LogTemp, Log, TEXT("Edrath VRAM Ceiling: %d MB (detected %d MB, reserved 512 MB)"),
           VRAMCeilingMB, HardwareProfile.DedicatedVRAMMB);
}

// ============================================================================
// VRAM Budget Allocation
// ============================================================================

void UEdrathHardwareOptimizationSubsystem::ApplyBaseHardwareConfiguration()
{
    // Calculate available VRAM for rendering pools (after fixed costs)
    const int32 FixedCosts = VRAMBudget.FramebufferMB + VRAMBudget.ShaderCacheMB
                           + VRAMBudget.DriverOverheadMB;
    const int32 AvailableForPools = VRAMCeilingMB - FixedCosts;

    // Distribute VRAM across rendering subsystems based on available budget
    if (AvailableForPools >= 5500)
    {
        // === HIGH VRAM TIER (RTX 5070 8GB — push to the limit) ===
        VRAMBudget.TextureStreamingMB = 2560;   // Maximum texture fidelity
        VRAMBudget.NaniteStreamingMB  = 1024;   // Full geometric detail
        VRAMBudget.LumenEstimateMB    = 1200;   // High probe density software Lumen
        VRAMBudget.VSMEstimateMB      = 768;    // Sharp virtual shadow maps
    }
    else if (AvailableForPools >= 3500)
    {
        // === MEDIUM VRAM TIER (4-6GB GPUs) ===
        VRAMBudget.TextureStreamingMB = 1792;
        VRAMBudget.NaniteStreamingMB  = 640;
        VRAMBudget.LumenEstimateMB    = 800;
        VRAMBudget.VSMEstimateMB      = 512;
    }
    else
    {
        // === LOW VRAM TIER (< 4GB — no Lumen, no VSM) ===
        VRAMBudget.TextureStreamingMB = 1024;
        VRAMBudget.NaniteStreamingMB  = 384;
        VRAMBudget.LumenEstimateMB    = 0;
        VRAMBudget.VSMEstimateMB      = 0;
    }

    ApplyVRAMBudget();

    // --- Shader Compiler: scale with CPU cores, cap memory for 16GB systems ---
    const int32 ShaderJobs = FMath::Clamp(HardwareProfile.LogicalCores - 4, 2, 10);
    const int32 ShaderMemory = (HardwareProfile.SystemRAMMB >= 32768) ? 2048 : 1536;
    SetConsoleVariable(TEXT("r.ShaderCompiler.MaxShaderJobs"), ShaderJobs);
    SetConsoleVariable(TEXT("r.ShaderCompiler.MaxShaderMemory"), ShaderMemory);
    SetConsoleVariable(TEXT("r.ShaderCompiler.WorkerProcessPriority"), -1);
    SetConsoleVariable(TEXT("r.ShaderDevelopmentMode"), 0);

    // --- GPU Performance ---
    SetConsoleVariable(TEXT("r.GPUCrashDebugging"), 0);
}

void UEdrathHardwareOptimizationSubsystem::ApplyVRAMBudget()
{
    // --- Texture Streaming ---
    SetConsoleVariable(TEXT("r.Streaming.PoolSize"), VRAMBudget.TextureStreamingMB);
    SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 256);
    SetConsoleVariable(TEXT("r.Streaming.FullyLoadUsedTextures"), 0);
    SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 4);

    // --- Nanite ---
    SetConsoleVariable(TEXT("r.Nanite.Streaming.PoolSize"), VRAMBudget.NaniteStreamingMB);
    SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.0f);

    // --- Lumen (Software — no HWRT, saves ~2GB for everything else) ---
    SetConsoleVariable(TEXT("r.Lumen.HardwareRayTracing"), 0);
    SetConsoleVariable(TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor"), 8);
    SetConsoleVariable(TEXT("r.Lumen.Reflections.DownsampleFactor"), 1);
    SetConsoleVariable(TEXT("r.Lumen.TraceMeshSDFs"), 1);
    SetConsoleVariable(TEXT("r.Lumen.ScreenProbeGather.Temporal.MaxFramesAccumulated"), 8);
    SetConsoleVariable(TEXT("r.Lumen.FinalGather.Temporal.MaxFramesAccumulated"), 8);

    // --- Virtual Shadow Maps ---
    const int32 VSMPages = (VRAMBudget.VSMEstimateMB >= 700) ? 2048 : 1024;
    SetConsoleVariable(TEXT("r.Shadow.Virtual.Enable"), (VRAMBudget.VSMEstimateMB > 0) ? 1 : 0);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.MaxPhysicalPages"), VSMPages);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasLocal"), 0.0f);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasDirectional"), -0.5f);
    SetConsoleVariable(TEXT("r.Shadow.Virtual.Cache.StaticSeparate"), 1);

    // --- TSR ---
    SetConsoleVariable(TEXT("r.TSR.History.ScreenPercentage"), 100);
    SetConsoleVariable(TEXT("r.TSR.ShadingRejection.Flickering"), 1);
    SetConsoleVariable(TEXT("r.ScreenPercentage"), 77.0f);

    UE_LOG(LogTemp, Log,
           TEXT("Edrath VRAM Budget: Tex=%d + Nanite=%d + Lumen~%d + VSM~%d + Fixed=%d = %d MB (Ceiling=%d MB, Headroom=%d MB)"),
           VRAMBudget.TextureStreamingMB, VRAMBudget.NaniteStreamingMB,
           VRAMBudget.LumenEstimateMB, VRAMBudget.VSMEstimateMB,
           VRAMBudget.FramebufferMB + VRAMBudget.ShaderCacheMB + VRAMBudget.DriverOverheadMB,
           VRAMBudget.GetTotalAllocatedMB(), VRAMCeilingMB,
           VRAMCeilingMB - VRAMBudget.GetTotalAllocatedMB());
}

// ============================================================================
// Context-Aware Runtime Optimizations
// ============================================================================

void UEdrathHardwareOptimizationSubsystem::ApplyCameraModeOptimization(EEdrathCameraMode CameraMode)
{
    OptimizedCameraMode = CameraMode;

    if (CameraMode == EEdrathCameraMode::ExteriorBirdsEye)
    {
        // Bird's-eye isometric: wide FOV, many objects visible at distance.
        // Trade close-up detail for draw distance and streaming efficiency.
        SetConsoleVariable(TEXT("r.ViewDistanceScale"), 1.1f);
        SetConsoleVariable(TEXT("r.StaticMeshLODDistanceScale"), 0.75f);
        SetConsoleVariable(TEXT("r.SkeletalMeshLODBias"), 1);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.2f);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.25f);

        // Shadows less scrutinized from above — slight quality reduction saves VRAM
        const int32 VSMPages = (VRAMBudget.VSMEstimateMB >= 700) ? 1536 : 768;
        SetConsoleVariable(TEXT("r.Shadow.Virtual.MaxPhysicalPages"), VSMPages);
        SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasLocal"), 0.5f);
    }
    else // InteriorThirdPerson
    {
        // Over-the-shoulder 3rd person: close-up detail is critical.
        // Fewer objects visible = budget goes to quality per object.
        SetConsoleVariable(TEXT("r.ViewDistanceScale"), 0.75f);
        SetConsoleVariable(TEXT("r.StaticMeshLODDistanceScale"), 0.5f);
        SetConsoleVariable(TEXT("r.SkeletalMeshLODBias"), 0);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 0.8f);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.0f);

        // Maximum shadow quality — player scrutinizes interiors closely
        const int32 VSMPages = (VRAMBudget.VSMEstimateMB >= 700) ? 2048 : 1024;
        SetConsoleVariable(TEXT("r.Shadow.Virtual.MaxPhysicalPages"), VSMPages);
        SetConsoleVariable(TEXT("r.Shadow.Virtual.ResolutionLodBiasLocal"), 0.0f);
    }
}

void UEdrathHardwareOptimizationSubsystem::OptimizeTimelineMemoryFootprint(ETimelineState ActiveState)
{
    OptimizedTimelineState = ActiveState;

    // During Possibility Shifts, two world states may coexist briefly in memory.
    // Reduce streaming budgets proportionally to prevent VRAM overcommit.

    if (ActiveState == ETimelineState::RejectedTimeline)
    {
        // Rejected Timeline: desaturated, corrupted world aesthetic.
        // Reduced texture quality is thematically appropriate — degraded reality.
        const int32 ReducedTexPool = FMath::Max(VRAMBudget.TextureStreamingMB - 512, 1024);
        SetConsoleVariable(TEXT("r.Streaming.PoolSize"), ReducedTexPool);
        SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 128);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.0f);
        SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 2);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.5f);
    }
    else if (ActiveState == ETimelineState::MemoryReconstruction)
    {
        // Memory Reconstruction: fragmented, dreamlike sequences.
        // Most aggressive reduction — the ghostly visual style masks lower fidelity.
        const int32 ReducedTexPool = FMath::Max(VRAMBudget.TextureStreamingMB - 768, 768);
        SetConsoleVariable(TEXT("r.Streaming.PoolSize"), ReducedTexPool);
        SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 128);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.0f);
        SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 1);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"), 1.25f);
    }
    else // PresentTimeline
    {
        // Present Timeline: the "real" world — full quality restored.
        SetConsoleVariable(TEXT("r.Streaming.PoolSize"), VRAMBudget.TextureStreamingMB);
        SetConsoleVariable(TEXT("r.Streaming.MaxTempMemoryAllowed"), 256);
        SetConsoleVariable(TEXT("r.Streaming.HiddenPrimitiveScale"), 0.5f);
        SetConsoleVariable(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), 4);
        SetConsoleVariable(TEXT("r.Nanite.MaxPixelsPerEdge"),
                           OptimizedCameraMode == EEdrathCameraMode::InteriorThirdPerson ? 0.8f : 1.2f);
    }
}
