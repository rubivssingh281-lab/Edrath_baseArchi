#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PriestCorpseActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AMemoryReconstructionAnchor;
class AAct1PreAlphaSliceDirector;
class UEdrathNarrativeSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPriestTwistRevealedSignature, class APriestCorpseActor*, PriestCorpse);

/**
 * Priest Corpse Forensic Actor - Zone 5 Local Crypt
 * Represents the skeletal remains of the settlement priest, dead for 114 years.
 * Inspecting the corpse and activating Memory Reconstruction shatters the settlement's
 * 7-day loop by exposing that the priest's living voice was a preserved memory echo.
 */
UCLASS()
class EDRATH_API APriestCorpseActor : public AActor
{
    GENERATED_BODY()

public:
    APriestCorpseActor();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UStaticMeshComponent> SarcophagusMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<UStaticMeshComponent> SkeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Crypt|Forensics")
    FGameplayTag CorpseEvidenceTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Crypt|Forensics")
    FGameplayTag MemoryAnchorTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Crypt|Forensics")
    FName MemoryOwnerName = TEXT("SettlementPriest");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Crypt|Forensics")
    int32 YearsDeceased = 114;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Crypt|State")
    bool bCorpseInspected = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Crypt|State")
    bool bMemoryReconstructed = false;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Crypt|Events")
    FOnPriestTwistRevealedSignature OnPriestTwistRevealed;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Crypt")
    bool InspectCorpse(AActor* Inspector);

    UFUNCTION(BlueprintCallable, Category = "Edrath|Crypt")
    bool ReconstructMemory(AActor* Reconstructor);

    UFUNCTION(BlueprintPure, Category = "Edrath|Crypt")
    bool HasBrokenSettlementLoop() const { return bMemoryReconstructed; }

    UFUNCTION(BlueprintCallable, Category = "Edrath|Crypt")
    void BindToDirector(AAct1PreAlphaSliceDirector* InDirector);

private:
    UPROPERTY()
    TWeakObjectPtr<AAct1PreAlphaSliceDirector> BoundDirector;

    UEdrathNarrativeSubsystem* GetNarrativeSubsystem() const;
};
