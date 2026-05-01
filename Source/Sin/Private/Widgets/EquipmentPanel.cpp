// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EquipmentPanel.h"
#include "Widgets/InventorySlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SinPlayerController.h"

void UEquipmentPanel::NativeConstruct()
{
	InvSlotData.Empty();
	Super::NativeConstruct();
	InitializeSlots();
}

void UEquipmentPanel::NativeDestruct()
{
	Super::NativeDestruct();
}

void UEquipmentPanel::MoveItemClient(UInventoryPanel* Panel, int32 OldIndex, int32 DstIndex, FGameplayTag IncomingSlotType, FGameplayTag LocalSlotType)
{
	Super::MoveItemClient(Panel, OldIndex, DstIndex, IncomingSlotType, LocalSlotType);
}

void UEquipmentPanel::InitializeSlots()
{
	if (!InvSlotData.Contains(TAG_Item_Equipment))
	{
		InvSlotData.Add(TAG_Item_Equipment);
	}
	InvSlotData.Add(TAG_Item_Equipment_Weapon);
	InvSlotData.Add(TAG_Item_Equipment_Weapon_OffHand);
	TArray<UUserWidget*> LocalSlots;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, LocalSlots, UInventorySlot::StaticClass(), false);
	for (UUserWidget* LocalSlot : LocalSlots)
	{
		UInventorySlot* LocSlot = Cast<UInventorySlot>(LocalSlot);
		if (LocSlot)
		{
			if (LocSlot->SlotType == TAG_Item_Equipment)
			{
				if (EquipmentContainer->HasChild(LocSlot) || JewelleryContainer->HasChild(LocSlot))
				{
					LocSlot->MasterPanel = this;
					InvSlotData[TAG_Item_Equipment].Add(LocSlot);
				}
			}
			if (LocSlot->SlotType == TAG_Item_Equipment_Weapon)
			{
				if (WeaponSlotContainer->HasChild(LocSlot))
				{
					LocSlot->MasterPanel = this;
					InvSlotData[TAG_Item_Equipment_Weapon].Add(LocSlot);
				}
			}
			if (LocSlot->SlotType == TAG_Item_Equipment_Weapon_OffHand)
			{
				if (OffHandSlotContainer->HasChild(LocSlot))
				{
					LocSlot->MasterPanel = this;
					InvSlotData[TAG_Item_Equipment_Weapon_OffHand].Add(LocSlot);
				}
			}
		}
	}
}
