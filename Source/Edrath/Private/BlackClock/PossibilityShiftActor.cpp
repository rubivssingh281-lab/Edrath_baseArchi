#include "BlackClock/PossibilityShiftActor.h"
#include "Components/StaticMeshComponent.h"

APossibilityShiftActor::APossibilityShiftActor()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;
    PresentTimelineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PresentTimelineMesh"));
    PresentTimelineMesh->SetupAttachment(SceneRoot);
    RejectedTimelineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RejectedTimelineMesh"));
    RejectedTimelineMesh->SetupAttachment(SceneRoot);
}

void APossibilityShiftActor::BeginPlay()
{
    Super::BeginPlay();
    ApplyTimelineState(ETimelineState::PresentTimeline);
}

void APossibilityShiftActor::ApplyTimelineState(ETimelineState NewState)
{
    CurrentState = NewState;
    const bool bRejected = NewState == ETimelineState::RejectedTimeline;
    PresentTimelineMesh->SetVisibility(!bRejected, true);
    PresentTimelineMesh->SetCollisionEnabled(bRejected ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
    RejectedTimelineMesh->SetVisibility(bRejected, true);
    RejectedTimelineMesh->SetCollisionEnabled(bRejected ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}
