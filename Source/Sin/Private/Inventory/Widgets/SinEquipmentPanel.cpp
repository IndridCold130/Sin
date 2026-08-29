#include "Inventory/Widgets/SinEquipmentPanel.h"
#include "Inventory/Inventory.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Widgets/InventorySlot.h"
#include "Inventory/SinInventoryEntryTypes.h"

void USinEquipmentPanel::SetEquipmentData(AActor* Owner)
{
	OwningActor = Owner; DataHolder = nullptr; if (!Owner){return;}
	TArray<UInventory*> Inventories; Owner->GetComponents<UInventory>(Inventories);
	
	for (UInventory* Inventory : Inventories)
	{
		if (!Inventory){continue;}

		if (Inventory->InventoryType == InventoryType)
		{
			DataHolder = Inventory;
			break;
		}
	}
	
	if (!DataHolder){return;}
	DataHolder->EnsureContainerIds(); InitializeEquipmentSlots();
	
	RefreshEquipmentSlots();
	
	if (!DataHolder->OnContainerChanged.IsAlreadyBound(this,&USinEquipmentPanel::HandleEquipmentContainerChanged))
	{
		DataHolder->OnContainerChanged.AddDynamic(this, &USinEquipmentPanel::HandleEquipmentContainerChanged);
	}
	if (!DataHolder->OnInventoryEntryAdded.IsAlreadyBound(this, &USinEquipmentPanel::HandleInventoryEntryAdded))
	{
		DataHolder->OnInventoryEntryAdded.AddDynamic(this, &USinEquipmentPanel::HandleInventoryEntryAdded);
	}

	if (!DataHolder->OnInventoryEntryRemoved.IsAlreadyBound(this, &USinEquipmentPanel::HandleInventoryEntryRemoved))
	{
		DataHolder->OnInventoryEntryRemoved.AddDynamic(this, &USinEquipmentPanel::HandleInventoryEntryRemoved);
	}

	if (!DataHolder->OnInventoryEntryChanged.IsAlreadyBound(this, &USinEquipmentPanel::HandleInventoryEntryChanged))
	{
		DataHolder->OnInventoryEntryChanged.AddDynamic(this, &USinEquipmentPanel::HandleInventoryEntryChanged);
	}
}

void USinEquipmentPanel::HandleInventoryEntryAdded(UInventory* Inventory, const FSinInventoryEntry& Entry)
{
	if (!DataHolder || Inventory != DataHolder) return;
	if (IsRelevantEntry(Entry)) RefreshEquipmentSlots();
}

void USinEquipmentPanel::HandleInventoryEntryRemoved(UInventory* Inventory, const FSinInventoryEntry& RemovedEntry)
{
	if (!DataHolder || Inventory != DataHolder) return;
	if (IsRelevantEntry(RemovedEntry)) RefreshEquipmentSlots();
}

void USinEquipmentPanel::HandleInventoryEntryChanged(UInventory* Inventory, const FSinInventoryEntry& OldEntry, const FSinInventoryEntry& NewEntry)
{
	if (!DataHolder || Inventory != DataHolder) return;
	if (IsRelevantEntry(OldEntry) || IsRelevantEntry(NewEntry))
	{
		RefreshEquipmentSlots();
	}
}

void USinEquipmentPanel::InitializeEquipmentSlots()
{
	if (!DataHolder){return;}

	// ---- Equipment ----
	if (const FSinInventoryContainerState* EquipmentContainer = DataHolder->FindContainerStateByTag(EquipmentContainerTag))
	{
		const FGuid Id = EquipmentContainer->ContainerId;

		if (HeadSlot)   HeadSlot->InitFixedContainerSlot(this, Id, 0);
		if (ChestSlot)  ChestSlot->InitFixedContainerSlot(this, Id, 1);
		if (HandsSlot)  HandsSlot->InitFixedContainerSlot(this, Id, 2);
		if (LegsSlot)   LegsSlot->InitFixedContainerSlot(this, Id, 3);
		if (FeetSlot)   FeetSlot->InitFixedContainerSlot(this, Id, 4);
		if (AmuletSlot) AmuletSlot->InitFixedContainerSlot(this, Id, 5);
		if (RingSlot1)  RingSlot1->InitFixedContainerSlot(this, Id, 6);
		if (RingSlot2)  RingSlot2->InitFixedContainerSlot(this, Id, 7);
	}
	
	// ---- Utility ----
	if (const FSinInventoryContainerState* UtilContainer = DataHolder->FindContainerStateByTag(UtilityContainerTag))
	{
		const FGuid Id = UtilContainer->ContainerId;

		if (UtilitySlot1) UtilitySlot1->InitFixedContainerSlot(this, Id, 0);
		if (UtilitySlot2) UtilitySlot2->InitFixedContainerSlot(this, Id, 1);
		if (UtilitySlot3) UtilitySlot3->InitFixedContainerSlot(this, Id, 2);
	}
	
	// ---- Weapons (NEW) ----
	if (const FSinInventoryContainerState* WeaponContainer = DataHolder->FindContainerStateByTag(WeaponContainerTag))
	{
		const FGuid Id = WeaponContainer->ContainerId;

		if (WeaponSlot0) WeaponSlot0->InitFixedContainerSlot(this, Id, 0);
		if (WeaponSlot1) WeaponSlot1->InitFixedContainerSlot(this, Id, 1);
		if (WeaponSlot2) WeaponSlot2->InitFixedContainerSlot(this, Id, 2);
		if (WeaponSlot3) WeaponSlot3->InitFixedContainerSlot(this, Id, 3);
	}
	
	// ---- Ammo (NEW) ----
	if (const FSinInventoryContainerState* AmmoContainer = DataHolder->FindContainerStateByTag(AmmoContainerTag))
	{
		const FGuid Id = AmmoContainer->ContainerId;

		if (AmmoSlot1) AmmoSlot1->InitFixedContainerSlot(this, Id, 0);
		if (AmmoSlot2) AmmoSlot2->InitFixedContainerSlot(this, Id, 1);
		if (AmmoSlot3) AmmoSlot3->InitFixedContainerSlot(this, Id, 2);
	}
}

