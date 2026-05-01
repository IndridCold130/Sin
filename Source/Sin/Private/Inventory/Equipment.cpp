// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Equipment.h"

class UGameItemBase;

UEquipment::UEquipment()
{
	InventoryType = TAG_Item_Equipment;
}

bool UEquipment::SlotRules(UGameItemBase* Item, int32 Slot)
{
	if (Super::SlotRules(Item, Slot))
	{
		if(EquipmentSlotRules.Contains(Slot))
		{
			if (Item->ItemTags.Num() > 0)
			{
				if (Item->GetItemTypeTag() == EquipmentSlotRules[Slot])
				{
					return true;
				}
			}
		}
	}
	return false;
}
