#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TombSarcophagusActor.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTombDisturbed, FGuid, LivingNpcId);

UCLASS()
class EDRATH_API ATombSarcophagusActor : public AActor
{
    GENERATED_BODY()

public:
    ATombSarcophagusActor();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|AshenExpanse")
    TObjectPtr<UStaticMeshComponent> SarcophagusMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|AshenExpanse")
    FGuid LivingNpcId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|AshenExpanse")
    FText DisturbedMemoryDescription = FText::FromString(TEXT("The living NPC remembers a death that never happened."));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|AshenExpanse")
    FText DisturbedDialogue = FText::FromString(TEXT("I remember the tomb beneath the ash."));

    UPROPERTY(BlueprintAssignable, Category="Edrath|AshenExpanse")
    FOnTombDisturbed OnTombDisturbed;

    UFUNCTION(BlueprintCallable, Category="Edrath|AshenExpanse")
    bool BindLivingNpc(AActor* LivingNpc, FGuid InLivingNpcId);

    UFUNCTION(BlueprintCallable, Category="Edrath|AshenExpanse")
    bool DisturbTomb(AActor* DisturbingActor);

    UFUNCTION(BlueprintPure, Category="Edrath|AshenExpanse")
    bool HasBeenDisturbed() const { return bHasBeenDisturbed; }

    UFUNCTION(BlueprintPure, Category="Edrath|AshenExpanse")
    FText GetCurrentLivingNpcDialogue() const { return CurrentLivingNpcDialogue; }

private:
    UPROPERTY()
    TWeakObjectPtr<AActor> LivingNpc;

    FText CurrentLivingNpcDialogue;
    bool bHasBeenDisturbed = false;
};
