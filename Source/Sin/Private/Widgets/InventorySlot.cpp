// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventorySlot.h"

#include "Input/DragAndDrop.h"
#include "Blueprint/DragDropOperation.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/Items/SinItemFragment_Display.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "SinGameHUD.h"
#include "Inventory/Widgets/InventoryDragDropOperation.h"
#include "Components/Image.h"
#include "Widgets/InvSlotVisual.h"
#include "Components/MenuAnchor.h"
#include "Inventory/Widgets/SinItemContextMenu.h"
#include "Misc/SinCommonLibrary.h"
#include "Inventory/Widgets/SinEquipmentPanel.h"
#include "Misc/SinCommonLibrary.h"
#include "SinGameHUD.h"


void UInventorySlot::NativeOnInitialized()
{
}

void UInventorySlot::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyVisualSettings(SlotSize, IconSize);
}

FReply UInventorySlot::NativeOnMouseButtonDoubleClick(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	if (!EntryId.IsValid())
	{
		return FReply::Unhandled();
	}

	UInventory* Inventory = GetOwningInventory();
	if (!Inventory)
	{
		return FReply::Unhandled();
	}

	// New inventory system path.
	if (UsesNewInventorySystem())
	{
		TSoftObjectPtr<USoundBase> PickupSound;
		TSoftObjectPtr<USoundBase> DropSound;
		GetItemSounds(PickupSound, DropSound);
		
		const bool bResult = Inventory->TryDoubleClickEntry(EntryId);
		if (bResult)
		{
			if (!DropSound.IsNull())
			{
				USinCommonLibrary::PlaySoftSound2D(this, DropSound);
			}
			OnHovered(false);
		}
		return bResult ? FReply::Handled() : FReply::Unhandled();
	}

	// Old system path.
	if (!MasterPanel)
	{
		return FReply::Unhandled();
	}

	if (!MasterPanel->HasItemInInventorySlot(SlotIndex, SlotType))
	{
		return FReply::Unhandled();
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!MasterPanel->OnDoubleClickedSlot(SlotType, SlotIndex))
	{
		SetVisibility(ESlateVisibility::Visible);
	}

	return FReply::Handled();
}

FReply UInventorySlot::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (MasterPanel && MasterPanel->bUseNewInventorySystem)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// Shift + RMB = quick split
			if (InMouseEvent.IsShiftDown())
			{
				UInventory* Inventory = MasterPanel->DataHolder;
				if (!Inventory || !EntryId.IsValid() || !ContainerId.IsValid())
				{
					return FReply::Handled();
				}

				const FSinInventoryEntry* Entry = Inventory->FindEntryById(EntryId);
				if (!Entry || Entry->StackCount <= 1)
				{
					return FReply::Handled();
				}

				int32 FreeSlot = INDEX_NONE;
				if (!Inventory->FindFirstFreeSlotV2(ContainerId, FreeSlot))
				{
					return FReply::Handled();
				}

				const int32 SplitAmount = Entry->StackCount / 2;
				
				const bool bSplitSucceeded = Inventory->SplitEntryStackToSlot(
					EntryId,
					SplitAmount,
					ContainerId,
					FreeSlot
				);
				
				if (bSplitSucceeded)
				{
					TSoftObjectPtr<USoundBase> PickupSound;
					TSoftObjectPtr<USoundBase> DropSound;

					if (GetItemSounds(PickupSound, DropSound) && !DropSound.IsNull())
					{
						USinCommonLibrary::PlaySoftSound2D(this, DropSound);
					}
				}

				return FReply::Handled();
			}

			// Regular RMB = context menu
			if (EntryId.IsValid())
			{
				//MasterPanel->TryShowItemContextMenuFromSlot(this);
				ContextAnchor->Open(true);
				return FReply::Handled();
			}
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlot::NativeOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent,UDragDropOperation*& Operation)
{
	UInventory* Inventory = GetOwningInventory();

	if (!Inventory || !SlotImg)
	{
		return;
	}

	const bool bUseNewSystem = UsesNewInventorySystem();

	const bool bHasDraggableItem = bUseNewSystem
		? EntryId.IsValid()
		: (MasterPanel && MasterPanel->HasItemInInventorySlot(SlotIndex, SlotType));

	if (!bHasDraggableItem)
	{
		return;
	}

	UInventoryDragDropOperation* Drag = NewObject<UInventoryDragDropOperation>();
	if (!Drag)
	{
		return;
	}

	Drag->Payload = this;
	Drag->SourceSlot = this;
	Drag->EntryId = EntryId;
	Drag->SourceContainerId = ContainerId;
	Drag->bWantsStackSplit = PointerEvent.IsShiftDown();
	Drag->Pivot = EDragPivot::CenterCenter;

	if (VisualClass)
	{
		UInvSlotVisual* DragVisual =
			CreateWidget<UInvSlotVisual>(GetOwningPlayer(), VisualClass);

		if (DragVisual && SlotImg && SlotImg->Icon)
		{
			if (DragVisual->Icon)
			{
				if (USizeBox* SizeBox = Cast<USizeBox>(DragVisual->Icon->GetParent()))
				{
					SizeBox->SetWidthOverride(DragVisualSizeOverride.X);
					SizeBox->SetHeightOverride(DragVisualSizeOverride.Y);
					SizeBox->SetMaxDesiredWidth(DragVisualSizeOverride.X);
					SizeBox->SetMaxDesiredHeight(DragVisualSizeOverride.Y);
				}

				DragVisual->Icon->SetBrush(SlotImg->Icon->GetBrush());
				DragVisual->Icon->SetDesiredSizeOverride(
					SlotImg->Icon->GetBrush().ImageSize
				);
			}

			DragVisual->SetVisibility(ESlateVisibility::Visible);
			DragVisual->SetRenderOpacity(1.0f);

			Drag->DefaultDragVisual = DragVisual;
		}
	}

	SetDraggingVisualState(true);

	Operation = Drag;
	TSoftObjectPtr<USoundBase> PickupSound = nullptr;
	TSoftObjectPtr<USoundBase> DropSound = nullptr;
	GetItemSounds(PickupSound, DropSound);
	if (!PickupSound.IsNull())
	{
		Drag->PickupSound = PickupSound;
		Drag->DropSound   = DropSound;
		USinCommonLibrary::PlaySoftSound2D(this,PickupSound);
	}
}

void UInventorySlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SetDraggingVisualState(false);
}

void UInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UInventorySlot::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	EndTooltipHover();
	
}

bool UInventorySlot::GetItemSounds( TSoftObjectPtr<USoundBase>& OutPickup,  TSoftObjectPtr<USoundBase>& OutDrop) const
{
	OutPickup = nullptr;
	OutDrop = nullptr;
	UInventory* Inventory = GetOwningInventory();
	if (!Inventory || !EntryId.IsValid()){return false;}
	
	const FSinInventoryEntry* Entry = Inventory->FindEntryById(EntryId);
	if (!Entry || !Entry->ItemDefinition) {return false;}
	
	const USinItemFragment_Display* Display =
		Entry->ItemDefinition->FindFragmentByClass<USinItemFragment_Display>();
	
	if (!Display){return false;}
	
	OutPickup = Display->PickupSound; OutDrop   = Display->DropSound; return true;
}

bool UInventorySlot::HandleInventoryDrop(UInventoryDragDropOperation* Operation)
{
	if (!Operation || !Operation->SourceSlot){return false;}

	UInventorySlot* DraggedSlot = Operation->SourceSlot;

	if (DraggedSlot == this){DraggedSlot->SetDraggingVisualState(false);return false;}

	UInventory* TargetInventory = GetOwningInventory();
	UInventory* SourceInventory = DraggedSlot->GetOwningInventory();

	if (!TargetInventory || !SourceInventory){DraggedSlot->SetDraggingVisualState(false);return false;}

	if (!UsesNewInventorySystem() || !DraggedSlot->UsesNewInventorySystem())
	{
		DraggedSlot->SetDraggingVisualState(false);
		return false;
	}

	bool bResult = false;
	TSoftObjectPtr<USoundBase> PickupSound = nullptr;
	TSoftObjectPtr<USoundBase> DropSound = nullptr;

	if (Operation->bWantsStackSplit)
	{
		const FSinInventoryEntry* SourceEntry =
		SourceInventory->FindEntryById(Operation->EntryId);

		if (!SourceEntry || SourceEntry->StackCount <= 1)
		{
			DraggedSlot->SetDraggingVisualState(false);
			return false;
		}

		// ----- NEW: validate the target slot first -----
		const bool bTargetAcceptsItem = TargetInventory->DoesContainerAcceptItemAtSlot(
			ContainerId,
			SlotIndex,
			SourceEntry->ItemDefinition
		);

		if (!bTargetAcceptsItem)
		{
			DraggedSlot->SetDraggingVisualState(false);
			return false;		// do NOT open the dialog
		}

		// Optional extra: if the slot is occupied, only allow if it can stack
		const int32 TargetEntryIndex = TargetInventory->FindEntryIndexAtSlot(ContainerId, SlotIndex);
		if (TargetEntryIndex != INDEX_NONE)
		{
			const FSinInventoryEntry& TargetEntry = TargetInventory->ItemInventory[TargetEntryIndex];

			if (!TargetInventory->CanStackEntries(*SourceEntry, TargetEntry))
			{
				DraggedSlot->SetDraggingVisualState(false);
				return false;	// occupied by something that can’t stack
			}
		}
		// -----------------------------------------------

		if (ASinGameHUD* HUD = USinCommonLibrary::GetSinGameHUD(this))
		{
			HUD->ShowSplitStackDialogForSlotDrop(
				SourceInventory,
				TargetInventory,
				Operation->EntryId,
				ContainerId,
				SlotIndex
			);
		}

		DraggedSlot->SetDraggingVisualState(false);
		return true;
	}

	if (SourceInventory == TargetInventory)
	{
		bResult = TargetInventory->MoveEntryTo(
			Operation->EntryId,
			ContainerId,
			SlotIndex
		);
	}
	else
	{
		bResult = SourceInventory->TransferEntryToInventory(
			Operation->EntryId,
			TargetInventory,
			ContainerId,
			SlotIndex
		);
	}

	DraggedSlot->SetDraggingVisualState(false);
	if (bResult)
	{
		if (Operation->DropSound)
		{
			USinCommonLibrary::PlaySoftSound2D(this,Operation->DropSound);
		}
	}
	return bResult;
}

