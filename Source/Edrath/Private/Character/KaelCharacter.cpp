#include "Character/KaelCharacter.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/EchoCombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

AKaelCharacter::AKaelCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    SternumBlackClockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SternumBlackClockMesh"));
    SternumBlackClockMesh->SetupAttachment(GetMesh(), TEXT("bone_chest_clock"));
    DualCameraComponent = CreateDefaultSubobject<UEdrathDualCameraComponent>(TEXT("DualCameraComponent"));
    BlackClockComponent = CreateDefaultSubobject<UBlackClockComponent>(TEXT("BlackClockComponent"));
    EchoCombatComponent = CreateDefaultSubobject<UEdrathEchoCombatComponent>(TEXT("EchoCombatComponent"));
}

void AKaelCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (DualCameraComponent)
    {
        DualCameraComponent->InitializeCameraRig(CameraBoom, FollowCamera, this);
    }
}

void AKaelCharacter::PerformPrimaryAttack()
{
    if (BlackClockComponent)
    {
        BlackClockComponent->RecordCombatFrame(TEXT("Slash_Combo_01"), 35.0f);
    }
}

bool AKaelCharacter::ActivateBlackClockShift()
{
    return BlackClockComponent && BlackClockComponent->TogglePossibilityShift(CurrentRegion);
}
