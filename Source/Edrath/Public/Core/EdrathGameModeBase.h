#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EdrathGameModeBase.generated.h"

/**
 * AEdrathGameModeBase establishes default game classes:
 * - DefaultPawn: AKaelCharacter
 * - PlayerController: AEdrathPlayerController
 * - HUD: AEdrathHUD
 */
UCLASS()
class EDRATH_API AEdrathGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AEdrathGameModeBase();
};
