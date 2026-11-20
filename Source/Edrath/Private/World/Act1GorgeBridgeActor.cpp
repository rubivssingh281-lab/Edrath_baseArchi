#include "World/Act1GorgeBridgeActor.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AAct1GorgeBridgeActor::AAct1GorgeBridgeActor()
{
    ProximityTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ProximityTrigger"));
    ProximityTrigger->SetupAttachment(RootComponent);
    ProximityTrigger->InitBoxExtent(FVector(300.0f, 400.0f, 200.0f));
    ProximityTrigger->SetCollisionProfileName(TEXT("Trigger"));

    bPlayerNearChasm = false;
}

void AAct1GorgeBridgeActor::BeginPlay()
{
    Super::BeginPlay();

    if (ProximityTrigger)
    {
        ProximityTrigger->OnComponentBeginOverlap.AddDynamic(this, &AAct1GorgeBridgeActor::HandleProximityBeginOverlap);
        ProximityTrigger->OnComponentEndOverlap.AddDynamic(this, &AAct1GorgeBridgeActor::HandleProximityEndOverlap);
    }

    if (!BoundDirector.IsValid() && GetWorld())
    {
        AActor* FoundDirector = UGameplayStatics::GetActorOfClass(GetWorld(), AAct1PreAlphaSliceDirector::StaticClass());
        if (FoundDirector)
        {
            BindToDirector(Cast<AAct1PreAlphaSliceDirector>(FoundDirector));
        }
    }
}

void AAct1GorgeBridgeActor::ApplyTimelineState(ETimelineState NewState)
{
    Super::ApplyTimelineState(NewState);

    OnBridgeTimelineShifted.Broadcast(this, NewState);

    if (BoundDirector.IsValid())
    {
        BoundDirector->RefreshZoneState();
    }
}

void AAct1GorgeBridgeActor::HandleProximityBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor && OtherActor == PlayerPawn)
    {
        bPlayerNearChasm = true;
    }
}

void AAct1GorgeBridgeActor::HandleProximityEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor && OtherActor == PlayerPawn)
    {
        bPlayerNearChasm = false;
    }
}

void AAct1GorgeBridgeActor::BindToDirector(AAct1PreAlphaSliceDirector* InDirector)
{
    BoundDirector = InDirector;
}
