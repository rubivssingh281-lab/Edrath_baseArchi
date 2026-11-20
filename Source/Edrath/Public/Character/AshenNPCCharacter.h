#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AshenNPCCharacter.generated.h"

UCLASS()
class EDRATH_API AAshenNPCCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAshenNPCCharacter();

protected:
    virtual void BeginPlay() override;
};
