#include "Inventory/Widgets/SinEquipmentPanel.h"
#include "Inventory/Inventory.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Widgets/InventorySlot.h"

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
}

void USinEquipmentPanel::InitializeEquipmentSlots()
{
	if (!DataHolder)
	{
		return;
	}

	const FSinInventoryContainerState* EquipmentContainer =
		DataHolder->FindContainerStateByTag(EquipmentContainerTag);

	if (!EquipmentContainer)
	{
		return;
	}

	const FGuid EquipmentContainerId = EquipmentContainer->ContainerId;

	HeadSlot->InitEquipmentSlot(this, EquipmentContainerId, 0);
	ChestSlot->InitEquipmentSlot(this, EquipmentContainerId, 1);
	RingSlot1->InitEquipmentSlot(this, EquipmentContainerId, 6);
	RingSlot2->InitEquipmentSlot(this, EquipmentContainerId, 7);
}

void USinEquipmentPanel::RefreshEquipmentSlots()
{
	const FSinInventoryContainerState* EquipmentContainer =
		DataHolder->FindContainerStateByTag(EquipmentContainerTag);

	if (!EquipmentContainer){return;}
	const FGuid EquipmentContainerId = EquipmentContainer->ContainerId;
	
	if (HeadSlot)
	{HeadSlot->RefreshSlotByLocation(EquipmentContainerId, 0);}

	if (ChestSlot)
	{ChestSlot->RefreshSlotByLocation(EquipmentContainerId, 1);}

	if (HandsSlot)
	{HandsSlot->RefreshSlotByLocation(EquipmentContainerId, 2);}

	if (LegsSlot)
	{LegsSlot->RefreshSlotByLocation(EquipmentContainerId, 3);}

	if (FeetSlot)
	{FeetSlot->RefreshSlotByLocation(EquipmentContainerId, 4);}

	if (AmuletSlot)
	{AmuletSlot->RefreshSlotByLocation(EquipmentContainerId, 5);}

	if (RingSlot1)
	{RingSlot1->RefreshSlotByLocation(EquipmentContainerId, 6);}

	if (RingSlot2)
	{RingSlot2->RefreshSlotByLocation(EquipmentContainerId, 7);}
}

void USinEquipmentPanel::HandleEquipmentContainerChanged(UInventory* Inventory, FGuid ContainerId)
{
	if (!DataHolder || Inventory != DataHolder){return;}
	const FSinInventoryContainerState* EquipmentContainer = DataHolder->FindContainerStateByTag(EquipmentContainerTag);
	if (!EquipmentContainer){return;}
	if (ContainerId != EquipmentContainer->ContainerId){return;}
	RefreshEquipmentSlots();
}
