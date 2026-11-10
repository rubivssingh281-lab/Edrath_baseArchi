#include "UI/ForensicContradictionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Core/EdrathGameplayTags.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UForensicContradictionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SlottedClueA = FGameplayTag::EmptyTag;
    SlottedClueB = FGameplayTag::EmptyTag;

    RegisterDefaultDeductionRules();
    ClearDeductionSlots();
}

void UForensicContradictionWidget::RegisterDefaultDeductionRules()
{
    DeductionRules.Empty();

    // Rule 1: Act 1 Crypt Priest Contradiction
    // Living Priest testimony in settlement vs Skeletal corpse verified dead 114 years in crypt
    {
        FContradictionDeductionRule Rule1;
        Rule1.PrimaryClueTag = EdrathGameplayTags::QuestAct1Crypt;
        Rule1.ContradictoryClueTag = EdrathGameplayTags::Twist1;
        Rule1.BreakthroughTag = EdrathGameplayTags::Twist1;
        Rule1.bBreaksSettlementLoop = true;
        Rule1.ResolutionNarrative = FText::FromString(
            TEXT("CONTRADICTION RESOLVED: The settlement priest speaking to Kael is an echo anchored to a 114-year-old memory. The settlement is looping in time."));
        AddDeductionRule(Rule1);
    }

    // Rule 2: Ilyra Executioner Blade Contradiction
    // Scratched portrait in Kael's house vs Executioner's notch blade residue
    {
        FContradictionDeductionRule Rule2;
        Rule2.PrimaryClueTag = EdrathGameplayTags::Twist2;
        Rule2.ContradictoryClueTag = EdrathGameplayTags::Twist1;
        Rule2.BreakthroughTag = EdrathGameplayTags::Twist2;
        Rule2.bBreaksSettlementLoop = false;
        Rule2.ResolutionNarrative = FText::FromString(
            TEXT("CONTRADICTION RESOLVED: Ilyra has been eliminating witnesses who remember catastrophic versions of Kael across timeline cycles."));
        AddDeductionRule(Rule2);
    }

    // Rule 3: The Wedding That Hasn't Happened Contradiction
    // Two strangers in village vs 40 years of shared marriage memories
    {
        FContradictionDeductionRule Rule3;
        Rule3.PrimaryClueTag = EdrathGameplayTags::QuestWeddingNotHappened;
        Rule3.ContradictoryClueTag = EdrathGameplayTags::QuestWeddingMergeTimelines;
        Rule3.BreakthroughTag = EdrathGameplayTags::QuestWeddingMergeTimelines;
        Rule3.bBreaksSettlementLoop = false;
        Rule3.ResolutionNarrative = FText::FromString(
            TEXT("CONTRADICTION RESOLVED: The marriage memories belong to an alternate timeline unanchored by the Black Clock defect."));
        AddDeductionRule(Rule3);
    }

    // Rule 4: The Last Honest King Contradiction
    // King confession vs Erased true perpetrator
    {
        FContradictionDeductionRule Rule4;
        Rule4.PrimaryClueTag = EdrathGameplayTags::QuestLastHonestKing;
        Rule4.ContradictoryClueTag = EdrathGameplayTags::QuestLastKingExposeTruth;
        Rule4.BreakthroughTag = EdrathGameplayTags::QuestLastKingExposeTruth;
        Rule4.bBreaksSettlementLoop = false;
        Rule4.ResolutionNarrative = FText::FromString(
            TEXT("CONTRADICTION RESOLVED: King confessing to war crimes committed by an erased general to anchor civil stability."));
        AddDeductionRule(Rule4);
    }
}

void UForensicContradictionWidget::AddDeductionRule(const FContradictionDeductionRule& InRule)
{
    DeductionRules.Add(InRule);
}

bool UForensicContradictionWidget::SlotPrimaryClue(FGameplayTag ClueTag)
{
    if (!ClueTag.IsValid())
    {
        return false;
    }

    SlottedClueA = ClueTag;
    if (SlotALabel)
    {
        SlotALabel->SetText(FText::FromName(SlottedClueA.GetTagName()));
    }
    return true;
}

bool UForensicContradictionWidget::SlotContradictoryClue(FGameplayTag ClueTag)
{
    if (!ClueTag.IsValid())
    {
        return false;
    }

    SlottedClueB = ClueTag;
    if (SlotBLabel)
    {
        SlotBLabel->SetText(FText::FromName(SlottedClueB.GetTagName()));
    }
    return true;
}

FContradictionResolutionResult UForensicContradictionWidget::AttemptResolveContradiction()
{
    FContradictionResolutionResult Result;
    Result.bSuccess = false;

    if (!AreBothSlotsFilled())
    {
        Result.RevelationTitle = FText::FromString(TEXT("Incomplete Deduction"));
        Result.RevelationBody = FText::FromString(TEXT("Both evidence slots must be filled to cross-reference statements."));
        return Result;
    }

    for (const FContradictionDeductionRule& Rule : DeductionRules)
    {
        const bool bMatchForward = (SlottedClueA == Rule.PrimaryClueTag && SlottedClueB == Rule.ContradictoryClueTag);
        const bool bMatchReverse = (SlottedClueA == Rule.ContradictoryClueTag && SlottedClueB == Rule.PrimaryClueTag);

        if (bMatchForward || bMatchReverse)
        {
            Result.bSuccess = true;
            Result.BreakthroughTag = Rule.BreakthroughTag;
            Result.RevelationTitle = FText::FromString(TEXT("Contradiction Breakthrough!"));
            Result.RevelationBody = Rule.ResolutionNarrative;

            ResolvedBreakthroughs.AddUnique(Rule.BreakthroughTag);

            if (ResolutionResultLabel)
            {
                ResolutionResultLabel->SetText(Result.RevelationBody);
            }

            // If this breakthrough breaks the settlement loop, notify the narrative subsystem
            if (Rule.bBreaksSettlementLoop && GetWorld() && GetWorld()->GetGameInstance())
            {
                if (UEdrathNarrativeSubsystem* NarrativeSys = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
                {
                    NarrativeSys->BreakAct1SettlementLoop();
                }
            }

            OnContradictionResolved.Broadcast(Result);
            return Result;
        }
    }

    // No matching hypothesis found
    Result.RevelationTitle = FText::FromString(TEXT("No Contradiction Found"));
    Result.RevelationBody = FText::FromString(TEXT("These two pieces of evidence do not exhibit a direct ontological contradiction."));

    if (ResolutionResultLabel)
    {
        ResolutionResultLabel->SetText(Result.RevelationBody);
    }

    OnContradictionFailed.Broadcast(SlottedClueA, SlottedClueB);
    return Result;
}

void UForensicContradictionWidget::ClearDeductionSlots()
{
    SlottedClueA = FGameplayTag::EmptyTag;
    SlottedClueB = FGameplayTag::EmptyTag;

    if (SlotALabel)
    {
        SlotALabel->SetText(FText::FromString(TEXT("[Select Primary Clue]")));
    }

    if (SlotBLabel)
    {
        SlotBLabel->SetText(FText::FromString(TEXT("[Select Conflicting Clue]")));
    }

    if (ResolutionResultLabel)
    {
        ResolutionResultLabel->SetText(FText::GetEmpty());
    }
}

bool UForensicContradictionWidget::HasBreakthroughBeenResolved(FGameplayTag BreakthroughTag) const
{
    return ResolvedBreakthroughs.Contains(BreakthroughTag);
}