void USinEquipmentPanel::RefreshEquipmentSlots()
{
	if (!DataHolder) return;

	// ---- Equipment ----
	if (const FSinInventoryContainerState* EquipmentContainer = DataHolder->FindContainerStateByTag(EquipmentContainerTag))
	{
		const FGuid Id = EquipmentContainer->ContainerId;

		if (HeadSlot)   HeadSlot->RefreshSlotByLocation(Id, 0);
		if (ChestSlot)  ChestSlot->RefreshSlotByLocation(Id, 1);
		if (HandsSlot)  HandsSlot->RefreshSlotByLocation(Id, 2);
		if (LegsSlot)   LegsSlot->RefreshSlotByLocation(Id, 3);
		if (FeetSlot)   FeetSlot->RefreshSlotByLocation(Id, 4);
		if (AmuletSlot) AmuletSlot->RefreshSlotByLocation(Id, 5);
		if (RingSlot1)  RingSlot1->RefreshSlotByLocation(Id, 6);
		if (RingSlot2)  RingSlot2->RefreshSlotByLocation(Id, 7);
	}

	// ---- Utility ----
	if (const FSinInventoryContainerState* UtilContainer = DataHolder->FindContainerStateByTag(UtilityContainerTag))
	{
		const FGuid Id = UtilContainer->ContainerId;

		if (UtilitySlot1) UtilitySlot1->RefreshSlotByLocation(Id, 0);
		if (UtilitySlot2) UtilitySlot2->RefreshSlotByLocation(Id, 1);
		if (UtilitySlot3) UtilitySlot3->RefreshSlotByLocation(Id, 2);
	}

	// ---- Weapons ----
	if (const FSinInventoryContainerState* WeaponContainer = DataHolder->FindContainerStateByTag(WeaponContainerTag))
	{
		const FGuid Id = WeaponContainer->ContainerId;

		if (WeaponSlot0) WeaponSlot0->RefreshSlotByLocation(Id, 0);
		if (WeaponSlot1) WeaponSlot1->RefreshSlotByLocation(Id, 1);
		if (WeaponSlot2) WeaponSlot2->RefreshSlotByLocation(Id, 2);
		if (WeaponSlot3) WeaponSlot3->RefreshSlotByLocation(Id, 3);
	}
	
	// ---- Ammo ----
	if (const FSinInventoryContainerState* AmmoContainer = DataHolder->FindContainerStateByTag(AmmoContainerTag))
	{
		const FGuid Id = AmmoContainer->ContainerId;

		if (AmmoSlot1) AmmoSlot1->RefreshSlotByLocation(Id, 0);
		if (AmmoSlot2) AmmoSlot2->RefreshSlotByLocation(Id, 1);
		if (AmmoSlot3) AmmoSlot3->RefreshSlotByLocation(Id, 2);
	}
}

void USinEquipmentPanel::HandleEquipmentContainerChanged(UInventory* Inventory, FGuid ContainerId)
{
	if (!DataHolder || Inventory != DataHolder) return;

	const FSinInventoryContainerState* EquipCont  = DataHolder->FindContainerStateByTag(EquipmentContainerTag);
	const FSinInventoryContainerState* UtilCont   = DataHolder->FindContainerStateByTag(UtilityContainerTag);
	const FSinInventoryContainerState* WeaponCont = DataHolder->FindContainerStateByTag(WeaponContainerTag);
	const FSinInventoryContainerState* AmmoCont = DataHolder->FindContainerStateByTag(AmmoContainerTag);

	const bool bRelevant =
		(EquipCont  && ContainerId == EquipCont->ContainerId)  ||
		(UtilCont   && ContainerId == UtilCont->ContainerId)   || 
		(AmmoCont   && ContainerId == AmmoCont->ContainerId)   ||
		(WeaponCont && ContainerId == WeaponCont->ContainerId);

	if (bRelevant)
	{
		RefreshEquipmentSlots();
	}
}

