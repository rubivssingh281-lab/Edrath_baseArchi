#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/EdrathTypes.h"
#include "ForensicClueActor.generated.h"

class AActor;
class UBlackClockComponent;
class UEdrathDualCameraComponent;
class UEdrathNarrativeSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnForensicClueInspected, FGameplayTag, EvidenceTag, AActor*, Inspector);

UCLASS()
class EDRATH_API AForensicClueActor : public AActor
{
    GENERATED_BODY()

public:
    AForensicClueActor();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    FGameplayTag EvidenceTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    FText InitialDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    FText RejectedTimelineDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    FText MemoryReconstructionDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    bool bIsTwistClue = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    bool bRequiresInteriorCamera = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Forensics")
    float InspectionRadius = 250.0f;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Forensics")
    FOnForensicClueInspected OnForensicClueInspected;

    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    bool Inspect(AActor* Inspector);

    UFUNCTION(BlueprintCallable, Category="Edrath|Forensics")
    void BindToBlackClock(UBlackClockComponent* BlackClock);

    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    FText GetCurrentDescription() const { return CurrentDescription; }

    UFUNCTION(BlueprintPure, Category="Edrath|Forensics")
    bool HasBeenInspected() const { return bHasBeenInspected; }

protected:
    UFUNCTION()
    void HandleTimelineStateShifted(ETimelineState NewState, FVector Epicenter, float WaveRadius);

private:
    bool IsInspectionAllowed(AActor* Inspector) const;
    void ApplyDescriptionForTimeline(ETimelineState NewState);
    UEdrathNarrativeSubsystem* GetNarrativeSubsystem() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Forensics", meta=(AllowPrivateAccess="true"))
    FText CurrentDescription;

    bool bHasBeenInspected = false;
    TWeakObjectPtr<UEdrathDualCameraComponent> InspectorCamera;
};
