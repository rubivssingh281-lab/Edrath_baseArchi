#include "Core/EdrathGameInstance.h"
#include "Core/EdrathScalabilitySubsystem.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"

void UEdrathGameInstance::Init()
{
    Super::Init();

    // Both subsystems auto-detect hardware and apply optimizations during Initialize().
    // Log the final state here for development visibility in the output log.
    if (const UEdrathScalabilitySubsystem* Scalability = GetSubsystem<UEdrathScalabilitySubsystem>())
    {
        UE_LOG(LogTemp, Log,
               TEXT("Edrath GameInstance: Scalability preset=%d, VRAM=%d MB, RAM=%d MB"),
               static_cast<int32>(Scalability->GetCurrentQualityPreset()),
               Scalability->GetDetectedVRAMMB(),
               Scalability->GetDetectedSystemRAMMB());
    }

    if (const UEdrathHardwareOptimizationSubsystem* HWOpt = GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
    {
        const FEdrathHardwareProfile& Profile = HWOpt->GetHardwareProfile();
        UE_LOG(LogTemp, Log,
               TEXT("Edrath GameInstance: GPU=%s, VRAM Ceiling=%d MB, Cores=%d"),
               *Profile.GPUAdapterName, HWOpt->GetVRAMCeilingMB(), Profile.LogicalCores);
    }
}

void UEdrathGameInstance::Shutdown()
{
    Super::Shutdown();
}

bool UEdrathGameInstance::IsMobileBuild() const
{
#if PLATFORM_ANDROID || PLATFORM_IOS
    return true;
#else
    return false;
#endif
}
