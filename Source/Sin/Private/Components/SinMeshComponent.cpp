// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SinMeshComponent.h"

#include "Inventory/Inventory.h"
#include "Inventory/Items/SinItemFragment_Equipment.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Components/MeshComponent.h"
#include "Engine/StreamableRenderAsset.h"

void USinMeshComponent::ManageMorphAdditive_Implementation(const FSinNamedFloat& Morph, bool Add)
{
	if (!GetSkeletalMeshAsset()->K2_GetAllMorphTargetNames().Contains(Morph.ID))
	{
		ManageDynamicBodyScalar(Morph, Add);
		return;
	}
	float PreviousValue = GetMorphTarget(Morph.ID);
	Add ? SetMorphTarget(Morph.ID, (GetMorphTarget(Morph.ID) + Morph.Value)) :
		SetMorphTarget(Morph.ID, (GetMorphTarget(Morph.ID) + Morph.Value * -1));
	OnSignalMorphChangedDelegate.Broadcast(Morph, GetMorphTarget(Morph.ID), PreviousValue);
}

void USinMeshComponent::ManageDynamicBodyScalar_Implementation(const FSinNamedFloat& Scalar, bool Add)
{
}


void USinMeshComponent::ManageMorphAdditives_Implementation(const TArray<FSinNamedFloat>& Morphs, bool Add)
{
	if (Morphs.IsEmpty())
	{
		return;
	}
	for (auto& Elem : Morphs)
	{
		ManageMorphAdditive(Elem, Add);
	}
}

void USinMeshComponent::SinSetMorph_Implementation(const FSinNamedFloat& Morph)
{
	if (!GetSkeletalMeshAsset()->K2_GetAllMorphTargetNames().Contains(Morph.ID))
	{
		ManageDynamicBodyScalar(Morph, true);
		return;
	}
	float PreviousValue = GetMorphTarget(Morph.ID);
	SetMorphTarget(Morph.ID, Morph.Value);
	OnSignalMorphChangedDelegate.Broadcast(Morph, GetMorphTarget(Morph.ID), PreviousValue);

}

void USinMeshComponent::SinSetMorphs_Implementation(const TArray<FSinNamedFloat>& Morphs)
{
	if (Morphs.IsEmpty())
	{
		return;
	}
	for (auto& Elem : Morphs)
	{
		SinSetMorph(Elem);
	}
}

void USinMeshComponent::SinGetMorphTarget(bool& Found, float& Value, FName MorphTargetName) const
{
	//Found = GetMorphTargetCurves().Contains(MorphTargetName);
	const float* CurveValPtr = GetMorphTargetCurves().Find(MorphTargetName);
	CurveValPtr ? Found = true : Found = false;
	if (CurveValPtr)
	{
		Value = *CurveValPtr;
	}
}

void USinMeshComponent::BindToInventory(UInventory* Inventory)
{
	if (!Inventory){return;} BoundInventory = Inventory;
	BoundInventory->OnInventoryEntryChanged.AddDynamic(this, &USinMeshComponent::HandleInventoryEntryChanged);
	Inventory->OnInventoryEntryAdded.AddDynamic(this,&USinMeshComponent::HandleInventoryEntryAdded);
	Inventory->OnInventoryEntryRemoved.AddDynamic(this, &USinMeshComponent::HandleInventoryEntryRemoved);
}

bool USinMeshComponent::IsEntryInEquipmentContainer(
	UInventory* Inventory,
	const FSinInventoryEntry& Entry) const
{
	if (!Inventory)
	{
		return false;
	}

	const FSinInventoryContainerState* Container =
		Inventory->FindContainerStateById(Entry.ContainerId);

	if (!Container)
	{
		UE_LOG(LogTemp, Warning, TEXT("No container found for EntryId: %s ContainerId: %s"),
			*Entry.EntryId.ToString(),
			*Entry.ContainerId.ToString());

		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Entry %s ContainerTag: %s EquipmentTag: %s"),
		*Entry.EntryId.ToString(),
		*Container->ContainerTag.ToString(),
		*ItemEquipmentTag.ToString());

	return Container->ContainerTag.MatchesTag(ItemEquipmentTag);
}

void USinMeshComponent::HandleInventoryEntryChanged(UInventory* Inventory, const FSinInventoryEntry& OldEntry, const FSinInventoryEntry& NewEntry)
{
	if (!Inventory){return;}

	const bool bWasEquipped = IsEntryInEquipmentContainer(Inventory, OldEntry);
	const bool bIsEquipped = IsEntryInEquipmentContainer(Inventory, NewEntry);

	if (bWasEquipped || bIsEquipped)
	{
		RefreshEquipmentFragmentsFromInventory(Inventory);
	}
}

