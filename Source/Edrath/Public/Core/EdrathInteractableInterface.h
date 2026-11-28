#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EdrathInteractableInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UEdrathInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * IEdrathInteractableInterface
 * Unified contract implemented by forensic clues, NPCs, graves, shrines, and quest anchors
 * so Kael's interaction scanner and HUD reticle prompt interact uniformly across both camera modes.
 */
class EDRATH_API IEdrathInteractableInterface
{
    GENERATED_BODY()

public:
    /** Returns true if the interactor currently meets range, camera, and quest state preconditions. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Edrath|Interaction")
    bool CanInteract(AActor* Interactor) const;

    /** Returns the contextual action prompt text displayed on HUD (e.g., "[E] Inspect Handwriting"). */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Edrath|Interaction")
    FText GetInteractionPrompt(AActor* Interactor) const;

    /** Returns the localized target title displayed alongside the prompt. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Edrath|Interaction")
    FText GetInteractionTargetName() const;

    /** Executes the primary world interaction and returns true if state mutated successfully. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Edrath|Interaction")
    bool ExecuteInteraction(AActor* Interactor);
};
