#include "BlackClock/EchoCombatComponent.h"

UEdrathEchoCombatComponent::UEdrathEchoCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEdrathEchoCombatComponent::SpawnVisualGhostStrike(const FEchoActionFrame& EchoFrame)
{
    LastGhostStrike = EchoFrame;
    OnVisualGhostStrikeRequested.Broadcast(LastGhostStrike);
}
