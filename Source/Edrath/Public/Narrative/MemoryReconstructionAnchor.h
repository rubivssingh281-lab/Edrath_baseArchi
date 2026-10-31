#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "MemoryReconstructionAnchor.generated.h"

class UBlackClockComponent;

UCLASS()
class EDRATH_API AMemoryReconstructionAnchor : public AActor
{
    GENERATED_BODY()

public:
    AMemoryReconstructionAnchor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Memory")
    FGameplayTag MemoryAnchorTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Memory")
    FName OriginalMemoryOwnerNPC = NAME_None;

    UFUNCTION(BlueprintCallable, Category="Edrath|Memory")
    bool ActivateMemoryReconstruction(AActor* ActivatingActor);
};
