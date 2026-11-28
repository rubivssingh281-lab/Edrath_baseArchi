#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KaelInteractionScannerComponent.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedInteractableChanged, AActor*, NewInteractable);

/**
 * UKaelInteractionScannerComponent
 * Continuously locates and scores nearby actors implementing IEdrathInteractableInterface,
 * synchronizes HUD reticle contextual inspection prompts, and dispatches [E] interactions.
 */
UCLASS(ClassGroup=(EdrathCharacter), meta=(BlueprintSpawnableComponent))
class EDRATH_API UKaelInteractionScannerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKaelInteractionScannerComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Scans the surrounding radius for valid interactables and updates HUD focus. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Interaction")
    AActor* ScanForInteractables();

    /** Explicitly registers/evaluates candidate actors (useful in automated tests or triggers). */
    UFUNCTION(BlueprintCallable, Category="Edrath|Interaction")
    AActor* SelectBestCandidate(const TArray<AActor*>& CandidateActors);

    /** Executes interaction against the currently focused actor if valid. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Interaction")
    bool TryInteractWithFocusedTarget();

    /** Returns the currently highlighted interactable actor, if any. */
    UFUNCTION(BlueprintPure, Category="Edrath|Interaction")
    AActor* GetFocusedInteractable() const { return FocusedInteractable.Get(); }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interaction")
    float InteractionScanRadius = 320.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Interaction")
    float ForwardAlignmentWeight = 0.35f;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Interaction")
    FOnFocusedInteractableChanged OnFocusedInteractableChanged;

private:
    void PushFocusToHUD(AActor* TargetActor);

    TWeakObjectPtr<AActor> FocusedInteractable;
};
