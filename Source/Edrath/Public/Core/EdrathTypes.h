#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EdrathTypes.generated.h"

UENUM(BlueprintType)
enum class ECombatActionState : uint8
{
    Idle,
    Attacking,
    Dodging,
    Parrying,
    Staggered
};

UENUM(BlueprintType)
enum class EEdrathCameraMode : uint8
{
    ExteriorBirdsEye,
    InteriorThirdPerson
};

UENUM(BlueprintType)
enum class ETimelineState : uint8
{
    PresentTimeline,
    RejectedTimeline,
    MemoryReconstruction
};

UENUM(BlueprintType)
enum class EEdrathRegion : uint8
{
    Veyl,
    Hollowgreen,
    AshenExpanse,
    Nhal,
    CrownOfWinter,
    UnwrittenSea
};

UENUM(BlueprintType)
enum class EEdrathEndingType : uint8
{
    None,
    PreservedWorld,
    UnwrittenWorld,
    MortalWorld,
    UncomfortableTruth
};

UENUM(BlueprintType)
enum class EEdrathQualityPreset : uint8
{
    Low_Mobile,
    Medium,
    High,
    Epic_PC
};

UENUM(BlueprintType)
enum class EKaelLifeEchoArchetype : uint8
{
    ForgottenSoldier_Act1,
    RoyalAssassin_Act2,
    PlaguedHealer_Act3,
    TyrantConqueror_Act3,
    SimpleFarmer_Act3,
    ShadowlessWraith_Act4,
    TimeArchivist_Act5
};

USTRUCT(BlueprintType)
struct FEchoActionFrame
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Edrath|EchoCombat")
    FVector WorldPosition = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|EchoCombat")
    FRotator WorldRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|EchoCombat")
    FName ActionMontageSection = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|EchoCombat")
    float BaseDamage = 25.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|EchoCombat")
    float TimestampSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct FEdrathEvidenceRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Narrative")
    FGameplayTag EvidenceTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Narrative")
    FText CurrentDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Narrative")
    bool bIsForeshadowingTwistClue = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Narrative")
    int32 MutationVersion = 1;
};

USTRUCT(BlueprintType)
struct FCompanionRelationshipState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companions")
    FName CompanionID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companions")
    float Loyalty = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companions")
    float SuspicionOfKael = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companions")
    bool bWithholdingCriticalTruth = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Companions")
    bool bHasTurnedHostileInNhal = false;
};
