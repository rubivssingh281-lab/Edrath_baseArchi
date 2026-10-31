#include "Narrative/MemoryReconstructionAnchor.h"
#include "BlackClock/BlackClockComponent.h"

AMemoryReconstructionAnchor::AMemoryReconstructionAnchor()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AMemoryReconstructionAnchor::ActivateMemoryReconstruction(AActor* ActivatingActor)
{
    if (!ActivatingActor)
    {
        return false;
    }
    if (UBlackClockComponent* BlackClock = ActivatingActor->FindComponentByClass<UBlackClockComponent>())
    {
        return BlackClock->EnterMemoryReconstruction(MemoryAnchorTag, OriginalMemoryOwnerNPC);
    }
    return false;
}