bool UInventorySlot::CanAcceptDraggedSlot(UDragDropOperation* Operation) const
{
	if (!Operation)
	{
		return false;
	}

	UInventoryDragDropOperation* DragOp =
		Cast<UInventoryDragDropOperation>(Operation);

	UInventorySlot* DraggedSlot = nullptr;

	if (DragOp && DragOp->SourceSlot)
	{
		DraggedSlot = DragOp->SourceSlot.Get();
	}
	else
	{
		DraggedSlot = Cast<UInventorySlot>(Operation->Payload);
	}

	if (!DraggedSlot || DraggedSlot == this)
	{
		return false;
	}

	UInventory* TargetInventory = GetOwningInventory();
	UInventory* SourceInventory = DraggedSlot->GetOwningInventory();

	if (!TargetInventory || !SourceInventory)
	{
		return false;
	}

	if (!DraggedSlot->EntryId.IsValid() || !ContainerId.IsValid())
	{
		return false;
	}

	if (UsesNewInventorySystem() && DraggedSlot->UsesNewInventorySystem())
	{
		const int32 DraggedEntryIndex =
			SourceInventory->FindEntryIndexById(DraggedSlot->EntryId);

		if (DraggedEntryIndex == INDEX_NONE)
		{
			return false;
		}

		const FSinInventoryEntry& DraggedEntry =
			SourceInventory->ItemInventory[DraggedEntryIndex];

		// Target slot-specific validation.
		if (!TargetInventory->DoesContainerAcceptItemAtSlot(
			ContainerId,
			SlotIndex,
			DraggedEntry.ItemDefinition))
		{
			return false;
		}

		const int32 TargetEntryIndex =
			TargetInventory->FindEntryIndexAtSlot(ContainerId, SlotIndex);

		// Empty target slot.
		if (TargetEntryIndex == INDEX_NONE)
		{
			return true;
		}

		const FSinInventoryEntry& TargetEntry =
			TargetInventory->ItemInventory[TargetEntryIndex];

		// Same item stack merge.
		if (TargetInventory->CanStackEntries(DraggedEntry, TargetEntry))
		{
			const int32 MaxStackSize =
				TargetInventory->GetMaxStackSize(DraggedEntry.ItemDefinition);

			if (TargetEntry.StackCount < MaxStackSize)
			{
				return true;
			}
		}

		// Swap validation:
		// target item must be accepted by the dragged item's original slot/container.
		return SourceInventory->DoesContainerAcceptItemAtSlot(
			DraggedEntry.ContainerId,
			DraggedEntry.SlotIndex,
			TargetEntry.ItemDefinition
		);
	}

	// Old inventory system path only works for normal inventory panel slots.
	if (!MasterPanel || !DraggedSlot->MasterPanel || !MasterPanel->DataHolder)
	{
		return false;
	}

	if (!DraggedSlot->SlottedItem)
	{
		return false;
	}

	if (!SlottedItem)
	{
		return MasterPanel->DataHolder->SlotRules(
			DraggedSlot->SlottedItem,
			LegacySlotType
		);
	}

	return MasterPanel->DataHolder->SlotRules(
		DraggedSlot->SlottedItem,
		LegacySlotType
	)
	&& MasterPanel->DataHolder->SlotRules(
		SlottedItem,
		DraggedSlot->LegacySlotType
	);
}

