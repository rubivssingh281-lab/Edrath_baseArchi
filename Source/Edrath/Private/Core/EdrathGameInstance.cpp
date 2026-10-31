#include "Core/EdrathGameInstance.h"
#include "Core/EdrathScalabilitySubsystem.h"

void UEdrathGameInstance::Init()
{
    Super::Init();

    if (UEdrathScalabilitySubsystem* Scalability = GetSubsystem<UEdrathScalabilitySubsystem>())
    {
        Scalability->AutoDetectPlatformAndApplyDefaults();
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
