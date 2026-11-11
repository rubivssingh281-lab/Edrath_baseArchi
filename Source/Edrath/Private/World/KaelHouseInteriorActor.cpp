#include "World/KaelHouseInteriorActor.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Character/KaelCharacter.h"
#include "Narrative/ForensicClueActor.h"
#include "Core/EdrathGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AKaelHouseInteriorActor::AKaelHouseInteriorActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    HouseStructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HouseStructureMesh"));
    HouseStructureMesh->SetupAttachment(RootComponent);
    HouseStructureMesh->SetCollisionProfileName(TEXT("BlockAll"));

    ExteriorRoofMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExteriorRoofMesh"));
    ExteriorRoofMesh->SetupAttachment(RootComponent);
    ExteriorRoofMesh->SetCollisionProfileName(TEXT("BlockAll"));

    InteriorBoundsTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBoundsTrigger"));
    InteriorBoundsTrigger->SetupAttachment(RootComponent);
    InteriorBoundsTrigger->InitBoxExtent(FVector(600.0f, 600.0f, 300.0f));
    InteriorBoundsTrigger->SetCollisionProfileName(TEXT("Trigger"));

    bIsPlayerInside = false;
    bHandwritingInspected = false;
    bPortraitInspected = false;
}

void AKaelHouseInteriorActor::BeginPlay()
{
    Super::BeginPlay();

    if (InteriorBoundsTrigger)
    {
        InteriorBoundsTrigger->OnComponentBeginOverlap.AddDynamic(this, &AKaelHouseInteriorActor::HandleInteriorBeginOverlap);
        InteriorBoundsTrigger->OnComponentEndOverlap.AddDynamic(this, &AKaelHouseInteriorActor::HandleInteriorEndOverlap);
    }

    if (!BoundDirector.IsValid() && GetWorld())
    {
        AActor* FoundDirector = UGameplayStatics::GetActorOfClass(GetWorld(), AAct1PreAlphaSliceDirector::StaticClass());
        if (FoundDirector)
        {
            BindToDirector(Cast<AAct1PreAlphaSliceDirector>(FoundDirector));
        }
    }

    if (HandwritingClue)
    {
        HandwritingClue->OnForensicClueInspected.AddDynamic(this, &AKaelHouseInteriorActor::HandleClueInspected);
        if (BoundDirector.IsValid())
        {
            BoundDirector->RegisterHouseForensicClue(HandwritingClue);
        }
    }

    if (ScratchedPortraitClue)
    {
        ScratchedPortraitClue->OnForensicClueInspected.AddDynamic(this, &AKaelHouseInteriorActor::HandleClueInspected);
        if (BoundDirector.IsValid())
        {
            BoundDirector->RegisterHouseForensicClue(ScratchedPortraitClue);
        }
    }
}

void AKaelHouseInteriorActor::SetInteriorMode(bool bInside)
{
    bIsPlayerInside = bInside;

    if (ExteriorRoofMesh)
    {
        ExteriorRoofMesh->SetHiddenInGame(bInside);
        ExteriorRoofMesh->SetCollisionEnabled(bInside ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
    }

    OnHouseInteriorStateChanged.Broadcast(bInside);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (AKaelCharacter* Kael = Cast<AKaelCharacter>(PlayerPawn))
    {
        if (Kael->DualCameraComponent)
        {
            const EEdrathCameraMode DesiredMode = bInside ? EEdrathCameraMode::InteriorThirdPerson : EEdrathCameraMode::ExteriorBirdsEye;
            Kael->DualCameraComponent->SetCameraPerspective(DesiredMode, this);
        }
    }

    if (BoundDirector.IsValid())
    {
        BoundDirector->RefreshZoneState();
    }
}

void AKaelHouseInteriorActor::HandleInteriorBeginOverlap(
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
        SetInteriorMode(true);
    }
}

void AKaelHouseInteriorActor::HandleInteriorEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor && OtherActor == PlayerPawn)
    {
        SetInteriorMode(false);
    }
}

void AKaelHouseInteriorActor::HandleClueInspected(FGameplayTag EvidenceTag, AActor* Inspector)
{
    if (EvidenceTag == EdrathGameplayTags::EvidenceHandwriting || EvidenceTag == EdrathGameplayTags::Twist1)
    {
        bHandwritingInspected = true;
    }
    else if (EvidenceTag == EdrathGameplayTags::EvidenceScratchedPortrait || EvidenceTag == EdrathGameplayTags::Twist2)
    {
        bPortraitInspected = true;
    }

    if (BoundDirector.IsValid())
    {
        BoundDirector->RefreshZoneState();
    }
}

void AKaelHouseInteriorActor::BindToDirector(AAct1PreAlphaSliceDirector* InDirector)
{
    BoundDirector = InDirector;
    if (BoundDirector.IsValid())
    {
        if (HandwritingClue)
        {
            BoundDirector->RegisterHouseForensicClue(HandwritingClue);
        }
        if (ScratchedPortraitClue)
        {
            BoundDirector->RegisterHouseForensicClue(ScratchedPortraitClue);
        }
    }
}