void UInventorySlot::ToggleItemTooltip(bool bShow)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	ASinGameHUD* HUD = Cast<ASinGameHUD>(PC->GetHUD());
	if (!HUD)
	{
		return;
	}

	// New system: works for normal inventory slots and equipment slots.
	if (UsesNewInventorySystem())
	{
		USinItemDefinition* ItemDefinition = GetTooltipItemDefinition();

		if (!ItemDefinition)
		{
			HUD->ShowTooltipV2(false, this, nullptr);
			return;
		}

		HUD->ShowTooltipV2(bShow, this, ItemDefinition);
		return;
	}

	// Old system only applies to MasterPanel-based slots.
	if (!MasterPanel)
	{
		HUD->ShowTooltip(false, this, nullptr);
		return;
	}

	if (!SlottedItem)
	{
		HUD->ShowTooltip(false, this, nullptr);
		return;
	}

	HUD->ShowTooltip(bShow, this, SlottedItem);
}

USinItemDefinition* UInventorySlot::GetTooltipItemDefinition() const
{
	UInventory* Inventory = GetOwningInventory();
	if (!Inventory || !EntryId.IsValid()){return nullptr;}

	const FSinInventoryEntry* Entry = Inventory->FindEntryById(EntryId); if (!Entry){return nullptr;}

	return Entry->ItemDefinition;
}

void UInventorySlot::BeginTooltipHover()
{
	if (!GetWorld()){return;}

	GetWorld()->GetTimerManager().ClearTimer(TooltipDelayHandle);

	GetWorld()->GetTimerManager().SetTimer(
		TooltipDelayHandle,
		this,
		&UInventorySlot::ShowTooltipDelayed,
		TooltipDelay,
		false
	);
}

void UInventorySlot::EndTooltipHover()
{
	if (!GetWorld()){return;}

	GetWorld()->GetTimerManager().ClearTimer(TooltipDelayHandle);

	ToggleItemTooltip(false);
}

void UInventorySlot::ShowTooltipDelayed()
{
	if (IsHovered())
	{
		ToggleItemTooltip(true);
	}
}

void UInventorySlot::RefreshSlotV2_Implementation(const FSinInventoryEntry& Entry)
{
	if (!SlotImg || !Entry.ItemDefinition)
	{
		ClearSlotV2();
		return;
	}

	const USinItemFragment_Display* Display =
		Entry.ItemDefinition->FindFragmentByClass<USinItemFragment_Display>();

	if (!Display || Display->Icon.IsNull())
	{
		ClearSlotV2();
		return;
	}

	UTexture2D* IconTexture = Display->Icon.LoadSynchronous();
	SlotImg->SetItemIconImage_Implementation(IconTexture);
	SlotImg->SetVisibility(ESlateVisibility::HitTestInvisible);
	EntryId = Entry.EntryId;
}

void UInventorySlot::SetDraggingVisualState(bool bDragging)
{
	if (SlotImg)
	{
		SlotImg->SetRenderOpacity(bDragging ? 0.35f : 1.0f);
	}

	BP_SetDraggingVisualState(bDragging);
}


void UInventorySlot::ClearSlotV2_Implementation()
{
	if (SlotImg)
	{
		SlotImg->SetVisibility(ESlateVisibility::Hidden);
	}
	EntryId.Invalidate();
}

TSoftObjectPtr<UTexture2D> UInventorySlot::GetDefaultPreviewIcon()
{
	if (PreviewIcons.Contains(SlotType))
	{
		return PreviewIcons[SlotType];
		//TSoftObjectPtr<UTexture2D> SoftTexture = PreviewIcons[SlotType];
	}
	return nullptr;
}

