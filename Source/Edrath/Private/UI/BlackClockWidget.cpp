#include "UI/BlackClockWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BlackClock/BlackClockComponent.h"
#include "Character/KaelCharacter.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UBlackClockWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!BoundClockComponent.IsValid())
    {
        if (AKaelCharacter* Kael = Cast<AKaelCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
        {
            if (Kael->BlackClockComponent)
            {
                BindToClockComponent(Kael->BlackClockComponent);
            }
        }
    }

    if (const UWorld* World = GetWorld())
    {
        if (const UGameInstance* GI = World->GetGameInstance())
        {
            if (const UEdrathNarrativeSubsystem* Narrative = GI->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                UpdateRegionalInstability(Narrative->GetRegionalInstabilityMap().FindRef(EEdrathRegion::Veyl));
            }
        }
    }

    UpdateTimelineState(DisplayedTimelineState);
    UpdateClockStability(100.0f, 100.0f);
}

void UBlackClockWidget::NativeDestruct()
{
    if (BoundClockComponent.IsValid())
    {
        BoundClockComponent->OnTimelineStateShifted.RemoveDynamic(this, &UBlackClockWidget::HandleClockTimelineShifted);
        BoundClockComponent.Reset();
    }

    Super::NativeDestruct();
}

void UBlackClockWidget::BindToClockComponent(UBlackClockComponent* InClockComponent)
{
    if (!InClockComponent)
    {
        return;
    }

    if (BoundClockComponent.IsValid())
    {
        BoundClockComponent->OnTimelineStateShifted.RemoveDynamic(this, &UBlackClockWidget::HandleClockTimelineShifted);
    }

    BoundClockComponent = InClockComponent;
    InClockComponent->OnTimelineStateShifted.AddUniqueDynamic(this, &UBlackClockWidget::HandleClockTimelineShifted);

    UpdateTimelineState(InClockComponent->GetActiveTimelineState());
    UpdateClockStability(InClockComponent->GetClockStability(), 100.0f);
}

void UBlackClockWidget::UpdateClockStability(float CurrentStability, float MaxStability)
{
    const float SafeMax = FMath::Max(1.0f, MaxStability);
    const float Ratio = FMath::Clamp(CurrentStability / SafeMax, 0.0f, 1.0f);
    DisplayedStabilityPercent = Ratio * 100.0f;

    if (StabilityProgressBar)
    {
        StabilityProgressBar->SetPercent(Ratio);
    }

    if (StabilityPercentLabel)
    {
        const FText PercentText = FText::Format(
            NSLOCTEXT("EdrathUI", "ClockPercentFmt", "{0}%"),
            FText::AsNumber(FMath::RoundToInt(DisplayedStabilityPercent)));
        StabilityPercentLabel->SetText(PercentText);
    }
}

void UBlackClockWidget::UpdateTimelineState(ETimelineState NewTimelineState)
{
    DisplayedTimelineState = NewTimelineState;

    if (TimelineStateLabel)
    {
        TimelineStateLabel->SetText(GetTimelineStateDisplayName(NewTimelineState));
    }

    OnTimelineVisualChanged.Broadcast(NewTimelineState);
    OnTimelineThemeChanged(NewTimelineState);
}

void UBlackClockWidget::UpdateRegionalInstability(float InstabilityScore)
{
    DisplayedRegionalInstability = FMath::Clamp(InstabilityScore, 0.0f, 100.0f);

    if (RegionalInstabilityLabel)
    {
        const FText InstabilityText = FText::Format(
            NSLOCTEXT("EdrathUI", "RegionalInstabilityFmt", "Reality Anomaly: {0}%"),
            FText::AsNumber(FMath::RoundToInt(DisplayedRegionalInstability)));
        RegionalInstabilityLabel->SetText(InstabilityText);
    }
}

void UBlackClockWidget::HandleClockTimelineShifted(ETimelineState NewState, FVector Epicenter, float WaveRadius)
{
    UpdateTimelineState(NewState);

    if (BoundClockComponent.IsValid())
    {
        UpdateClockStability(BoundClockComponent->GetClockStability(), 100.0f);
    }
}

FText UBlackClockWidget::GetTimelineStateDisplayName(ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::PresentTimeline:
            return NSLOCTEXT("EdrathUI", "StatePresent", "Present Timeline");
        case ETimelineState::RejectedTimeline:
            return NSLOCTEXT("EdrathUI", "StateRejected", "Rejected Timeline (Shifted)");
        case ETimelineState::MemoryReconstruction:
            return NSLOCTEXT("EdrathUI", "StateMemory", "Memory Reconstruction Echo");
        default:
            return NSLOCTEXT("EdrathUI", "StateUnknown", "Timeline Indeterminate");
    }
}
