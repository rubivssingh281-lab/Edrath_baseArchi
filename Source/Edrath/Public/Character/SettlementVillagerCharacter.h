#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "SettlementVillagerCharacter.generated.h"

class USphereComponent;
class UEdrathNarrativeSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVillagerSpokenSignature, class ASettlementVillagerCharacter*, Villager, AActor*, Interactor, const FText&, DialogueLine);

/**
 * Settlement Villager Character - Zone 3 Looping Settlement
 * An inhabitant of the 7-day looping settlement who recognizes Kael as an old friend
 * but possesses zero recollection of the preceding 7 days due to temporal loop reset.
 */
UCLASS()
class EDRATH_API ASettlementVillagerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASettlementVillagerCharacter();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Components")
    TObjectPtr<USphereComponent> DialogueTriggerSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Villager")
    FName VillagerName = TEXT("Villager_Bram");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Villager|Dialogue")
    FText NormalDayDialogue = FText::FromString(
        TEXT("Welcome back, Kael! Your workshop down the lane is right where you left it. Good to see you on your feet."));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Villager|Dialogue")
    FText LoopEndDaySevenDialogue = FText::FromString(
        TEXT("The crypt bell chimes seven times today... Why does every morning feel like I am repeating someone else's dream?"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edrath|Villager|Dialogue")
    FText BrokenLoopDialogue = FText::FromString(
        TEXT("The eighth dawn... By the Old Archive, the day has changed! The fog in my chest has lifted!"));

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Villager|State")
    int32 ObservedLoopResets = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Edrath|Villager|State")
    bool bHasTalkedThisCycle = false;

    UPROPERTY(BlueprintAssignable, Category = "Edrath|Villager|Events")
    FOnVillagerSpokenSignature OnVillagerSpoken;

    UFUNCTION(BlueprintCallable, Category = "Edrath|Villager")
    FText InteractWithVillager(AActor* Interactor);

    UFUNCTION(BlueprintPure, Category = "Edrath|Villager")
    FText GetContextualDialogue() const;

    UFUNCTION()
    void HandleSettlementLoopReset(int32 TotalLoopResets);

private:
    UEdrathNarrativeSubsystem* GetNarrativeSubsystem() const;
};
