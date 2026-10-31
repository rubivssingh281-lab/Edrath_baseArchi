#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "NhalFactionManager.generated.h"

class AActor;
class AKaelCharacter;

UCLASS()
class EDRATH_API ANhalFactionManager : public AActor
{
    GENERATED_BODY()

public:
    ANhalFactionManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Nhal")
    FGameplayTag RestorationistsFactionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Nhal")
    FGameplayTag RevanchistsFactionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Nhal")
    TSubclassOf<AActor> FutureDeadCompanionEchoClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Nhal")
    TArray<TObjectPtr<AActor>> LivingVeylFamilyActors;

    UFUNCTION(BlueprintCallable, Category="Edrath|Nhal")
    void SetFactionReputation(FGameplayTag FactionTag, int32 NewReputation);

    UFUNCTION(BlueprintCallable, Category="Edrath|Nhal")
    void AdjustFactionReputation(FGameplayTag FactionTag, int32 Delta);

    UFUNCTION(BlueprintPure, Category="Edrath|Nhal")
    int32 GetFactionReputation(FGameplayTag FactionTag) const;

    UFUNCTION(BlueprintCallable, Category="Edrath|Nhal")
    AActor* SpawnFutureDeadCompanionEcho(FName CompanionId, FVector SpawnLocation);

    UFUNCTION(BlueprintCallable, Category="Edrath|Nhal")
    int32 SpawnActiveCompanionEchoes(const TArray<FName>& CompanionIds, FVector Origin, float Spacing = 180.0f);

    UFUNCTION(BlueprintCallable, Category="Edrath|Nhal")
    bool RestoreNhalCitizen(FName CitizenId);

    UFUNCTION(BlueprintCallable, Category="Edrath|Nhal")
    bool RestoreNhalCitizenViaBlackClock(FName CitizenId, AActor* Restorer);

    UFUNCTION(BlueprintPure, Category="Edrath|Nhal")
    int32 GetRestoredCitizenCount() const { return RestoredCitizenCount; }

    UFUNCTION(BlueprintPure, Category="Edrath|Nhal")
    bool HasTriggeredTwistThree() const { return bTwistThreeTriggered; }

private:
    TMap<FGameplayTag, int32> FactionReputation;
    TSet<FName> RestoredCitizens;
    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedCompanionEchoes;
    int32 RestoredCitizenCount = 0;
    bool bTwistThreeTriggered = false;
};
