#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/EdrathTypes.h"
#include "Core/EdrathChronalCombatInterface.h"
#include "Core/EdrathInteractableInterface.h"
#include "MaevraPursuerBossCharacter.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EMaevraEncounterState : uint8
{
    HostilePursuer,
    ShieldShattered_Kneeling,
    ClockBargainOffered,
    RecruitedAlly,
    Defeated
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaevraStateChangedSignature, EMaevraEncounterState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaevraRecruitedSignature, AActor*, Recruiter);

/**
 * AMaevraPursuerBossCharacter
 * Act II Royal Court Alchemist who relentlessly hunts Kael across Veyl as an elite pursuer boss.
 * Upon shattering her alchemical ward and reducing her health below 35%, she kneels and offers a pact
 * to join the party in exchange for using the Black Clock to restore her erased 40-year marriage.
 */
UCLASS()
class EDRATH_API AMaevraPursuerBossCharacter : public ACharacter, public IEdrathChronalCombatInterface, public IEdrathInteractableInterface
{
    GENERATED_BODY()

public:
    AMaevraPursuerBossCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Act2|Boss")
    TObjectPtr<UStaticMeshComponent> AlchemicalAlembicMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Act2|Boss")
    float MaxHealth = 480.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Act2|Boss")
    float CurrentHealth = 480.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Act2|Boss")
    float MaxPoise = 120.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Act2|Boss")
    float CurrentPoise = 120.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Act2|Boss")
    bool bAlchemicalBarrierActive = true;

    /** Accepts the Black Clock pact while Maevra is kneeling, converting her from pursuer into an ally. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act2|Boss")
    bool RecruitMaevraViaClockBargain(AActor* Recruiter);

    /** Executes lethal defeat if player rejects her plea or finishes her below threshold. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Act2|Boss")
    void DefeatMaevra();

    UFUNCTION(BlueprintPure, Category="Edrath|Act2|Boss")
    EMaevraEncounterState GetEncounterState() const { return EncounterState; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act2|Boss")
    float GetHealthRatio() const { return MaxHealth > 0.0f ? (CurrentHealth / MaxHealth) : 0.0f; }

    UFUNCTION(BlueprintPure, Category="Edrath|Act2|Boss")
    bool IsRecruited() const { return EncounterState == EMaevraEncounterState::RecruitedAlly; }

    // IEdrathChronalCombatInterface
    virtual float ReceiveChronalHit_Implementation(float DamageAmount, float PoiseDamage, bool bIsEchoStrike, AActor* InstigatorActor, const FVector& HitLocation) override;

    // IEdrathInteractableInterface
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionPrompt_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionTargetName_Implementation() const override;
    virtual bool ExecuteInteraction_Implementation(AActor* Interactor) override;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act2|Boss")
    FOnMaevraStateChangedSignature OnMaevraStateChanged;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Act2|Boss")
    FOnMaevraRecruitedSignature OnMaevraRecruited;

protected:
    virtual void BeginPlay() override;
    void TransitionEncounterState(EMaevraEncounterState NewState);

private:
    EMaevraEncounterState EncounterState = EMaevraEncounterState::HostilePursuer;
};