void UInventorySlot::ApplyVisualSettings(FVector2D InSlotSize, FVector2D InIconSize)
{
	if (SlotSizeBox)
	{
		SlotSizeBox->SetWidthOverride(InSlotSize.X);
		SlotSizeBox->SetHeightOverride(InSlotSize.Y);
	}

	if (IconSizeBox)
	{
		IconSizeBox->SetWidthOverride(InIconSize.X);
		IconSizeBox->SetHeightOverride(InIconSize.Y);
	}
	if (SlotImg)
	{
		SlotImg->IconBox->SetWidthOverride(InIconSize.X);
		SlotImg->IconBox->SetHeightOverride(InIconSize.Y);
	}
}

void UInventorySlot::RefreshSlot_Implementation(UGameItemBase* Item)
{
	SlottedItem = Item;

	if (!SlotImg)
	{
		return;
	}

	if (Item)
	{
		SlotImg->SetItemIconImage_Implementation(Item->GetIcon());
		SlotImg->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SlotImg->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventorySlot::SetPreviewIcon_Implementation(bool bShow)
{
}

void UInventorySlot::OnHovered_Implementation(bool Hover)
{
	if (IsAnyContextMenuOpen())
	{
		if (!Hover)
		{
			EndTooltipHover();
		}

		return;
	}
	Hover ? BeginTooltipHover() : EndTooltipHover();
}

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();
	ContextAnchor->OnMenuOpenChanged.AddDynamic(this,&UInventorySlot::HandleContextMenuOpenChanged);
	ContextAnchor->OnGetUserMenuContentEvent.BindDynamic(this,&UInventorySlot::CreateContextMenuWidget);
}

void UInventorySlot::HandleContextMenuOpenChanged(bool bIsOpen)
{
	if (bIsOpen)
	{
		EndTooltipHover();
	}
	else
	{
		if (IsHovered()){BeginTooltipHover();}
		//if (Hovered){BeginTooltipHover();}
		// optional: allow tooltip again; usually no action needed
	}
}

UUserWidget* UInventorySlot::CreateContextMenuWidget()
{
	if (!ContextMenuClass)
	{
		return nullptr;
	}

	USinItemContextMenu* Menu =
		CreateWidget<USinItemContextMenu>(GetOwningPlayer(), ContextMenuClass);

	if (Menu)
	{
		Menu->InitContextMenu(MasterPanel, this);
	}

	return Menu;
}

void UInventorySlot::InitFixedContainerSlot(UUserWidget* InOwningPanel,const FGuid& InContainerId,int32 InSlotIndex)
{
	if (!InOwningPanel){return;}
	
	MasterPanel = Cast<UInventoryPanel>(InOwningPanel);
	EquipmentPanel = Cast<USinEquipmentPanel>(InOwningPanel);
	//QuickslotPanel = Cast<USinQuickslotPanel>(InOwningPanel);

	ContainerId = InContainerId; SlotIndex = InSlotIndex;

	EntryId.Invalidate();

	ClearSlotV2();
}

void UInventorySlot::RefreshSlotByLocation(const FGuid& InContainerId, int32 InSlotIndex)
{
	ContainerId = InContainerId; SlotIndex = InSlotIndex; UInventory* InventoryRef = nullptr;
	
	if (MasterPanel) {InventoryRef = MasterPanel->DataHolder;}
	else if (EquipmentPanel){InventoryRef = EquipmentPanel->DataHolder;}
	if (!InventoryRef){ClearSlotV2();return;}
	const int32 EntryIndex = InventoryRef->FindEntryIndexAtSlot(ContainerId,SlotIndex);
	if (EntryIndex == INDEX_NONE)
	{EntryId.Invalidate();ClearSlotV2();return;}
	const FSinInventoryEntry& Entry = InventoryRef->ItemInventory[EntryIndex];
	RefreshSlotV2(Entry);
	//SetSlotData(Entry);
}

UInventory* UInventorySlot::GetOwningInventory() const
{
	if (MasterPanel){return MasterPanel->DataHolder;}

	if (EquipmentPanel){return EquipmentPanel->DataHolder;} return nullptr;
}

bool UInventorySlot::UsesNewInventorySystem() const
{
	if (MasterPanel){return MasterPanel->bUseNewInventorySystem;}

	// Equipment panel is only for the new system.
	if (EquipmentPanel){return true;} return false;
}

bool UInventorySlot::IsAnyContextMenuOpen() const
{
	if (MasterPanel && MasterPanel->ActiveContextMenu){return true;}
	if (EquipmentPanel)
	{
		// If equipment panel does not own context menus directly,
		// check whatever shared owner/panel/HUD you use here.
		return false;
	}
	return false;
}
