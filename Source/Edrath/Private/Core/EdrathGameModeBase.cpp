#include "Core/EdrathGameModeBase.h"
#include "Character/KaelCharacter.h"
#include "Core/EdrathPlayerController.h"
#include "UI/EdrathHUD.h"

AEdrathGameModeBase::AEdrathGameModeBase()
{
    DefaultPawnClass = AKaelCharacter::StaticClass();
    PlayerControllerClass = AEdrathPlayerController::StaticClass();
    HUDClass = AEdrathHUD::StaticClass();
}
