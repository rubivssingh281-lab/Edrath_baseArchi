#include "Character/SettlementVillagerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

ASettlementVillagerCharacter::ASettlementVillagerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    DialogueTriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DialogueTriggerSphere"));
    DialogueTriggerSphere->SetupAttachment(RootComponent);
    DialogueTriggerSphere->InitSphereRadius(200.0f);
    DialogueTriggerSphere->SetCollisionProfileName(TEXT("Trigger"));

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->InitCapsuleSize(40.0f, 92.0f);
        GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    }

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = 200.0f;
        GetCharacterMovement()->bOrientRotationToMovement = true;
    }

    ObservedLoopResets = 0;
    bHasTalkedThisCycle = false;
}

void ASettlementVillagerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UEdrathNarrativeSubsystem* NarrativeSys = GetNarrativeSubsystem())
    {
        NarrativeSys->OnSettlementLoopReset.AddDynamic(this, &ASettlementVillagerCharacter::HandleSettlementLoopReset);
    }
}

void ASettlementVillagerCharacter::HandleSettlementLoopReset(int32 TotalLoopResets)
{
    ObservedLoopResets = TotalLoopResets;
    bHasTalkedThisCycle = false;
}

FText ASettlementVillagerCharacter::InteractWithVillager(AActor* Interactor)
{
    bHasTalkedThisCycle = true;
    const FText SpokenLine = GetContextualDialogue();
    OnVillagerSpoken.Broadcast(this, Interactor, SpokenLine);
    return SpokenLine;
}

FText ASettlementVillagerCharacter::GetContextualDialogue() const
{
    if (UEdrathNarrativeSubsystem* NarrativeSys = GetNarrativeSubsystem())
    {
        if (NarrativeSys->IsSettlementLoopBroken())
        {
            return BrokenLoopDialogue;
        }

        const int32 CurrentDay = NarrativeSys->GetCurrentSettlementLoopDay();
        if (CurrentDay >= 7)
        {
            return LoopEndDaySevenDialogue;
        }
    }

    return NormalDayDialogue;
}

UEdrathNarrativeSubsystem* ASettlementVillagerCharacter::GetNarrativeSubsystem() const
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
