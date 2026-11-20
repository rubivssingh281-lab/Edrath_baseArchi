#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "FirstArchivistBossCharacter.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EArchivistBossPhase : uint8
{
    Phase1_EchoProjection,
    Phase2_CrystallineShield,
    Phase3_PossibilityCollapse,
    Defeated
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArchivistPhaseChangedSignature, EArchivistBossPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchivistShieldBrokenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchivistDefeatedEventSignature);

/**
 * AFirstArchivistBossCharacter
 * Climax boss version of Kael (SK_First_Archivist) fought at the end of Act VI.
 * Possesses crystalline time-archive armor, multi-phase combat states, echo projections,
 * and reality-dissolving strike capabilities. Defeating him strips the reality-pruning safeguards.
 */
UCLASS()
class EDRATH_API AFirstArchivistBossCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFirstArchivistBossCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Boss")
    TObjectPtr<UStaticMeshComponent> TimeArchiveCrownMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Boss")
    TObjectPtr<UStaticMeshComponent> ChronoBladeMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Boss")
    float MaxHealth = 1200.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Boss")
    float CurrentHealth = 1200.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Boss")
    float ShieldStability = 100.0f;

    /** Applies incoming damage, accounting for active timeline shield immunity during Phase 2. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Boss")
    float ApplyBossDamage(float DamageAmount, bool bIsEchoStrike);

    /** Breaks the crystalline shield during Phase 2 using a Black Clock Echo Strike. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Boss")
    bool ShatterCrystallineShield(float StaggerValue);

    UFUNCTION(BlueprintPure, Category="Edrath|Boss")
    EArchivistBossPhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure, Category="Edrath|Boss")
    float GetHealthRatio() const { return MaxHealth > 0.0f ? (CurrentHealth / MaxHealth) : 0.0f; }

    UFUNCTION(BlueprintPure, Category="Edrath|Boss")
    bool IsShieldActive() const { return CurrentPhase == EArchivistBossPhase::Phase2_CrystallineShield && ShieldStability > 0.0f; }

    UFUNCTION(BlueprintPure, Category="Edrath|Boss")
    bool IsDefeated() const { return CurrentPhase == EArchivistBossPhase::Defeated; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Boss")
    FOnArchivistPhaseChangedSignature OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Boss")
    FOnArchivistShieldBrokenSignature OnShieldBroken;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Boss")
    FOnArchivistDefeatedEventSignature OnBossDefeated;

protected:
    virtual void BeginPlay() override;
    void AdvancePhase(EArchivistBossPhase NewPhase);

private:
    EArchivistBossPhase CurrentPhase = EArchivistBossPhase::Phase1_EchoProjection;
};
