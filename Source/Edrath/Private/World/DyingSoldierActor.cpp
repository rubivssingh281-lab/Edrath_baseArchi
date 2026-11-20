#include "World/DyingSoldierActor.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Core/EdrathGameplayTags.h"

ADyingSoldierActor::ADyingSoldierActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    SoldierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoldierMesh"));
    SoldierMesh->SetupAttachment(RootComponent);
    SoldierMesh->SetCollisionProfileName(TEXT("BlockAll"));

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->InitSphereRadius(250.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));

    EvidenceTag = EdrathGameplayTags::NarrativeDyingSoldier;
    bHasTriggeredDialogue = false;
}

void ADyingSoldierActor::BeginPlay()
{
    Super::BeginPlay();

    if (InteractionSphere)
    {
        InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADyingSoldierActor::HandleSphereOverlap);
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

void ADyingSoldierActor::HandleSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && !bHasTriggeredDialogue)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (OtherActor == PlayerPawn)
        {
            TriggerDialogue(OtherActor);
        }
    }
}

bool ADyingSoldierActor::TriggerDialogue(AActor* Interactor)
{
    if (bHasTriggeredDialogue)
    {
        return false;
    }

    bHasTriggeredDialogue = true;
    OnDyingSoldierInteracted.Broadcast(Interactor, SpokenDialogue);

    if (BoundDirector.IsValid())
    {
        BoundDirector->RegisterDyingSoldierDialogueTriggered();
    }

    if (UEdrathNarrativeSubsystem* NarrativeSys = GetNarrativeSubsystem())
    {
        const FText EvidenceDesc = FText::FromString(
            TEXT("A dying soldier on the ancient battlefield called Kael by an unfamiliar name and pleaded: 'Do not let my daughter be born.'"));
        NarrativeSys->AddOrMutateEvidence(EvidenceTag, EvidenceDesc, true);
    }

    return true;
}

void ADyingSoldierActor::BindToDirector(AAct1PreAlphaSliceDirector* InDirector)
{
    BoundDirector = InDirector;
}

UEdrathNarrativeSubsystem* ADyingSoldierActor::GetNarrativeSubsystem() const
{
    if (const UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            return GI->GetSubsystem<UEdrathNarrativeSubsystem>();
        }
    }
    return nullptr;
}
