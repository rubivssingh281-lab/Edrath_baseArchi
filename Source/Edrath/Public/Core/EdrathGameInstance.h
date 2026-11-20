#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EdrathGameInstance.generated.h"

UCLASS()
class EDRATH_API UEdrathGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    UFUNCTION(BlueprintPure, Category="Edrath|Core")
    bool IsMobileBuild() const;
};
