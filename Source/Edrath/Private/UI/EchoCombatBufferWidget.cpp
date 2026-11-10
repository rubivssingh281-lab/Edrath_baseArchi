#include "UI/EchoCombatBufferWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "BlackClock/BlackClockComponent.h"

void UEchoCombatBufferWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StoredFrames = 0;
    MaxFramesCapacity = 120;
    BufferFillRatio = 0.0f;
    StrikeDelaySeconds = 1.2f;
    bIsStrikeReady = true;
    CooldownRemaining = 0.0f;
    CooldownTotal = 5.0f;
    CooldownPercent = 0.0f;
    ActiveRegion = EEdrathRegion::Veyl;
    bInstabilityWarningActive = false;
    CurrentInstability = 0.0f;

    UpdateBufferMetrics(0, 120);
    SetRegionalResonance(EEdrathRegion::Veyl);
    UpdateCooldown(0.0f, 5.0f);
}

void UEchoCombatBufferWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (CooldownRemaining > 0.0f)
    {
        CooldownRemaining = FMath::Max(0.0f, CooldownRemaining - InDeltaTime);
        UpdateCooldown(CooldownRemaining, CooldownTotal);
    }
}

void UEchoCombatBufferWidget::BindToBlackClock(UBlackClockComponent* InBlackClock)
{
    if (!InBlackClock)
    {
        return;
    }

    UpdateBufferMetrics(InBlackClock->GetCombatBufferFrameCount(), 120);
}

void UEchoCombatBufferWidget::UpdateBufferMetrics(int32 InStoredFrames, int32 InMaxFrames)
{
    StoredFrames = FMath::Clamp(InStoredFrames, 0, InMaxFrames);
    MaxFramesCapacity = FMath::Max(1, InMaxFrames);
    BufferFillRatio = static_cast<float>(StoredFrames) / static_cast<float>(MaxFramesCapacity);

    if (BufferProgressBar)
    {
        BufferProgressBar->SetPercent(BufferFillRatio);
    }

    if (StoredFramesLabel)
    {
        const FText FrameText = FText::Format(
            NSLOCTEXT("EdrathUI", "EchoBufferFrames", "{0}/{1} ({2}s)"),
            FText::AsNumber(StoredFrames),
            FText::AsNumber(MaxFramesCapacity),
            FText::AsNumber(FMath::RoundToFloat(BufferFillRatio * 2.0f * 10.0f) / 10.0f));
        StoredFramesLabel->SetText(FrameText);
    }

    OnEchoBufferUpdated.Broadcast(StoredFrames, BufferFillRatio);
}

void UEchoCombatBufferWidget::PushActionRecord(FName MontageSectionName, float ActionDamage)
{
    if (MontageSectionName.IsNone())
    {
        return;
    }

    RecentActions.Insert(MontageSectionName, 0);
    if (RecentActions.Num() > MaxRecentActionsDisplayed)
    {
        RecentActions.SetNum(MaxRecentActionsDisplayed);
    }
}

void UEchoCombatBufferWidget::SetStrikeDelay(float InDelaySeconds)
{
    StrikeDelaySeconds = FMath::Clamp(InDelaySeconds, 0.2f, 2.0f);

    if (StrikeDelayLabel)
    {
        const FText DelayText = FText::Format(
            NSLOCTEXT("EdrathUI", "EchoStrikeDelay", "Echo Delay: -{0}s"),
            FText::AsNumber(StrikeDelaySeconds));
        StrikeDelayLabel->SetText(DelayText);
    }
}

void UEchoCombatBufferWidget::UpdateCooldown(float RemainingTime, float TotalCooldownDuration)
{
    CooldownRemaining = FMath::Max(0.0f, RemainingTime);
    CooldownTotal = FMath::Max(0.1f, TotalCooldownDuration);
    CooldownPercent = FMath::Clamp(1.0f - (CooldownRemaining / CooldownTotal), 0.0f, 1.0f);
    bIsStrikeReady = (CooldownRemaining <= KINDA_SMALL_NUMBER);

    if (CooldownProgressBar)
    {
        CooldownProgressBar->SetPercent(CooldownPercent);
    }

    OnEchoStrikeCooldownChanged.Broadcast(CooldownRemaining, bIsStrikeReady);
}

void UEchoCombatBufferWidget::SetRegionalResonance(EEdrathRegion InRegion)
{
    ActiveRegion = InRegion;
    const FLinearColor RegionColor = GetResonanceColorForRegion(ActiveRegion);

    if (ResonanceGlowImage)
    {
        ResonanceGlowImage->SetColorAndOpacity(RegionColor);
    }

    if (RegionalResonanceLabel)
    {
        FString RegionName;
        switch (ActiveRegion)
        {
        case EEdrathRegion::Veyl:          RegionName = TEXT("Veyl (Silver Echo)"); break;
        case EEdrathRegion::Hollowgreen:   RegionName = TEXT("Hollowgreen (Verdant Echo)"); break;
        case EEdrathRegion::AshenExpanse:  RegionName = TEXT("Ashen Expanse (Shadowless Echo)"); break;
        case EEdrathRegion::Nhal:          RegionName = TEXT("Nhal (Obsidian Echo)"); break;
        case EEdrathRegion::CrownOfWinter: RegionName = TEXT("Crown of Winter (Cryo Echo)"); break;
        case EEdrathRegion::UnwrittenSea:  RegionName = TEXT("Unwritten Sea (Astral Echo)"); break;
        }
        RegionalResonanceLabel->SetText(FText::FromString(RegionName));
    }

    OnRegionalResonanceChanged.Broadcast(ActiveRegion);
}

void UEchoCombatBufferWidget::SetInstabilityWarning(bool bShowWarning, float InstabilityLevel)
{
    bInstabilityWarningActive = bShowWarning;
    CurrentInstability = InstabilityLevel;

    if (InstabilityWarningBadge)
    {
        InstabilityWarningBadge->SetVisibility(bShowWarning ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
    }
}

FLinearColor UEchoCombatBufferWidget::GetResonanceColorForRegion(EEdrathRegion InRegion) const
{
    switch (InRegion)
    {
    case EEdrathRegion::Veyl:
        return FLinearColor(0.85f, 0.90f, 1.0f, 1.0f); // Silver-white blue
    case EEdrathRegion::Hollowgreen:
        return FLinearColor(0.15f, 0.95f, 0.45f, 1.0f); // Verdant emerald
    case EEdrathRegion::AshenExpanse:
        return FLinearColor(0.80f, 0.78f, 0.75f, 0.7f); // Shadowless ash
    case EEdrathRegion::Nhal:
        return FLinearColor(0.65f, 0.20f, 0.95f, 1.0f); // Obsidian violet
    case EEdrathRegion::CrownOfWinter:
        return FLinearColor(0.35f, 0.75f, 1.0f, 1.0f); // Frost cyan
    case EEdrathRegion::UnwrittenSea:
        return FLinearColor(0.95f, 0.85f, 0.35f, 1.0f); // Solar golden-amber
    default:
        return FLinearColor::White;
    }
}