void USinMeshComponent::HandleInventoryEntryAdded(UInventory* Inventory, const FSinInventoryEntry& Entry)
{
	if (!Inventory || !Entry.ItemDefinition){return;}

	if (!IsEntryInEquipmentContainer(Inventory, Entry)){return;}

	if (USinItemFragment_Equipment* EquipFragment =
		Entry.ItemDefinition->GetEquipmentFragment())
	{
		ActiveEquipmentFragmentsBySlot.Add(Entry.SlotIndex, EquipFragment);
		RebuildEquipmentVisuals();
	}
}

void USinMeshComponent::HandleInventoryEntryRemoved(UInventory* Inventory,const FSinInventoryEntry& RemovedEntry)
{
	if (!Inventory || !RemovedEntry.ItemDefinition){return;}

	if (!IsEntryInEquipmentContainer(Inventory, RemovedEntry)){return;}

	ActiveEquipmentFragmentsBySlot.Remove(RemovedEntry.SlotIndex);
	RebuildEquipmentVisuals();
}

void USinMeshComponent::RebuildEquipmentVisuals()
{
	ClearEquipmentVisuals();

	for (const TPair<int32, TObjectPtr<USinItemFragment_Equipment>>& Pair
		: ActiveEquipmentFragmentsBySlot)
	{
		const int32 SlotIndex = Pair.Key;
		const USinItemFragment_Equipment* Fragment = Pair.Value;

		if (!Fragment)
		{
			continue;
		}

		SetEquipmentMeshAtIndex(
			SlotIndex,
			Fragment->Mesh,
			true
		);
	}
}

void USinMeshComponent::ClearEquipmentVisuals()
{
	for (TPair<int32, TObjectPtr<UMeshComponent>>& Pair
		: AttachedEquipmentMeshesBySlot)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}

	AttachedEquipmentMeshesBySlot.Empty();
}

UMeshComponent* USinMeshComponent::SetEquipmentMeshAtIndex(int32 Index, TSoftObjectPtr<UStreamableRenderAsset> Mesh, bool bAdd)
{
	if (!bAdd)
	{
		if (TObjectPtr<UMeshComponent>* Existing =
			AttachedEquipmentMeshesBySlot.Find(Index))
		{
			if (*Existing)
			{
				(*Existing)->DestroyComponent();
			}

			AttachedEquipmentMeshesBySlot.Remove(Index);
		}

		return nullptr;
	}

	if (Mesh.IsNull()){return nullptr;}

	SetEquipmentMeshAtIndex(Index, TSoftObjectPtr<UStreamableRenderAsset>(), false);

	UStreamableRenderAsset* LoadedAsset = Mesh.LoadSynchronous();
	
	if (!LoadedAsset){return nullptr;}

	UMeshComponent* NewComponent = nullptr;

	if (USkeletalMesh* LoadedSkeletalMesh = Cast<USkeletalMesh>(LoadedAsset))
	{
		USkeletalMeshComponent* SkeletalComp =
			NewObject<USkeletalMeshComponent>(GetOwner());

		if (!SkeletalComp)
		{
			return nullptr;
		}

		SkeletalComp->SetSkeletalMesh(LoadedSkeletalMesh);
		SkeletalComp->SetLeaderPoseComponent(this);

		NewComponent = SkeletalComp;
	}
	else if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(LoadedAsset))
	{
		UStaticMeshComponent* StaticComp =
			NewObject<UStaticMeshComponent>(GetOwner());

		if (!StaticComp)
		{
			return nullptr;
		}

		StaticComp->SetStaticMesh(StaticMesh);

		NewComponent = StaticComp;
	}

	if (!NewComponent)
	{
		return nullptr;
	}

	NewComponent->RegisterComponent();

	NewComponent->AttachToComponent(
		this,
		FAttachmentTransformRules::SnapToTargetIncludingScale
	);

	AttachedEquipmentMeshesBySlot.Add(Index, NewComponent);

	return NewComponent;
}

void USinMeshComponent::RefreshEquipmentFragmentsFromInventory(UInventory* Inventory)
{
	if (!Inventory)
	{
		return;
	}

	ActiveEquipmentFragmentsBySlot.Empty();

	for (const FSinInventoryEntry& Entry : Inventory->ItemInventory)
	{
		if (!Entry.ItemDefinition)
		{
			continue;
		}

		if (!IsEntryInEquipmentContainer(Inventory, Entry))
		{
			continue;
		}

		if (USinItemFragment_Equipment* EquipFragment =
			Entry.ItemDefinition->GetEquipmentFragment())
		{
			ActiveEquipmentFragmentsBySlot.Add(Entry.SlotIndex, EquipFragment);
		}
	}

	RebuildEquipmentVisuals();
}
