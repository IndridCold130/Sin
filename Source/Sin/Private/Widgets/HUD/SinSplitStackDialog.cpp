// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/SinSplitStackDialog.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Widgets/SinItemContextMenu.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_Inventory.h"
#include "Components/Slider.h"
#include "Components/RichTextBlock.h"
#include "Inventory/Inventory.h"
#include "Misc/SinCommonLibrary.h"
#include "Components/Image.h"
#include "Inventory/Items/SinItemFragment_Display.h"
#include "Inventory/Items/SinItemDefinition.h"

void USinSplitStackDialog::InitFromContextMenu(USinItemContextMenu* ContextMenu)
{
	Super::InitFromContextMenu(ContextMenu);
	CachedInventoryFragment = ContextMenu->ItemDefinition->FindFragmentByClass<USinItemFragment_Inventory>();
	ItemEntry = ContextMenu->Entry;
	TargetContainerIdForSplit = ItemEntry->ContainerId;
	TargetSlotIndexForSplit = -1;

	MaxStack = CachedInventoryFragment->MaxStackSize; CurrentStack = ItemEntry->StackCount; DeduceAmount = FMath::Max(1, CurrentStack / 2);

	if (Slider_Amount)
	{
		Slider_Amount->OnValueChanged.AddDynamic(
			this,
			&USinSplitStackDialog::HandleSliderChanged
		);

		//Slider_Amount->SetMinValue(1.0f);
		Slider_Amount->SetMaxValue(CurrentStack - 1);
		Slider_Amount->SetValue(DeduceAmount);
	}
	SetSplitSource(ContextMenu->SourceInventory,ContextMenu->Entry->EntryId,ContextMenu->Entry->ContainerId);
	RefreshAmountText();
}

// WHATTT
void USinSplitStackDialog::InitFromDrag(UInventory* Source, UInventory* Target, const FGuid& IncomingSourceEntryId,
	const FGuid& TargetContainerId, int32 TargetSlotIndex)
{
	if (!Source || !IncomingSourceEntryId.IsValid()){return;}
	
	SourceInventory = Source; TargetInventory = Target; SourceEntryIdCached = IncomingSourceEntryId; SourceContainerId = FGuid(); 
	TargetContainerIdCached = TargetContainerId; TargetSlotIndexCached = TargetSlotIndex;
	ItemEntry = Source->FindEntryById(IncomingSourceEntryId);
	if (!ItemEntry || !ItemEntry->ItemDefinition) {return;}
	
	ItemDefinition = ItemEntry->ItemDefinition;
	CachedDisplayFragment = ItemDefinition->FindFragmentByClass<USinItemFragment_Display>();
	
	if (CachedDisplayFragment)
	{
		if (Image_ItemIcon)
		{
			Image_ItemIcon->SetBrushFromSoftTexture(CachedDisplayFragment->Icon);
		}

		if (RichText_Title)
		{
			RichText_Title->SetText(
				USinCommonLibrary::MakeRichText(
					TitleStyleName,
					CachedDisplayFragment->DisplayName
				)
			);
		}
	}
	
	CachedInventoryFragment = ItemDefinition->FindFragmentByClass<USinItemFragment_Inventory>(); if (!CachedInventoryFragment){return;}
	
	SourceContainerId = ItemEntry->ContainerId;
	
	MaxStack = CachedInventoryFragment->MaxStackSize;
	CurrentStack = ItemEntry->StackCount;
	DeduceAmount = FMath::Max(1, CurrentStack / 2);
	
	if (Slider_Amount)
	{
		Slider_Amount->OnValueChanged.RemoveDynamic(
			this,
			&USinSplitStackDialog::HandleSliderChanged
		);

		Slider_Amount->OnValueChanged.AddDynamic(
			this,
			&USinSplitStackDialog::HandleSliderChanged
		);
		
		Slider_Amount->SetMaxValue(CurrentStack - 1);
		Slider_Amount->SetValue(DeduceAmount);
	}

	SetSplitSource(Source,IncomingSourceEntryId,ItemEntry->ContainerId);

	TargetContainerIdForSplit = TargetContainerId;
	TargetSlotIndexForSplit = TargetSlotIndex;

	RefreshAmountText();
}
// WHATT

void USinSplitStackDialog::HandleSliderChanged(float Value)
{
	DeduceAmount = FMath::Max(Value, 1);
	RichText_DeducedAmount->SetText(USinCommonLibrary::MakeRichText(MaxStackStyleName,FText::AsNumber(DeduceAmount)));
	//RichText_DeducedAmount->SetText(FText::AsNumber(DeduceAmount));
}

void USinSplitStackDialog::HandleValueChanged(float Value)
{
	DeduceAmount = FMath::Clamp(DeduceAmount+Value,1,CurrentStack-1);
	RefreshAmountText();
}

void USinSplitStackDialog::RefreshAmountText()
{
	RichText_DeducedAmount->SetText(USinCommonLibrary::MakeRichText(MaxStackStyleName,FText::AsNumber(DeduceAmount)));
	RichText_Amount->SetText(USinCommonLibrary::MakeRichText(MaxStackStyleName,FText::AsNumber(CurrentStack)));
	//RichText_Amount->SetText(FText::AsNumber(CurrentStack));
}

void USinSplitStackDialog::SetSplitSource(UInventory* InInventory, const FGuid& InEntryId, const FGuid& InContainerId)
{
	SourceInventory = InInventory;
	SourceEntryId = InEntryId;
	SourceContainerId = InContainerId;
}

void USinSplitStackDialog::SplitStack()
{
	if (!SourceInventory || !SourceEntryId.IsValid())
	{
		return;
	}

	bool bSplitSucceeded;

	// Same inventory
	if (!TargetInventory || TargetInventory == SourceInventory)
	{
		bSplitSucceeded = SourceInventory->SplitEntryStackToSlot(
			SourceEntryId,
			DeduceAmount,
			TargetContainerIdForSplit,
			TargetSlotIndexForSplit
		);
	}
	// Cross-inventory (player ↔ chest, etc.)
	else
	{
		bSplitSucceeded = SourceInventory->SplitEntryStackToOtherInventory(
			SourceEntryId,
			DeduceAmount,
			TargetInventory,
			TargetContainerIdForSplit,
			TargetSlotIndexForSplit
		);
	}

	if (bSplitSucceeded)
	{
		const USinItemFragment_Display* Display = CachedDisplayFragment;

		if (!Display && ItemDefinition)
		{
			Display = ItemDefinition->FindFragmentByClass<USinItemFragment_Display>();
		}

		if (Display && !Display->DropSound.IsNull())
		{
			USinCommonLibrary::PlaySoftSound2D(this, Display->DropSound);
		}
	}
	RemoveFromParent();
}
