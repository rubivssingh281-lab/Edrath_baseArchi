#include "Character/KaelInteractionScannerComponent.h"
#include "Core/EdrathInteractableInterface.h"
#include "UI/EdrathHUD.h"
#include "UI/EdrathReticleWidget.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UKaelInteractionScannerComponent::UKaelInteractionScannerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.08f; // 12.5 Hz scan cadence preserves CPU budget
}

void UKaelInteractionScannerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ScanForInteractables();
}

AActor* UKaelInteractionScannerComponent::ScanForInteractables()
{
    AActor* OwnerActor = GetOwner();
    UWorld* World = GetWorld();
    if (!OwnerActor || !World)
    {
        return nullptr;
    }

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EdrathInteractableScan), false, OwnerActor);
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(FMath::Max(50.0f, InteractionScanRadius));

    World->OverlapMultiByChannel(
        Overlaps,
        OwnerActor->GetActorLocation(),
        FQuat::Identity,
        ECC_WorldDynamic,
        SphereShape,
        QueryParams);

    TArray<AActor*> Candidates;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        if (AActor* HitActor = Overlap.GetActor())
        {
            if (HitActor != OwnerActor && HitActor->Implements<UEdrathInteractableInterface>())
            {
                Candidates.AddUnique(HitActor);
            }
        }
    }

    return SelectBestCandidate(Candidates);
}

AActor* UKaelInteractionScannerComponent::SelectBestCandidate(const TArray<AActor*>& CandidateActors)
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return nullptr;
    }

    const FVector OwnerLoc = OwnerActor->GetActorLocation();
    const FVector OwnerForward = OwnerActor->GetActorForwardVector().GetSafeNormal();
    const float MaxDistSq = FMath::Square(FMath::Max(1.0f, InteractionScanRadius));

    AActor* BestActor = nullptr;
    float BestScore = -FLT_MAX;

    for (AActor* Candidate : CandidateActors)
    {
        if (!IsValid(Candidate) || Candidate == OwnerActor)
        {
            continue;
        }

        if (!Candidate->Implements<UEdrathInteractableInterface>())
        {
            continue;
        }

        if (!IEdrathInteractableInterface::Execute_CanInteract(Candidate, OwnerActor))
        {
            continue;
        }

        const FVector ToCandidate = Candidate->GetActorLocation() - OwnerLoc;
        const float DistSq = ToCandidate.SizeSquared();
        if (DistSq > MaxDistSq)
        {
            continue;
        }

        const float DistanceRatio = 1.0f - FMath::Clamp(FMath::Sqrt(DistSq) / InteractionScanRadius, 0.0f, 1.0f);
        const float Alignment = ToCandidate.IsNearlyZero()
            ? 1.0f
            : FVector::DotProduct(OwnerForward, ToCandidate.GetSafeNormal()) * 0.5f + 0.5f;

        const float Score = (1.0f - ForwardAlignmentWeight) * DistanceRatio + ForwardAlignmentWeight * Alignment;
        if (Score > BestScore)
        {
            BestScore = Score;
            BestActor = Candidate;
        }
    }

    if (FocusedInteractable.Get() != BestActor)
    {
        FocusedInteractable = BestActor;
        PushFocusToHUD(BestActor);
        OnFocusedInteractableChanged.Broadcast(BestActor);
    }

    return BestActor;
}

bool UKaelInteractionScannerComponent::TryInteractWithFocusedTarget()
{
    AActor* OwnerActor = GetOwner();
    AActor* Target = FocusedInteractable.Get();
    if (!OwnerActor || !IsValid(Target) || !Target->Implements<UEdrathInteractableInterface>())
    {
        return false;
    }

    if (!IEdrathInteractableInterface::Execute_CanInteract(Target, OwnerActor))
    {
        return false;
    }

    const bool bInteracted = IEdrathInteractableInterface::Execute_ExecuteInteraction(Target, OwnerActor);
    // Refresh focus immediately in case interaction consumed one-shot interactable
    ScanForInteractables();
    return bInteracted;
}

void UKaelInteractionScannerComponent::PushFocusToHUD(AActor* TargetActor)
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
    AEdrathHUD* HUD = PC ? Cast<AEdrathHUD>(PC->GetHUD()) : nullptr;
    UEdrathReticleWidget* Reticle = HUD ? HUD->GetReticleWidget() : nullptr;

    if (!Reticle)
    {
        return;
    }

    if (TargetActor && TargetActor->Implements<UEdrathInteractableInterface>())
    {
        const FText Prompt = IEdrathInteractableInterface::Execute_GetInteractionPrompt(TargetActor, OwnerPawn);
        const FText TargetName = IEdrathInteractableInterface::Execute_GetInteractionTargetName(TargetActor);
        Reticle->SetInspectionPrompt(Prompt, TargetName, true);
    }
    else
    {
        Reticle->ClearInspectionPrompt();
    }
}
