#include "Inventory/Widgets/InventoryContainerButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Widgets/InventoryPanel.h"
#include "Widgets/InventorySlot.h"
#include "Inventory/Widgets/InventoryDragDropOperation.h"
#include "Inventory/SinInventoryEntryTypes.h"

void UInventoryContainerButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Container)
	{
		Button_Container->OnClicked.AddDynamic(
			this,
			&UInventoryContainerButton::HandleButtonClicked
		);
	}
}

void UInventoryContainerButton::InitContainerButton(
	UInventoryPanel* InOwnerPanel,
	const FGuid& InContainerId,
	FGameplayTag InContainerTag,
	const FText& InDisplayName)
{
	OwnerPanel = InOwnerPanel;
	ContainerId = InContainerId;
	ContainerTag = InContainerTag;
	DisplayName = InDisplayName;

	if (Text_ContainerName)
	{
		Text_ContainerName->SetText(DisplayName);
	}
}

void UInventoryContainerButton::HandleButtonClicked()
{
	if (OwnerPanel && ContainerId.IsValid())
	{
		OwnerPanel->SetDisplayedContainer(ContainerId);
	}
}

void UInventoryContainerButton::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	if (Button_Container)
	{
		Button_Container->SetIsEnabled(!bSelected);
	}

	BP_OnSelectedChanged(bSelected);
}

bool UInventoryContainerButton::CanAcceptDragOperation(UDragDropOperation* Operation) const
{
	const UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(Operation);

	if (!DragOp || !OwnerPanel || !OwnerPanel->DataHolder)
	{
		return false;
	}

	if (!DragOp->EntryId.IsValid() || !ContainerId.IsValid())
	{
		return false;
	}

	const int32 EntryIndex = OwnerPanel->DataHolder->FindEntryIndexById(DragOp->EntryId);
	if (EntryIndex == INDEX_NONE)
	{
		return false;
	}

	const FSinInventoryEntry& Entry = OwnerPanel->DataHolder->ItemInventory[EntryIndex];

	return OwnerPanel->DataHolder->DoesContainerAcceptItem(ContainerId, Entry.ItemDefinition)
		&& !OwnerPanel->DataHolder->IsContainerFull(ContainerId);
}

void UInventoryContainerButton::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                                  UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryContainerButton::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

bool UInventoryContainerButton::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation);
	if (!DragOp || !DragOp->SourceSlot || !OwnerPanel || !OwnerPanel->DataHolder)
	{
		return false;
	}

	UInventory* TargetInventory = OwnerPanel->DataHolder;
	UInventory* SourceInventory = DragOp->SourceSlot->MasterPanel
		? DragOp->SourceSlot->MasterPanel->DataHolder
		: nullptr;

	if (!SourceInventory || !TargetInventory || !DragOp->EntryId.IsValid() || !ContainerId.IsValid())
	{
		return false;
	}

	if (SourceInventory == TargetInventory)
	{
		return TargetInventory->MoveEntryTo(DragOp->EntryId, ContainerId, INDEX_NONE);
	}

	return SourceInventory->TransferEntryToInventory(
		DragOp->EntryId,
		TargetInventory,
		ContainerId,
		INDEX_NONE
	);
}
