#include "Camera/EdrathInteriorTransitionVolume.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AEdrathInteriorTransitionVolume::AEdrathInteriorTransitionVolume()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    InteriorBoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBoundsBox"));
    RootComponent = InteriorBoundsBox;
    InteriorBoundsBox->SetBoxExtent(FVector(600.0f, 600.0f, 350.0f));
    InteriorBoundsBox->SetCollisionProfileName(TEXT("Trigger"));

    DoorwayEntryArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DoorwayEntryArrow"));
    DoorwayEntryArrow->SetupAttachment(RootComponent);
    DoorwayEntryArrow->ArrowColor = FColor::Cyan;
    DoorwayEntryArrow->ArrowSize = 1.5f;
    DoorwayEntryArrow->bTreatAsASprite = true;
}

void AEdrathInteriorTransitionVolume::BeginPlay()
{
    Super::BeginPlay();
    InteriorBoundsBox->OnComponentBeginOverlap.AddDynamic(this, &AEdrathInteriorTransitionVolume::OnInteriorOverlapBegin);
    InteriorBoundsBox->OnComponentEndOverlap.AddDynamic(this, &AEdrathInteriorTransitionVolume::OnInteriorOverlapEnd);

    CurrentRoofOpacity = 1.0f;
    TargetRoofOpacity = 1.0f;
    bIsFadingRoof = false;
    ApplyRoofMaterialOpacity(1.0f);
}

void AEdrathInteriorTransitionVolume::OnInteriorOverlapBegin(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*,
    int32, bool, const FHitResult&)
{
    if (!OtherActor)
    {
        return;
    }

    if (UEdrathDualCameraComponent* DualCamera = OtherActor->FindComponentByClass<UEdrathDualCameraComponent>())
    {
        bPlayerInside = true;
        DualCamera->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson, this);
        InteriorEntryTimestampSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

        // Initiate smooth dithered roof fade-out
        TargetRoofOpacity = 0.0f;
        bIsFadingRoof = true;

        OnInteriorTransitionStateChanged.Broadcast(true, this);
    }
}

void AEdrathInteriorTransitionVolume::OnInteriorOverlapEnd(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
    if (!OtherActor)
    {
        return;
    }

    if (UEdrathDualCameraComponent* DualCamera = OtherActor->FindComponentByClass<UEdrathDualCameraComponent>())
    {
        bPlayerInside = false;
        DualCamera->SetCameraPerspective(EEdrathCameraMode::ExteriorBirdsEye, this);

        // Initiate smooth dithered roof fade-in: immediately unhide actors so fade is visible
        for (AActor* RoofActor : RoofActorsToToggle)
        {
            if (RoofActor)
            {
                RoofActor->SetActorHiddenInGame(false);
            }
        }

        TargetRoofOpacity = 1.0f;
        bIsFadingRoof = true;

        // Crown of Winter Simulated Exterior Time Advancement
        if (bIsCrownOfWinterTimeDilatedInterior && GetWorld() && GetWorld()->GetGameInstance())
        {
            const double SecondsSpentInside = FMath::Max(0.0, GetWorld()->GetTimeSeconds() - InteriorEntryTimestampSeconds);
            const int32 ExteriorDays = FMath::RoundToInt(SecondsSpentInside * SimulatedDaysPerInteriorSecond);
            if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                Narrative->AdvanceExteriorTimeDays(ExteriorDays);
            }
        }

        OnInteriorTransitionStateChanged.Broadcast(false, this);
    }
}

void AEdrathInteriorTransitionVolume::ApplyRoofMaterialOpacity(float Opacity)
{
    for (AActor* RoofActor : RoofActorsToToggle)
    {
        if (!RoofActor)
        {
            continue;
        }

        TArray<UMeshComponent*> MeshComponents;
        RoofActor->GetComponents<UMeshComponent>(MeshComponents);

        for (UMeshComponent* Mesh : MeshComponents)
        {
            if (Mesh)
            {
                Mesh->SetScalarParameterValueOnMaterials(RoofOpacityMaterialParamName, Opacity);
            }
        }
    }
}

void AEdrathInteriorTransitionVolume::UpdateRoofFade(float DeltaTime)
{
    if (!bIsFadingRoof)
    {
        return;
    }

    const float Step = DeltaTime / FMath::Max(0.01f, RoofFadeDuration);
    if (CurrentRoofOpacity < TargetRoofOpacity)
    {
        CurrentRoofOpacity = FMath::Min(CurrentRoofOpacity + Step, TargetRoofOpacity);
    }
    else
    {
        CurrentRoofOpacity = FMath::Max(CurrentRoofOpacity - Step, TargetRoofOpacity);
    }

    ApplyRoofMaterialOpacity(CurrentRoofOpacity);

    // When fully faded out, hide actors to save render passes
    if (FMath::IsNearlyEqual(CurrentRoofOpacity, 0.0f, 0.01f))
    {
        for (AActor* RoofActor : RoofActorsToToggle)
        {
            if (RoofActor)
            {
                RoofActor->SetActorHiddenInGame(true);
            }
        }
        bIsFadingRoof = false;
    }
    else if (FMath::IsNearlyEqual(CurrentRoofOpacity, 1.0f, 0.01f))
    {
        bIsFadingRoof = false;
    }
}

void AEdrathInteriorTransitionVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateRoofFade(DeltaTime);
}
