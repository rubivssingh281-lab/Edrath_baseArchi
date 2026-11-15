#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/EdrathTypes.h"
#include "KaelModularRigComponent.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class USkeletalMesh;
class UStaticMesh;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKaelOutfitChangedSignature, EKaelLifeEchoArchetype, NewArchetype);

/**
 * Standard socket identifiers for Kael's UE5 Mannequin Skeleton standard.
 */
namespace KaelSkeletalSockets
{
    const FName SpineHolster = FName(TEXT("spine_03_holster"));
    const FName HandLeftRelic = FName(TEXT("hand_l_relic"));
    const FName HandRightWeapon = FName(TEXT("hand_r_weapon"));
    const FName ClavicleCenterClock = FName(TEXT("bone_chest_clock"));
    const FName CameraFocus = FName(TEXT("socket_camera_focus"));
    const FName FootLeft = FName(TEXT("foot_l"));
    const FName FootRight = FName(TEXT("foot_r"));
}

/**
 * Defines modular outfit asset assignments for each of Kael's archetypes.
 */
USTRUCT(BlueprintType)
struct FKaelModularOutfitDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|ModularRig")
    TSoftObjectPtr<USkeletalMesh> HeadMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|ModularRig")
    TSoftObjectPtr<USkeletalMesh> TorsoMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|ModularRig")
    TSoftObjectPtr<USkeletalMesh> ArmsMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|ModularRig")
    TSoftObjectPtr<USkeletalMesh> LegsMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|ModularRig")
    TSoftObjectPtr<USkeletalMesh> CloakMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|Character|ModularRig")
    FLinearColor PrimaryColorTint = FLinearColor::White;
};

/**
 * UKaelModularRigComponent
 * Manages modular character mesh attachments (Head, Torso, Arms, Legs, Cloak),
 * synchronized animation deformation via Leader Pose Component, and weapon/relic
 * socket mounts adhering to the 3D Modeling and Technical Art Pipeline Guide.
 */
UCLASS(ClassGroup=(EdrathCharacter), meta=(BlueprintSpawnableComponent))
class EDRATH_API UKaelModularRigComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKaelModularRigComponent();

    /** Initializes the modular rig by binding all modular sub-meshes to follow the master body mesh. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Character|ModularRig")
    void InitializeModularRig(USkeletalMeshComponent* InMasterBodyMesh);

    /** Sets the active Life-Echo archetype outfit and swaps corresponding modular pieces. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Character|ModularRig")
    void SetLifeEchoArchetype(EKaelLifeEchoArchetype NewArchetype);

    /** Returns currently equipped Life-Echo archetype. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|ModularRig")
    EKaelLifeEchoArchetype GetLifeEchoArchetype() const { return CurrentArchetype; }

    /** Sets a specific modular skeletal mesh part (e.g. "Head", "Torso", "Arms", "Legs", "Cloak"). */
    UFUNCTION(BlueprintCallable, Category="Edrath|Character|ModularRig")
    bool SetModularMeshPart(FName SlotName, USkeletalMesh* NewMesh);

    /** Mounts primary weapon to hand or back holster socket. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Character|ModularRig")
    void EquipWeaponMesh(UStaticMesh* WeaponMesh, bool bHolsteredOnBack = false);

    /** Mounts off-hand relic to hand_l_relic socket. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Character|ModularRig")
    void EquipRelicMesh(UStaticMesh* RelicMesh);

    /** Returns whether the master body mesh is valid and modular components are bound. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|ModularRig")
    bool IsModularRigInitialized() const { return MasterBodyMesh.IsValid(); }

    /** Returns world transform of a named skeletal socket. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|ModularRig")
    FTransform GetSkeletalSocketTransform(FName SocketName) const;

    /** Returns world location of a named skeletal socket. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|ModularRig")
    FVector GetSkeletalSocketLocation(FName SocketName) const;

    /** Returns whether a socket exists on the master skeleton. */
    UFUNCTION(BlueprintPure, Category="Edrath|Character|ModularRig")
    bool DoesSocketExist(FName SocketName) const;

    UPROPERTY(BlueprintAssignable, Category="Edrath|Character|ModularRig")
    FOnKaelOutfitChangedSignature OnOutfitChanged;

    // Modular Skeletal Mesh Sub-Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<USkeletalMeshComponent> HeadMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<USkeletalMeshComponent> TorsoMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<USkeletalMeshComponent> ArmsMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<USkeletalMeshComponent> LegsMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<USkeletalMeshComponent> CloakMeshComponent;

    // Equipment Sockets Static Mesh Sub-Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<UStaticMeshComponent> RightHandWeaponComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<UStaticMeshComponent> LeftHandRelicComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Components")
    TObjectPtr<UStaticMeshComponent> BackHolsterComponent;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Edrath|Character|ModularRig|Presets")
    TMap<EKaelLifeEchoArchetype, FKaelModularOutfitDefinition> ArchetypeOutfitPresets;

private:
    TWeakObjectPtr<USkeletalMeshComponent> MasterBodyMesh;
    EKaelLifeEchoArchetype CurrentArchetype = EKaelLifeEchoArchetype::ForgottenSoldier_Act1;
};
