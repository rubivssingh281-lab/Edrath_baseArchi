#include "Character/AshenNPCCharacter.h"

AAshenNPCCharacter::AAshenNPCCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAshenNPCCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (USkeletalMeshComponent* CharacterMesh = GetMesh())
    {
        CharacterMesh->SetCastShadow(false);
        CharacterMesh->bCastDynamicShadow = false;
    }
}
