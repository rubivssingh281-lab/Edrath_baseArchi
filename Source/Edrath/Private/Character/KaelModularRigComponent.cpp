#include "Character/KaelModularRigComponent.h"

UKaelModularRigComponent::UKaelModularRigComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UKaelModularRigComponent::SetLifeEchoArchetype(EKaelLifeEchoArchetype NewArchetype)
{
    if (CurrentArchetype != NewArchetype)
    {
        CurrentArchetype = NewArchetype;
        OnOutfitChanged.Broadcast();
    }
}
