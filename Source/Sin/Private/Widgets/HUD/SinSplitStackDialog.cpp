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

		Slider_Amount->SetMinValue(1.0f);
		Slider_Amount->SetMaxValue(CurrentStack - 1);
		Slider_Amount->SetValue(DeduceAmount);
	}
	SetSplitSource(ContextMenu->SourceInventory,ContextMenu->Entry->EntryId,ContextMenu->Entry->ContainerId);
	RefreshAmountText();
}

// WHATTT
void USinSplitStackDialog::InitFromDrag(UInventory* Inventory, const FGuid& IncomingSourceEntryId,
	const FGuid& TargetContainerId, int32 TargetSlotIndex)
{
	if (!Inventory || !IncomingSourceEntryId.IsValid()){return;}
	
	SourceInventory = Inventory; SourceEntryIdCached = IncomingSourceEntryId; SourceContainerId = FGuid(); 
	TargetContainerIdCached = TargetContainerId; TargetSlotIndexCached = TargetSlotIndex;
	ItemEntry = Inventory->FindEntryById(IncomingSourceEntryId);
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
		
		Slider_Amount->SetMinValue(1.0f);
		Slider_Amount->SetMaxValue(CurrentStack - 1);
		Slider_Amount->SetValue(DeduceAmount);
	}

	SetSplitSource(Inventory,IncomingSourceEntryId,ItemEntry->ContainerId);

	TargetContainerIdForSplit = TargetContainerId;
	TargetSlotIndexForSplit = TargetSlotIndex;

	RefreshAmountText();
}
// WHATT

void USinSplitStackDialog::HandleSliderChanged(float Value)
{
	DeduceAmount = Value;
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

	SourceInventory->SplitEntryStackToSlot(
		SourceEntryId,
		DeduceAmount,
		TargetContainerIdForSplit,
		TargetSlotIndexForSplit
	);

	RemoveFromParent();
}
