#include "Character/KaelModularRigComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

UKaelModularRigComponent::UKaelModularRigComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UKaelModularRigComponent::InitializeModularRig(USkeletalMeshComponent* InMasterBodyMesh)
{
    if (!InMasterBodyMesh)
    {
        return;
    }

    MasterBodyMesh = InMasterBodyMesh;
    AActor* OwnerActor = GetOwner();

    // Create modular sub-components if they do not already exist on the actor
    auto EnsureSkeletalSlot = [OwnerActor, InMasterBodyMesh](TObjectPtr<USkeletalMeshComponent>& SlotComp, FName SlotName)
    {
        if (!SlotComp && OwnerActor)
        {
            SlotComp = NewObject<USkeletalMeshComponent>(OwnerActor, SlotName);
            SlotComp->RegisterComponent();
            SlotComp->AttachToComponent(InMasterBodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
        }
        if (SlotComp)
        {
            SlotComp->SetLeaderPoseComponent(InMasterBodyMesh);
        }
    };

    EnsureSkeletalSlot(HeadMeshComponent, TEXT("Kael_ModularHead"));
    EnsureSkeletalSlot(TorsoMeshComponent, TEXT("Kael_ModularTorso"));
    EnsureSkeletalSlot(ArmsMeshComponent, TEXT("Kael_ModularArms"));
    EnsureSkeletalSlot(LegsMeshComponent, TEXT("Kael_ModularLegs"));
    EnsureSkeletalSlot(CloakMeshComponent, TEXT("Kael_ModularCloak"));

    // Ensure weapon and equipment static mesh sockets
    auto EnsureStaticSocket = [OwnerActor, InMasterBodyMesh](TObjectPtr<UStaticMeshComponent>& MeshComp, FName CompName, FName SocketName)
    {
        if (!MeshComp && OwnerActor)
        {
            MeshComp = NewObject<UStaticMeshComponent>(OwnerActor, CompName);
            MeshComp->RegisterComponent();
            MeshComp->AttachToComponent(InMasterBodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
        }
    };

    EnsureStaticSocket(RightHandWeaponComponent, TEXT("Kael_WeaponRight"), KaelSkeletalSockets::HandRightWeapon);
    EnsureStaticSocket(LeftHandRelicComponent, TEXT("Kael_RelicLeft"), KaelSkeletalSockets::HandLeftRelic);
    EnsureStaticSocket(BackHolsterComponent, TEXT("Kael_WeaponBackHolster"), KaelSkeletalSockets::SpineHolster);
}

void UKaelModularRigComponent::SetLifeEchoArchetype(EKaelLifeEchoArchetype NewArchetype)
{
    CurrentArchetype = NewArchetype;

    if (const FKaelModularOutfitDefinition* Preset = ArchetypeOutfitPresets.Find(NewArchetype))
    {
        if (Preset->HeadMesh.IsValid()) SetModularMeshPart(TEXT("Head"), Preset->HeadMesh.Get());
        if (Preset->TorsoMesh.IsValid()) SetModularMeshPart(TEXT("Torso"), Preset->TorsoMesh.Get());
        if (Preset->ArmsMesh.IsValid()) SetModularMeshPart(TEXT("Arms"), Preset->ArmsMesh.Get());
        if (Preset->LegsMesh.IsValid()) SetModularMeshPart(TEXT("Legs"), Preset->LegsMesh.Get());
        if (Preset->CloakMesh.IsValid()) SetModularMeshPart(TEXT("Cloak"), Preset->CloakMesh.Get());
    }

    OnOutfitChanged.Broadcast(CurrentArchetype);
}

bool UKaelModularRigComponent::SetModularMeshPart(FName SlotName, USkeletalMesh* NewMesh)
{
    USkeletalMeshComponent* TargetComp = nullptr;
    const FString SlotStr = SlotName.ToString().ToLower();

    if (SlotStr == TEXT("head"))
    {
        TargetComp = HeadMeshComponent;
    }
    else if (SlotStr == TEXT("torso") || SlotStr == TEXT("chest"))
    {
        TargetComp = TorsoMeshComponent;
    }
    else if (SlotStr == TEXT("arms") || SlotStr == TEXT("hands"))
    {
        TargetComp = ArmsMeshComponent;
    }
    else if (SlotStr == TEXT("legs") || SlotStr == TEXT("greaves"))
    {
        TargetComp = LegsMeshComponent;
    }
    else if (SlotStr == TEXT("cloak") || SlotStr == TEXT("cape"))
    {
        TargetComp = CloakMeshComponent;
    }

    if (TargetComp)
    {
        TargetComp->SetSkeletalMesh(NewMesh);
        if (MasterBodyMesh.IsValid())
        {
            TargetComp->SetLeaderPoseComponent(MasterBodyMesh.Get());
        }
        return true;
    }

    return false;
}

void UKaelModularRigComponent::EquipWeaponMesh(UStaticMesh* WeaponMesh, bool bHolsteredOnBack)
{
    if (bHolsteredOnBack)
    {
        if (BackHolsterComponent)
        {
            BackHolsterComponent->SetStaticMesh(WeaponMesh);
            BackHolsterComponent->SetVisibility(true);
        }
        if (RightHandWeaponComponent)
        {
            RightHandWeaponComponent->SetVisibility(false);
        }
    }
    else
    {
        if (RightHandWeaponComponent)
        {
            RightHandWeaponComponent->SetStaticMesh(WeaponMesh);
            RightHandWeaponComponent->SetVisibility(true);
        }
        if (BackHolsterComponent)
        {
            BackHolsterComponent->SetVisibility(false);
        }
    }
}

void UKaelModularRigComponent::EquipRelicMesh(UStaticMesh* RelicMesh)
{
    if (LeftHandRelicComponent)
    {
        LeftHandRelicComponent->SetStaticMesh(RelicMesh);
        LeftHandRelicComponent->SetVisibility(RelicMesh != nullptr);
    }
}

FTransform UKaelModularRigComponent::GetSkeletalSocketTransform(FName SocketName) const
{
    if (MasterBodyMesh.IsValid())
    {
        return MasterBodyMesh->GetSocketTransform(SocketName, RTS_World);
    }
    return FTransform::Identity;
}

FVector UKaelModularRigComponent::GetSkeletalSocketLocation(FName SocketName) const
{
    if (MasterBodyMesh.IsValid())
    {
        return MasterBodyMesh->GetSocketLocation(SocketName);
    }
    return FVector::ZeroVector;
}

bool UKaelModularRigComponent::DoesSocketExist(FName SocketName) const
{
    if (MasterBodyMesh.IsValid())
    {
        return MasterBodyMesh->DoesSocketExist(SocketName);
    }
    return false;
}
