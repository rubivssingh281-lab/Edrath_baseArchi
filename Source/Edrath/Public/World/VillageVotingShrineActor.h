#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "VillageVotingShrineActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVillageVoteResolvedSignature, bool, bResurrectDeceased, bool, bExposeMayor);

/**
 * AVillageVotingShrineActor
 * Implements the annual ballot shrine for Quest.VillageVotesOnDead (The Village That Votes on Its Dead).
 * Villagers vote to resurrect one dead citizen, retroactively erasing one living person.
 * Kael investigates the ballot urn to uncover the mayor's rigged vote against his accuser.
 */
UCLASS()
class EDRATH_API AVillageVotingShrineActor : public AActor
{
    GENERATED_BODY()

public:
    AVillageVotingShrineActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> BallotUrnMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<UStaticMeshComponent> LedgerPedestalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Quests")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> MayorActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Quests")
    TObjectPtr<AActor> AccuserGhostActor;

    /** Forensically examines the ballots to prove the mayor swapped his accuser out of existence. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool InspectVotingLedger();

    /** Resolves the village vote outcome. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Quests")
    bool CastFinalVote(bool bResurrectDeceased, bool bExposeMayorCorruption);

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsLedgerInspected() const { return bLedgerInspected; }

    UFUNCTION(BlueprintPure, Category="Edrath|Quests")
    bool IsVoteResolved() const { return bVoteResolved; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Quests")
    FOnVillageVoteResolvedSignature OnVoteResolved;

private:
    bool bLedgerInspected = false;
    bool bVoteResolved = false;
};
