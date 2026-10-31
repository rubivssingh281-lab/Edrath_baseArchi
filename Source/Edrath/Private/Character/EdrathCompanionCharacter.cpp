#include "Character/EdrathCompanionCharacter.h"

AEdrathCompanionCharacter::AEdrathCompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEdrathCompanionCharacter::BeginPlay()
{
    Super::BeginPlay();
    RelationshipState.CompanionID = CompanionID;
}

void AEdrathCompanionCharacter::SetAutonomousState(float NewLoyalty, float NewSuspicion, bool bNewWithholdingTruth)
{
    RelationshipState.Loyalty = FMath::Clamp(NewLoyalty, 0.0f, 100.0f);
    RelationshipState.SuspicionOfKael = FMath::Clamp(NewSuspicion, 0.0f, 100.0f);
    RelationshipState.bWithholdingCriticalTruth = bNewWithholdingTruth;
    RelationshipState.bHasTurnedHostileInNhal = RelationshipState.Loyalty <= 0.0f;
}
