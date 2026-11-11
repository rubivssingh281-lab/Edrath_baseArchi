#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "GameplayTagContainer.h"
#include "BattlefieldWraithCharacter.generated.h"

class AAct1PreAlphaSliceDirector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattlefieldWraithDefeatedSignature, class ABattlefieldWraithCharacter*, DefeatedWraith);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWraithStaggeredSignature, class ABattlefieldWraithCharacter*, StaggeredWraith, float, StaggerDuration);

/**
 * Battlefield Wraith - Zone 1 Echo Combat Enemy
 * A temporal wraith trapped in the battlefield of ancient dead armies.
 * Features dual timeline phasing (Present vs. Rejected timeline) and vulnerable
 * to Kael's 120-frame Black Clock Echo Strikes (TriggerEchoStrike).
 */
UCLASS()
class EDRATH_API ABattlefieldWraithCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABattlefieldWraithCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Combat|Attributes")
    float MaxHealth = 60.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Combat|Attributes")
    float CurrentHealth = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Combat|Attributes")
    float BaseSlashVulnerabilityMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Combat|Attributes")
    float EchoStrikeVulnerabilityMultiplier = 2.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Combat|State")
    bool bIsStaggered = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Combat|State")
    bool bIsDefeated = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Combat|Timeline")
    ETimelineState ActiveTimelinePhase = ETimelineState::PresentTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Combat|Timeline")
    bool bTimelinePhasedShieldActive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Combat|FX")
    float DissolveProgress = 0.0f;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Combat|Events")
    FOnBattlefieldWraithDefeatedSignature OnWraithDefeated;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Combat|Events")
    FOnWraithStaggeredSignature OnWraithStaggered;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Combat")
    bool TakeSlashDamage(float InDamage, AActor* DamageCauser);

    UFUNCTION(BlueprintCallable, Category = "Edrath|Combat")
    bool TakeEchoStrikeDamage(float InDamage, AActor* DamageCauser, float ProjectionDelay = 1.2f);

    UFUNCTION(BlueprintCallable, Category = "Edrath|Combat")
    void ApplyTimelinePhaseShift(ETimelineState NewPhase);

    UFUNCTION(BlueprintPure, Category = "Edrath|Combat")
    bool IsDefeated() const { return bIsDefeated; }

    UFUNCTION(BlueprintPure, Category = "Edrath|Combat")
    bool IsStaggered() const { return bIsStaggered; }

    UFUNCTION(BlueprintCallable, Category = "Edrath|Combat")
    void BindToDirector(AAct1PreAlphaSliceDirector* InDirector);

private:
    float StaggerRemainingTime = 0.0f;

    UPROPERTY()
    TWeakObjectPtr<AAct1PreAlphaSliceDirector> BoundDirector;
};
