#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "KaelModularRigComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKaelOutfitChanged);

UCLASS(ClassGroup=(EdrathCharacter), meta=(BlueprintSpawnableComponent))
class EDRATH_API UKaelModularRigComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKaelModularRigComponent();

    UFUNCTION(BlueprintCallable, Category="Edrath|Character")
    void SetLifeEchoArchetype(EKaelLifeEchoArchetype NewArchetype);

    UFUNCTION(BlueprintPure, Category="Edrath|Character")
    EKaelLifeEchoArchetype GetLifeEchoArchetype() const { return CurrentArchetype; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Character")
    FOnKaelOutfitChanged OnOutfitChanged;

private:
    EKaelLifeEchoArchetype CurrentArchetype = EKaelLifeEchoArchetype::ForgottenSoldier_Act1;
};
