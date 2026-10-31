#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/EdrathTypes.h"
#include "PossibilityShiftActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class EDRATH_API APossibilityShiftActor : public AActor
{
    GENERATED_BODY()

public:
    APossibilityShiftActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|PossibilityShift")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|PossibilityShift")
    TObjectPtr<UStaticMeshComponent> PresentTimelineMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|PossibilityShift")
    TObjectPtr<UStaticMeshComponent> RejectedTimelineMesh;

    UFUNCTION(BlueprintCallable, Category="Edrath|PossibilityShift")
    void ApplyTimelineState(ETimelineState NewState);

    UFUNCTION(BlueprintPure, Category="Edrath|PossibilityShift")
    ETimelineState GetCurrentActorTimelineState() const { return CurrentState; }

protected:
    virtual void BeginPlay() override;

private:
    ETimelineState CurrentState = ETimelineState::PresentTimeline;
};
