#include "World/PriestCorpseActor.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BlackClock/BlackClockComponent.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Core/EdrathGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

APriestCorpseActor::APriestCorpseActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    SarcophagusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SarcophagusMesh"));
    SarcophagusMesh->SetupAttachment(RootComponent);
    SarcophagusMesh->SetCollisionProfileName(TEXT("BlockAll"));

    SkeletalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetCollisionProfileName(TEXT("BlockAll"));

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->InitSphereRadius(250.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));

    CorpseEvidenceTag = EdrathGameplayTags::EvidencePriestCorpse114Years;
    MemoryAnchorTag = EdrathGameplayTags::QuestAct1Crypt;
    MemoryOwnerName = TEXT("SettlementPriest");
    YearsDeceased = 114;
    bCorpseInspected = false;
    bMemoryReconstructed = false;
}

void APriestCorpseActor::BeginPlay()
{
    Super::BeginPlay();

    if (!BoundDirector.IsValid() && GetWorld())
    {
        AActor* FoundDirector = UGameplayStatics::GetActorOfClass(GetWorld(), AAct1PreAlphaSliceDirector::StaticClass());
        if (FoundDirector)
        {
            BindToDirector(Cast<AAct1PreAlphaSliceDirector>(FoundDirector));
        }
    }
}

bool APriestCorpseActor::InspectCorpse(AActor* Inspector)
{
    if (bCorpseInspected)
    {
        return true;
    }

    bCorpseInspected = true;

    if (UEdrathNarrativeSubsystem* NarrativeSys = GetNarrativeSubsystem())
    {
        const FText EvidenceDesc = FText::FromString(
            TEXT("The skeletal remains inside the sealed sarcophagus belong to the settlement priest. Forensic dating confirms he died 114 years ago, despite speaking with Kael moments earlier."));
        NarrativeSys->AddOrMutateEvidence(CorpseEvidenceTag, EvidenceDesc, true);
    }

    if (BoundDirector.IsValid())
    {
        BoundDirector->RegisterPriestCorpseInspected();
    }

    return true;
}

bool APriestCorpseActor::ReconstructMemory(AActor* Reconstructor)
{
    if (bMemoryReconstructed)
    {
        return true;
    }

    if (!bCorpseInspected)
    {
        InspectCorpse(Reconstructor);
    }

    if (!Reconstructor)
    {
        return false;
    }

    UBlackClockComponent* BlackClock = Reconstructor->FindComponentByClass<UBlackClockComponent>();
    if (!BlackClock)
    {
        return false;
    }

    if (!BlackClock->EnterMemoryReconstruction(MemoryAnchorTag, MemoryOwnerName))
    {
        return false;
    }

    if (UEdrathNarrativeSubsystem* NarrativeSys = GetNarrativeSubsystem())
    {
        NarrativeSys->BreakAct1SettlementLoop();
        NarrativeSys->RecordRecoveredMemory(MemoryAnchorTag, MemoryOwnerName);
    }

    bMemoryReconstructed = true;
    OnPriestTwistRevealed.Broadcast(this);

    if (BoundDirector.IsValid())
    {
        BoundDirector->CompletePriestMemoryReconstruction();
    }

    return true;
}

void APriestCorpseActor::BindToDirector(AAct1PreAlphaSliceDirector* InDirector)
{
    BoundDirector = InDirector;
}

UEdrathNarrativeSubsystem* APriestCorpseActor::GetNarrativeSubsystem() const
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
