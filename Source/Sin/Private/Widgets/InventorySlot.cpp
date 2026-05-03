// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventorySlot.h"
#include "Input/DragAndDrop.h"
#include "Blueprint/DragDropOperation.h"


void UInventorySlot::NativeOnInitialized()
{
}

FReply UInventorySlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!MasterPanel->HasItemInInventorySlot(SlotIndex, SlotType))
		{
			return FReply::Unhandled();
		}
		SetVisibility(ESlateVisibility::HitTestInvisible);
		bool Pending = MasterPanel->OnDoubleClickedSlot(SlotType, SlotIndex);
		if (!Pending)
		{
			SetVisibility(ESlateVisibility::Visible);
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
			return Reply;
		}
		return Reply;
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		MasterPanel->AttemptSplitStack(SlotType, SlotIndex);
	}
	return Reply;
}

void UInventorySlot::NativeOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{
	if (MasterPanel->HasItemInInventorySlot(SlotIndex, SlotType))
	{
		if (SlotImg)
		{
			UDragDropOperation* Drag = NewObject<UDragDropOperation>();
			Drag->Payload = this;
			//SlotImg->SetClipping(EClippin);
			Drag->DefaultDragVisual = SlotImg;
			Drag->Pivot = EDragPivot::CenterCenter;
			Operation = Drag;
		}
	}
}

void UInventorySlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
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
}

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();
}