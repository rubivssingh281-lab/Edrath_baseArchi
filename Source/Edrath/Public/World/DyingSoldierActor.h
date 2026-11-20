#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "DyingSoldierActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AAct1PreAlphaSliceDirector;
class UEdrathNarrativeSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDyingSoldierInteractedSignature, AActor*, Interactor, const FText&, SpokenLine);

/**
 * Dying Soldier Narrative Actor - Zone 1 Ancient Battlefield
 * The dying soldier lying beside thousands of corpses who calls Kael by an unfamiliar name
 * and speaks the fateful plea: "Do not let my daughter be born".
 */
UCLASS()
class EDRATH_API ADyingSoldierActor : public AActor
{
    GENERATED_BODY()

public:
    ADyingSoldierActor();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UStaticMeshComponent> SoldierMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Dialogue")
    FText SpokenDialogue = FText::FromString(TEXT("Do not let my daughter be born"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Dialogue")
    FName SoldierIdentifier = TEXT("DyingSoldier_Act1");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Dialogue")
    FGameplayTag EvidenceTag;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|State")
    bool bHasTriggeredDialogue = false;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Events")
    FOnDyingSoldierInteractedSignature OnDyingSoldierInteracted;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Dialogue")
    bool TriggerDialogue(AActor* Interactor);

    UFUNCTION(BlueprintPure, Category = "Edrath|Dialogue")
    bool HasTriggeredDialogue() const { return bHasTriggeredDialogue; }

    UFUNCTION(BlueprintCallable, Category = "Edrath|Dialogue")
    void BindToDirector(AAct1PreAlphaSliceDirector* InDirector);

protected:
    UFUNCTION()
    void HandleSphereOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

private:
    UPROPERTY()
    TWeakObjectPtr<AAct1PreAlphaSliceDirector> BoundDirector;

    UEdrathNarrativeSubsystem* GetNarrativeSubsystem() const;
};
