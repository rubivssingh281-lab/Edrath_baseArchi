#include "Camera/EdrathInteriorTransitionVolume.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

AEdrathInteriorTransitionVolume::AEdrathInteriorTransitionVolume()
{
    PrimaryActorTick.bCanEverTick = false;
    InteriorBoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBoundsBox"));
    RootComponent = InteriorBoundsBox;
    InteriorBoundsBox->SetBoxExtent(FVector(600.0f, 600.0f, 350.0f));
    InteriorBoundsBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AEdrathInteriorTransitionVolume::BeginPlay()
{
    Super::BeginPlay();
    InteriorBoundsBox->OnComponentBeginOverlap.AddDynamic(this, &AEdrathInteriorTransitionVolume::OnInteriorOverlapBegin);
    InteriorBoundsBox->OnComponentEndOverlap.AddDynamic(this, &AEdrathInteriorTransitionVolume::OnInteriorOverlapEnd);
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
        DualCamera->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson, this);
        InteriorEntryTimestampSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
        for (AActor* RoofActor : RoofActorsToToggle)
        {
            if (RoofActor)
            {
                RoofActor->SetActorHiddenInGame(true);
            }
        }
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
        DualCamera->SetCameraPerspective(EEdrathCameraMode::ExteriorBirdsEye, this);
        for (AActor* RoofActor : RoofActorsToToggle)
        {
            if (RoofActor)
            {
                RoofActor->SetActorHiddenInGame(false);
            }
        }
        if (bIsCrownOfWinterTimeDilatedInterior && GetWorld() && GetWorld()->GetGameInstance())
        {
            const double SecondsSpentInside = FMath::Max(0.0, GetWorld()->GetTimeSeconds() - InteriorEntryTimestampSeconds);
            const int32 ExteriorDays = FMath::RoundToInt(SecondsSpentInside * SimulatedDaysPerInteriorSecond);
            if (UEdrathNarrativeSubsystem* Narrative = GetWorld()->GetGameInstance()->GetSubsystem<UEdrathNarrativeSubsystem>())
            {
                Narrative->AdvanceExteriorTimeDays(ExteriorDays);
            }
        }
    }
}
