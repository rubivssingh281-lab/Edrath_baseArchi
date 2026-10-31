#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "EchoCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualGhostStrikeRequested, const FEchoActionFrame&, EchoFrame);

UCLASS(ClassGroup=(EdrathBlackClock), meta=(BlueprintSpawnableComponent))
class EDRATH_API UEdrathEchoCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEdrathEchoCombatComponent();

    UFUNCTION(BlueprintCallable, Category="Edrath|BlackClock|EchoCombat")
    void SpawnVisualGhostStrike(const FEchoActionFrame& EchoFrame);

    UFUNCTION(BlueprintPure, Category="Edrath|BlackClock|EchoCombat")
    const FEchoActionFrame& GetLastGhostStrike() const { return LastGhostStrike; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|BlackClock|EchoCombat")
    FOnVisualGhostStrikeRequested OnVisualGhostStrikeRequested;

private:
    FEchoActionFrame LastGhostStrike;
};
