#include "BlackClock/EchoCombatComponent.h"
#include "Character/KaelCharacter.h"
#include "BlackClock/BlackClockComponent.h"

UEdrathEchoCombatComponent::UEdrathEchoCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.0f;
}

void UEdrathEchoCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CurrentState != ECombatActionState::Idle)
    {
        StateTimeRemaining -= DeltaTime;
        if (StateTimeRemaining <= 0.0f)
        {
            SetCombatState(ECombatActionState::Idle, 0.0f);
        }
    }
}

void UEdrathEchoCombatComponent::SetCombatState(ECombatActionState NewState, float Duration)
{
    CurrentState = NewState;
    StateTimeRemaining = Duration;

    if (OnCombatActionStateChanged.IsBound())
    {
        OnCombatActionStateChanged.Broadcast(CurrentState);
    }
}

bool UEdrathEchoCombatComponent::ExecuteAttackCombo(int32 ComboIndex, float BaseDamage)
{
    if (CurrentState == ECombatActionState::Dodging || CurrentState == ECombatActionState::Staggered)
    {
        return false;
    }

    if (AKaelCharacter* Kael = Cast<AKaelCharacter>(GetOwner()))
    {
        if (!Kael->ConsumeStamina(AttackStaminaCost))
        {
            return false;
        }

        const FName SectionName = FName(*FString::Printf(TEXT("Slash_Combo_%02d"), FMath::Clamp(ComboIndex, 1, 3)));
        if (Kael->BlackClockComponent)
        {
            Kael->BlackClockComponent->RecordCombatFrame(SectionName, BaseDamage);
        }

        SetCombatState(ECombatActionState::Attacking, 0.45f);
        return true;
    }

    return false;
}

bool UEdrathEchoCombatComponent::ExecuteDodgeRoll(float InDodgeDuration)
{
    if (CurrentState == ECombatActionState::Staggered)
    {
        return false;
    }

    if (AKaelCharacter* Kael = Cast<AKaelCharacter>(GetOwner()))
    {
        if (!Kael->ConsumeStamina(DodgeStaminaCost))
        {
            return false;
        }

        SetCombatState(ECombatActionState::Dodging, FMath::Max(0.2f, InDodgeDuration));
        return true;
    }

    return false;
}

bool UEdrathEchoCombatComponent::ExecuteParry(float InParryWindow)
{
    if (CurrentState != ECombatActionState::Idle)
    {
        return false;
    }

    if (AKaelCharacter* Kael = Cast<AKaelCharacter>(GetOwner()))
    {
        if (!Kael->ConsumeStamina(10.0f))
        {
            return false;
        }

        SetCombatState(ECombatActionState::Parrying, FMath::Clamp(InParryWindow, 0.15f, 0.4f));
        return true;
    }

    return false;
}

bool UEdrathEchoCombatComponent::CheckAndResolveIncomingHit(float IncomingDamage, AActor* Attacker)
{
    if (IsInvulnerable())
    {
        return false;
    }

    if (IsInParryWindow())
    {
        SetCombatState(ECombatActionState::Idle, 0.0f);
        if (OnSuccessfulParry.IsBound())
        {
            OnSuccessfulParry.Broadcast();
        }
        return false;
    }

    return true;
}

void UEdrathEchoCombatComponent::SpawnVisualGhostStrike(const FEchoActionFrame& EchoFrame)
{
    LastGhostStrike = EchoFrame;
    if (OnVisualGhostStrikeRequested.IsBound())
    {
        OnVisualGhostStrikeRequested.Broadcast(LastGhostStrike);
    }
}
