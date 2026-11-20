#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/EchoCombatComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

UBlackClockComponent::UBlackClockComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBlackClockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (ActiveTimelineState == ETimelineState::PresentTimeline)
    {
        CurrentClockStability = FMath::Min(MaxClockStability, CurrentClockStability + StabilityRegenPerSecond * DeltaTime);
    }
    else if (ActiveTimelineState == ETimelineState::RejectedTimeline)
    {
        CurrentClockStability = FMath::Max(0.0f, CurrentClockStability - 2.5f * DeltaTime);
        if (CurrentClockStability <= 0.0f)
        {
            TogglePossibilityShift(EEdrathRegion::Veyl);
        }
    }
}

bool UBlackClockComponent::TogglePossibilityShift(EEdrathRegion CurrentRegion)
{
    AActor* Owner = GetOwner();
    if (!Owner || !GetWorld())
    {
        return false;
    }
    if (ActiveTimelineState == ETimelineState::PresentTimeline)
    {
        if (CurrentClockStability < PossibilityShiftStabilityCost)
        {
            return false;
        }
        CurrentClockStability -= PossibilityShiftStabilityCost;
        ActiveTimelineState = ETimelineState::RejectedTimeline;
        if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
        {
            if (UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                Narrative->RegisterBlackClockActivation(CurrentRegion, 5.0f);
            }
        }
    }
    else
    {
        ActiveTimelineState = ETimelineState::PresentTimeline;
    }
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        if (UEdrathHardwareOptimizationSubsystem* HardwareOptimization =
            GameInstance->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
        {
            HardwareOptimization->OptimizeTimelineMemoryFootprint(ActiveTimelineState);
        }
    }

    TArray<AActor*> ShiftActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APossibilityShiftActor::StaticClass(), ShiftActors);
    for (AActor* Actor : ShiftActors)
    {
        if (APossibilityShiftActor* ShiftActor = Cast<APossibilityShiftActor>(Actor))
        {
            if (FVector::Dist(Owner->GetActorLocation(), ShiftActor->GetActorLocation()) <= PossibilityWaveMaxRadius)
            {
                ShiftActor->ApplyTimelineState(ActiveTimelineState);
            }
        }
    }
    OnTimelineStateShifted.Broadcast(ActiveTimelineState, Owner->GetActorLocation(), PossibilityWaveMaxRadius);
    return true;
}

bool UBlackClockComponent::EnterMemoryReconstruction(FGameplayTag MemoryAnchorTag, FName OriginalMemoryOwnerNPC)
{
    if (!GetWorld() || !GetWorld()->GetGameInstance())
    {
        return false;
    }
    ActiveTimelineState = ETimelineState::MemoryReconstruction;
    if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
    {
        Narrative->RecordRecoveredMemory(MemoryAnchorTag, OriginalMemoryOwnerNPC);
    }
    if (UEdrathHardwareOptimizationSubsystem* HardwareOptimization =
        GetWorld()->GetGameInstance()->GetSubsystem<UEdrathHardwareOptimizationSubsystem>())
    {
        HardwareOptimization->OptimizeTimelineMemoryFootprint(ActiveTimelineState);
    }
    OnTimelineStateShifted.Broadcast(ActiveTimelineState, GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector, PossibilityWaveMaxRadius);
    return true;
}

void UBlackClockComponent::RecordCombatFrame(FName MontageSection, float BaseDamage)
{
    if (!GetOwner() || !GetWorld())
    {
        return;
    }
    FEchoActionFrame Frame;
    Frame.WorldPosition = GetOwner()->GetActorLocation();
    Frame.WorldRotation = GetOwner()->GetActorRotation();
    Frame.ActionMontageSection = MontageSection;
    Frame.BaseDamage = BaseDamage;
    Frame.TimestampSeconds = GetWorld()->GetTimeSeconds();
    if (ActionHistoryBuffer.Num() >= MaxBufferFrames)
    {
        ActionHistoryBuffer.RemoveAt(0);
    }
    ActionHistoryBuffer.Add(Frame);
}

bool UBlackClockComponent::InterpolateFrameAtTime(float TargetTimestamp, FEchoActionFrame& OutFrame) const
{
    if (ActionHistoryBuffer.IsEmpty())
    {
        return false;
    }

    // Boundary edge cases: clamped to buffer bounds
    if (ActionHistoryBuffer.Num() == 1 || TargetTimestamp <= ActionHistoryBuffer[0].TimestampSeconds)
    {
        OutFrame = ActionHistoryBuffer[0];
        return true;
    }

    if (TargetTimestamp >= ActionHistoryBuffer.Last().TimestampSeconds)
    {
        OutFrame = ActionHistoryBuffer.Last();
        return true;
    }

    // Scan for bounding adjacent frames
    for (int32 Index = 0; Index < ActionHistoryBuffer.Num() - 1; ++Index)
    {
        const FEchoActionFrame& FrameA = ActionHistoryBuffer[Index];
        const FEchoActionFrame& FrameB = ActionHistoryBuffer[Index + 1];

        if (TargetTimestamp >= FrameA.TimestampSeconds && TargetTimestamp <= FrameB.TimestampSeconds)
        {
            const float TimeDelta = FrameB.TimestampSeconds - FrameA.TimestampSeconds;
            const float Alpha = (TimeDelta > KINDA_SMALL_NUMBER)
                ? FMath::Clamp((TargetTimestamp - FrameA.TimestampSeconds) / TimeDelta, 0.0f, 1.0f)
                : 0.0f;

            OutFrame.WorldPosition = FMath::Lerp(FrameA.WorldPosition, FrameB.WorldPosition, Alpha);
            OutFrame.WorldRotation = FQuat::Slerp(FrameA.WorldRotation.Quaternion(), FrameB.WorldRotation.Quaternion(), Alpha).Rotator();
            OutFrame.BaseDamage = FMath::Lerp(FrameA.BaseDamage, FrameB.BaseDamage, Alpha);
            OutFrame.ActionMontageSection = (Alpha < 0.5f) ? FrameA.ActionMontageSection : FrameB.ActionMontageSection;
            OutFrame.TimestampSeconds = TargetTimestamp;
            return true;
        }
    }

    OutFrame = ActionHistoryBuffer.Last();
    return true;
}

void UBlackClockComponent::RestoreStability(float Amount)
{
    CurrentClockStability = FMath::Clamp(CurrentClockStability + Amount, 0.0f, MaxClockStability);
}

bool UBlackClockComponent::TriggerEchoStrike(float SecondsAgo, EEdrathRegion CurrentRegion, FEchoActionFrame& OutEchoFrame)
{
    if (CurrentClockStability < EchoStrikeStabilityCost || ActionHistoryBuffer.IsEmpty() || !GetWorld())
    {
        return false;
    }
    const float TargetTimestamp = GetWorld()->GetTimeSeconds() - FMath::Max(0.0f, SecondsAgo);

    if (!InterpolateFrameAtTime(TargetTimestamp, OutEchoFrame))
    {
        return false;
    }

    CurrentClockStability -= EchoStrikeStabilityCost;

    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RegisterBlackClockActivation(CurrentRegion, 2.0f);
        }
    }
    OnEchoGhostSpawned.Broadcast(OutEchoFrame);
    if (GetOwner())
    {
        if (UEdrathEchoCombatComponent* EchoCombat = GetOwner()->FindComponentByClass<UEdrathEchoCombatComponent>())
        {
            EchoCombat->SpawnVisualGhostStrike(OutEchoFrame);
        }
    }
    return true;
}
