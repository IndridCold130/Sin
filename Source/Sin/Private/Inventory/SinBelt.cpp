// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/SinBelt.h"
#include "Misc/SinLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GAS/SinASC.h"
#include "Interfaces/I_CombatInterface.h"

USinBelt::USinBelt()
{
	ActiveItemIndex = 0;
	InventoryType = TAG_Item_Equipment_Weapon;
	InventorySize = 3;
}

bool USinBelt::SlotRules(UGameItemBase* Item, int32 Slot)
{
	bool LocFound;
	FGameplayTag LocalWeaponTypeTag;
	USinLibrary::GetFirstMatchingGPT(Item->ItemTags, TAG_Item_Equipment_Weapon, false, LocFound, LocalWeaponTypeTag);
	if (!LocFound)
	{
		return false;
	}
	if (InventoryType == TAG_Item_Equipment_Weapon_MainHand || InventoryType == TAG_Item_Equipment_Weapon)
	{
		return LocalWeaponTypeTag == TAG_Item_Equipment_Weapon_MainHand || LocalWeaponTypeTag == TAG_Item_Equipment_Weapon
			|| LocalWeaponTypeTag == TAG_Item_Equipment_Weapon_OneHand;
	}
	if (InventoryType == TAG_Item_Equipment_Weapon_OffHand)
	{
		return LocalWeaponTypeTag == TAG_Item_Equipment_Weapon || LocalWeaponTypeTag == TAG_Item_Equipment_Weapon_OffHand ||
			LocalWeaponTypeTag == TAG_Item_Equipment_Weapon_OneHand;
	}
	return Super::SlotRules(Item, Slot);
}

int32 USinBelt::AttemptSwapActiveIndex()
{
	int32 NewIndex = ActiveItemIndex;
	if (InventoryItemCount() > 1)
	{
		for (int32 i = 0; i < Container.Num(); i++)
		{
			NewIndex = (NewIndex + 1) % Container.Num();
			if (Container[NewIndex] != nullptr)
			{
				break;
			}
		}
		if (NewIndex != ActiveItemIndex)
		{
				AttemptSwapActiveIndexPRC(NewIndex);
				this->OnSignalQuickSlotEquip.Broadcast(false, this, ActiveItemIndex, Container[ActiveItemIndex]);
				ActiveItemIndex = NewIndex;
				this->OnSignalQuickSlotEquip.Broadcast(true, this, NewIndex, Container[NewIndex]);
				return ActiveItemIndex;
		}
	}
	return ActiveItemIndex;
}

void USinBelt::AttemptSwapActiveIndexPRC_Implementation(int32 NewIndex)
{
	AttemptSwapActiveIndexMulti(NewIndex);
	return;
}

bool USinBelt::AttemptSwapActiveIndexPRC_Validate(int32 NewIndex) { return true; }

void USinBelt::AttemptSwapActiveIndexMulti_Implementation(int32 NewIndex)
{
	if (Activated)
	{

	}
	if (!(Cast<ASinCharacter>(this->GetOwner())->IsLocallyControlled()))
	{
		this->OnSignalQuickSlotEquip.Broadcast(false, this, ActiveItemIndex, Container[ActiveItemIndex]);
		ActiveItemIndex = NewIndex;
		this->OnSignalQuickSlotEquip.Broadcast(true, this, NewIndex, Container[NewIndex]);
		return;
	}
}

void USinBelt::HandleActiveItem(bool Added, int32 NewIndex, UGameItemBase* Item, int32 OldIndex, UInventory* SrcInventory)
{
	if (Added)
	{
		ActiveItemIndex = NewIndex;
		this->OnSignalQuickSlotEquip.Broadcast(true, this, NewIndex, Item);
		return;
	}
	else
	{
		this->OnSignalQuickSlotEquip.Broadcast(false, this, NewIndex, Item);
		if (InventoryItemCount() > 0)
		{
			int32 LocIndex = ActiveItemIndex;
			for (int32 i = 0; i < Container.Num(); i++)
			{
				LocIndex = (LocIndex + 1) % Container.Num();
				if (Container[LocIndex] != nullptr)
				{
					UGameItemBase* LocItem = Container[LocIndex];
					ActiveItemIndex = LocIndex;
					this->OnSignalQuickSlotEquip.Broadcast(true, this, LocIndex, LocItem);
					return;
				}
			}
		}
	}
}

void USinBelt::ItemAdded(UInventory* NewInventory, int32 NewIndex, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
	if (NewIndex == ActiveItemIndex || InventoryItemCount() < 2)
	{
		HandleActiveItem(true, NewIndex, Item, SrcIndex, SrcInventory);
	}
}

void USinBelt::ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
	if (Index == ActiveItemIndex)
	{
		HandleActiveItem(false, Index, Item, SrcIndex, SrcInventory);
	}
}

void USinBelt::ActivateBelt(bool Activate)
{
}

void USinBelt::BeginPlay()
{
	Super::BeginPlay();
	OnSignalItemAdded.AddDynamic(this, &USinBelt::ItemAdded);
	OnSignalItemRemoved.AddDynamic(this, &USinBelt::ItemRemoved);
}

void USinBelt::HandleClient_Implementation(bool Added, int32 NewIndex, UGameItemBase* Item, int32 OldIndex, UInventory* SrcInventory)
{
	if (Added)
	{
		if (GetNetMode() == NM_Client)
		{
			AddItem(Item, NewIndex, SrcInventory, OldIndex);
		}
		this->OnSignalItemAdded.Broadcast(this, NewIndex, Item, OldIndex, SrcInventory);
		return;
	}
	else
	{
		if (GetNetMode() == NM_Client)
		{
			RemoveItem(NewIndex);
		}
		this->OnSignalItemRemoved.Broadcast(this, NewIndex, Item, OldIndex, SrcInventory);
		return;
	}
}

void USinBelt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USinBelt, ActiveItemIndex);
	DOREPLIFETIME(USinBelt, Activated);
}