UInventorySlot* USinEquipmentPanel::GetSlotWidgetByLocation(const FGuid& ContainerId, int32 SlotIndex) const
{
	if (!DataHolder) return nullptr;

	const FSinInventoryContainerState* EquipCont = DataHolder->FindContainerStateByTag(EquipmentContainerTag);
	const FSinInventoryContainerState* UtilCont  = DataHolder->FindContainerStateByTag(UtilityContainerTag);

	if (EquipCont && ContainerId == EquipCont->ContainerId)
	{
		switch (SlotIndex)
		{
		case 0: return HeadSlot;
		case 1: return ChestSlot;
		case 2: return HandsSlot;
		case 3: return LegsSlot;
		case 4: return FeetSlot;
		case 5: return AmuletSlot;
		case 6: return RingSlot1;
		case 7: return RingSlot2;
		default: return nullptr;
		}
	}

	if (UtilCont && ContainerId == UtilCont->ContainerId)
	{
		switch (SlotIndex)
		{
		case 0: return UtilitySlot1;
		case 1: return UtilitySlot2;
		case 2: return UtilitySlot3;
		default: return nullptr;
		}
	}

	if (const FSinInventoryContainerState* WeaponCont = DataHolder->FindContainerStateByTag(WeaponContainerTag))
	{
		if (ContainerId == WeaponCont->ContainerId)
		{
			switch (SlotIndex)
			{
			case 0: return WeaponSlot0;
			case 1: return WeaponSlot1;
			case 2: return WeaponSlot2;
			case 3: return WeaponSlot3;
			default: return nullptr;
			}
		}
	}
	
	if (const FSinInventoryContainerState* AmmoCont = DataHolder->FindContainerStateByTag(AmmoContainerTag))
	{
		if (ContainerId == AmmoCont->ContainerId)
		{
			switch (SlotIndex)
			{
			case 0: return AmmoSlot1;
			case 1: return AmmoSlot2;
			case 2: return AmmoSlot3;
			default: return nullptr;
			}
		}
	}
	
	return nullptr;
}

void USinEquipmentPanel::RefreshSlotIfRelevant(const FGuid& ContainerId, int32 SlotIndex)
{
	if (UInventorySlot* InvSlot = GetSlotWidgetByLocation(ContainerId, SlotIndex))
	{
		InvSlot->RefreshSlotByLocation(ContainerId, SlotIndex);
	}
}

void USinEquipmentPanel::RefreshEntryLocation(const FSinInventoryEntry& Entry)
{
	// Assumes your FSinInventoryEntry has ContainerId + SlotIndex
	RefreshSlotIfRelevant(Entry.ContainerId, Entry.SlotIndex);
}

bool USinEquipmentPanel::IsRelevantEntry(const FSinInventoryEntry& Entry) const
{
	if (!DataHolder)
	{
		return false;
	}

	const FSinInventoryContainerState* Equip  = DataHolder->FindContainerStateByTag(EquipmentContainerTag);
	const FSinInventoryContainerState* Util   = DataHolder->FindContainerStateByTag(UtilityContainerTag);
	const FSinInventoryContainerState* Weapon = DataHolder->FindContainerStateByTag(WeaponContainerTag);
	const FSinInventoryContainerState* Ammo = DataHolder->FindContainerStateByTag(AmmoContainerTag);

	return (Equip  && Entry.ContainerId == Equip->ContainerId)  ||
		   (Util   && Entry.ContainerId == Util->ContainerId)   ||
		   	(Ammo   && Entry.ContainerId == Ammo->ContainerId)   ||
		   (Weapon && Entry.ContainerId == Weapon->ContainerId);
}

void USinEquipmentPanel::ClearEquipmentData()
{
	if (DataHolder)
	{
		// Container-level
		DataHolder->OnContainerChanged.RemoveDynamic(this, &USinEquipmentPanel::HandleEquipmentContainerChanged);

		// Entry-level (matching the exact signatures we fixed)
		DataHolder->OnInventoryEntryAdded.RemoveDynamic(this, &USinEquipmentPanel::HandleInventoryEntryAdded);
		DataHolder->OnInventoryEntryRemoved.RemoveDynamic(this, &USinEquipmentPanel::HandleInventoryEntryRemoved);
		DataHolder->OnInventoryEntryChanged.RemoveDynamic(this, &USinEquipmentPanel::HandleInventoryEntryChanged);
	}

	DataHolder = nullptr;
	OwningActor = nullptr;
}

void USinEquipmentPanel::NativeDestruct()
{
	ClearEquipmentData();
	Super::NativeDestruct();
}
