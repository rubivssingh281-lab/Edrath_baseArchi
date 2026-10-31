#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeLoopSettlementManager.generated.h"

class UEdrathNarrativeSubsystem;

UCLASS()
class EDRATH_API ATimeLoopSettlementManager : public AActor
{
    GENERATED_BODY()

public:
    ATimeLoopSettlementManager();

    UFUNCTION(BlueprintCallable, Category="Edrath|Act1")
    void AdvanceSettlementDay();

    UFUNCTION(BlueprintCallable, Category="Edrath|Act1")
    void BreakSettlementLoop();

    UFUNCTION(BlueprintPure, Category="Edrath|Act1")
    bool IsLoopBroken() const;

protected:
    virtual void BeginPlay() override;

private:
    TWeakObjectPtr<UEdrathNarrativeSubsystem> NarrativeSubsystem;
};
