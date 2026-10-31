#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "EdrathCompanionCharacter.generated.h"

UCLASS()
class EDRATH_API AEdrathCompanionCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEdrathCompanionCharacter();

    UFUNCTION(BlueprintCallable, Category="Edrath|Companion")
    void SetAutonomousState(float NewLoyalty, float NewSuspicion, bool bNewWithholdingTruth);

    UFUNCTION(BlueprintPure, Category="Edrath|Companion")
    FCompanionRelationshipState GetRelationshipState() const { return RelationshipState; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companion")
    FName CompanionID = NAME_None;

protected:
    virtual void BeginPlay() override;

private:
    FCompanionRelationshipState RelationshipState;
};
