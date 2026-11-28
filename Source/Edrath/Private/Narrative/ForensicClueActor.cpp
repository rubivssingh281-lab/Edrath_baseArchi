#include "Narrative/ForensicClueActor.h"
#include "BlackClock/BlackClockComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Core/EdrathTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathForensicSubsystem.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AForensicClueActor::AForensicClueActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AForensicClueActor::BeginPlay()
{
    Super::BeginPlay();
    CurrentDescription = InitialDescription;
    if (UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem())
    {
        Narrative->AddOrMutateEvidence(EvidenceTag, CurrentDescription, bIsTwistClue);
    }
}

UEdrathNarrativeSubsystem* AForensicClueActor::GetNarrativeSubsystem() const
{
    if (const UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
        }
    }
    return nullptr;
}

bool AForensicClueActor::IsInspectionAllowed(AActor* Inspector) const
{
    if (!Inspector || !GetWorld())
    {
        return false;
    }

    const bool bCloseEnough = FVector::DistSquared(GetActorLocation(), Inspector->GetActorLocation()) <=
        FMath::Square(FMath::Max(0.0f, InspectionRadius));
    if (!bRequiresInteriorCamera)
    {
        return bCloseEnough;
    }

    const UEdrathDualCameraComponent* DualCamera = Inspector->FindComponentByClass<UEdrathDualCameraComponent>();
    const bool bInteriorCamera = DualCamera &&
        DualCamera->GetCurrentCameraMode() == EEdrathCameraMode::InteriorThirdPerson;
    return bInteriorCamera || bCloseEnough;
}

bool AForensicClueActor::Inspect(AActor* Inspector)
{
    if (!IsInspectionAllowed(Inspector))
    {
        return false;
    }

    bHasBeenInspected = true;
    OnForensicClueInspected.Broadcast(EvidenceTag, Inspector);
    if (UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem())
    {
        Narrative->AddOrMutateEvidence(EvidenceTag, CurrentDescription, bIsTwistClue);
    }
    if (GetWorld() && GetWorld()->GetGameInstance())
    {
        if (UEdrathForensicSubsystem* Forensics = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathForensicSubsystem>())
        {
            Forensics->RegisterDiscoveredClue(EvidenceTag, EEdrathRegion::Veyl, CurrentDescription);
        }
    }
    return true;
}

void AForensicClueActor::BindToBlackClock(UBlackClockComponent* BlackClock)
{
    if (BlackClock)
    {
        BlackClock->OnTimelineStateShifted.AddUniqueDynamic(
            this, &AForensicClueActor::HandleTimelineStateShifted);
    }
}

void AForensicClueActor::ApplyDescriptionForTimeline(ETimelineState NewState)
{
    if (NewState == ETimelineState::RejectedTimeline && !RejectedTimelineDescription.IsEmpty())
    {
        CurrentDescription = RejectedTimelineDescription;
    }
    else if (NewState == ETimelineState::MemoryReconstruction && !MemoryReconstructionDescription.IsEmpty())
    {
        CurrentDescription = MemoryReconstructionDescription;
    }
    else if (NewState == ETimelineState::PresentTimeline && !InitialDescription.IsEmpty())
    {
        CurrentDescription = InitialDescription;
    }

    if (UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem())
    {
        Narrative->AddOrMutateEvidence(EvidenceTag, CurrentDescription, bIsTwistClue);
    }
}

void AForensicClueActor::HandleTimelineStateShifted(ETimelineState NewState, FVector Epicenter, float WaveRadius)
{
    if (FVector::DistSquared(GetActorLocation(), Epicenter) <= FMath::Square(FMath::Max(0.0f, WaveRadius)))
    {
        ApplyDescriptionForTimeline(NewState);
    }
}

bool AForensicClueActor::CanInteract_Implementation(AActor* Interactor) const
{
    return IsInspectionAllowed(Interactor);
}

FText AForensicClueActor::GetInteractionPrompt_Implementation(AActor*) const
{
    return FText::FromString(bHasBeenInspected ? TEXT("[E] Re-Examine Evidence") : TEXT("[E] Inspect Forensic Clue"));
}

FText AForensicClueActor::GetInteractionTargetName_Implementation() const
{
    return EvidenceTag.IsValid() ? FText::FromName(EvidenceTag.GetTagName()) : FText::FromString(TEXT("Forensic Clue"));
}

bool AForensicClueActor::ExecuteInteraction_Implementation(AActor* Interactor)
{
    return Inspect(Interactor);
}
