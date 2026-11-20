#include "Character/EdrathCompanionCharacter.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AEdrathCompanionCharacter::AEdrathCompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEdrathCompanionCharacter::BeginPlay()
{
    Super::BeginPlay();
    RelationshipState.CompanionID = CompanionID;
}

void AEdrathCompanionCharacter::SetAutonomousState(float NewLoyalty, float NewSuspicion, bool bNewWithholdingTruth)
{
    RelationshipState.Loyalty = FMath::Clamp(NewLoyalty, 0.0f, 100.0f);
    RelationshipState.SuspicionOfKael = FMath::Clamp(NewSuspicion, 0.0f, 100.0f);
    RelationshipState.bWithholdingCriticalTruth = bNewWithholdingTruth;
    RelationshipState.bHasTurnedHostileInNhal = RelationshipState.Loyalty <= 0.0f;
}

bool AEdrathCompanionCharacter::ResolveIlyraOutcome(EIlyraResolution NewResolution)
{
    if (CompanionID != TEXT("Ilyra") || NewResolution == EIlyraResolution::None ||
        IlyraResolutionState != EIlyraResolution::None)
    {
        return false;
    }

    IlyraResolutionState = NewResolution;
    if (NewResolution == EIlyraResolution::Reconciliation)
    {
        SetAutonomousState(85.0f, 20.0f, false);
    }
    else if (NewResolution == EIlyraResolution::Imprisonment)
    {
        SetAutonomousState(35.0f, 65.0f, true);
    }
    else
    {
        SetAutonomousState(0.0f, 100.0f, false);
    }
    OnIlyraResolutionChanged.Broadcast(IlyraResolutionState);
    return true;
}

bool AEdrathCompanionCharacter::GenerateIlyraBladeNotchClue()
{
    if (CompanionID != TEXT("Ilyra") || !GetWorld() || !GetWorld()->GetGameInstance())
    {
        return false;
    }
    if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
    {
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::Twist2,
            FText::FromString(TEXT("Execution wounds carry the notch pattern of Ilyra's blade.")),
            true);
        return true;
    }
    return false;
}

bool AEdrathCompanionCharacter::AssistMemoryReconstruction(FGameplayTag MemoryTag, FText OverwrittenDescription)
{
    if (CompanionID != TEXT("Orren") || !MemoryTag.IsValid() || !GetWorld() || !GetWorld()->GetGameInstance())
    {
        return false;
    }
    if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
    {
        Narrative->AddOrMutateEvidence(MemoryTag, MoveTemp(OverwrittenDescription), true);
        bHasOverwrittenHistoricalEvidence = true;
        return true;
    }
    return false;
}

void AEdrathCompanionCharacter::TriggerForesightWarning(float WarningSeconds)
{
    if (CompanionID != TEXT("Senn"))
    {
        return;
    }
    LastForesightWarningSeconds = FMath::Clamp(WarningSeconds, 0.0f, 5.0f);
    OnSennForesightWarning.Broadcast(LastForesightWarningSeconds);
}

void AEdrathCompanionCharacter::SetActVIChoiceKnowledge(FText ChoiceKnowledge)
{
    if (CompanionID == TEXT("Senn"))
    {
        ActVIChoiceKnowledge = MoveTemp(ChoiceKnowledge);
        bKnowsActVIChoice = !ActVIChoiceKnowledge.IsEmpty();
    }
}

void AEdrathCompanionCharacter::SetHostilePursuer(bool bNewHostilePursuer)
{
    if (CompanionID == TEXT("Maevra"))
    {
        bIsHostilePursuer = bNewHostilePursuer;
        if (bIsHostilePursuer)
        {
            bIsPartyMember = false;
            SetAutonomousState(15.0f, 80.0f, true);
        }
    }
}

bool AEdrathCompanionCharacter::RecruitMaevraToParty()
{
    if (CompanionID != TEXT("Maevra") || !bIsHostilePursuer ||
        RecoveredMarriageMemoryCount < RequiredMarriageMemoryCount)
    {
        return false;
    }
    bIsHostilePursuer = false;
    bIsPartyMember = true;
    SetAutonomousState(70.0f, 25.0f, false);
    return true;
}

void AEdrathCompanionCharacter::SetErasedMarriageRestorationProgress(int32 RecoveredMemoryCount, int32 RequiredMemoryCount)
{
    if (CompanionID == TEXT("Maevra"))
    {
        RecoveredMarriageMemoryCount = FMath::Max(0, RecoveredMemoryCount);
        RequiredMarriageMemoryCount = FMath::Max(1, RequiredMemoryCount);
    }
}
