#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EdrathChronalCombatInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UEdrathChronalCombatInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * IEdrathChronalCombatInterface
 * Unified combat damage & poise resolution contract implemented by enemies and bosses
 * (Wraiths, Maevra, The First Archivist) so sword slashes, parries, and Echo Strike sweeps apply uniformly.
 */
class EDRATH_API IEdrathChronalCombatInterface
{
    GENERATED_BODY()

public:
    /** Receives a slash, parry counter, or Echo Strike impact and returns actual health damage applied. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Edrath|Combat")
    float ReceiveChronalHit(float DamageAmount, float PoiseDamage, bool bIsEchoStrike, AActor* InstigatorActor, const FVector& HitLocation);
};
