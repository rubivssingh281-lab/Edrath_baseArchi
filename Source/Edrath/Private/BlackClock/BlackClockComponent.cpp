#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/EchoCombatComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
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

bool UBlackClockComponent::TriggerEchoStrike(float SecondsAgo, EEdrathRegion CurrentRegion, FEchoActionFrame& OutEchoFrame)
{
    if (CurrentClockStability < EchoStrikeStabilityCost || ActionHistoryBuffer.IsEmpty() || !GetWorld())
    {
        return false;
    }
    const float TargetTimestamp = GetWorld()->GetTimeSeconds() - FMath::Max(0.0f, SecondsAgo);
    int32 BestIndex = 0;
    float SmallestDifference = TNumericLimits<float>::Max();
    for (int32 Index = 0; Index < ActionHistoryBuffer.Num(); ++Index)
    {
        const float Difference = FMath::Abs(ActionHistoryBuffer[Index].TimestampSeconds - TargetTimestamp);
        if (Difference < SmallestDifference)
        {
            SmallestDifference = Difference;
            BestIndex = Index;
        }
    }
    CurrentClockStability -= EchoStrikeStabilityCost;
    OutEchoFrame = ActionHistoryBuffer[BestIndex];
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        if (UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>())
        {
            Narrative->RegisterBlackClockActivation(CurrentRegion, 2.0f);
        }
    }
    OnEchoGhostSpawned.Broadcast(OutEchoFrame);
    if (UEdrathEchoCombatComponent* EchoCombat = GetOwner()->FindComponentByClass<UEdrathEchoCombatComponent>())
    {
        EchoCombat->SpawnVisualGhostStrike(OutEchoFrame);
    }
    return true;
}
