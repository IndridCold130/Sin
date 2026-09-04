// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Widgets/SinItemContextMenu.h"
#include "Widgets/InventoryPanel.h"
#include "Widgets/InventorySlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/MenuAnchor.h"
#include "Inventory/Widgets/SinItemContextMenuButton.h"
#include "Components/VerticalBox.h"

#include "Inventory/Inventory.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_Equipment.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Misc/SinCommonLibrary.h"
#include "SinGameHUD.h"

void USinItemContextMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void USinItemContextMenu::InitContextMenu(UInventoryPanel* InOwnerPanel, UInventorySlot* InSourceSlot)
{
	SetIsFocusable(true);
	SetKeyboardFocus();
	SetFocus();

	OwnerPanel = InOwnerPanel;
	SourceSlot = InSourceSlot;

	if (!SourceSlot)
	{
		return;
	}

	SourceSlot->EndTooltipHover();

	SourceInventory = SourceSlot->GetOwningInventory();
	if (!SourceInventory)
	{
		return;
	}

	Entry = SourceInventory->FindEntryById(SourceSlot->EntryId);
	if (!Entry || !Entry->ItemDefinition)
	{
		return;
	}

	ItemDefinition = Entry->ItemDefinition;
	BuildDefaultButtons();
}

FReply USinItemContextMenu::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (SourceSlot)
		{
			SourceSlot->ContextAnchor->Close();
		}

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply USinItemContextMenu::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (OwnerPanel)
	{
		OwnerPanel->HideItemContextMenu();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void USinItemContextMenu::AddContextButton(const FText& Label, ESinItemContextAction Action)
{
	USinItemContextMenuButton* Button =
	CreateWidget<USinItemContextMenuButton>(GetOwningPlayer(), ContextMenuButtonClass);

	if (!Button)
	{
		return;
	}

	Button->InitContextButton(Label, Action);

	Button->OnClickedAction.AddDynamic(
		this,
		&USinItemContextMenu::HandleContextButtonClicked
	);

	ButtonBox->AddChild(Button);
}

void USinItemContextMenu::HandleContextButtonClicked(ESinItemContextAction Action)
{
	UInventory* Inventory = SourceInventory;
	if (!Inventory && SourceSlot)
	{
		Inventory = SourceSlot->GetOwningInventory();
	}

	FGuid EntryId;
	if (SourceSlot && SourceSlot->EntryId.IsValid())
	{
		EntryId = SourceSlot->EntryId;
	}
	else if (Entry)
	{
		EntryId = Entry->EntryId;
	}

	TSoftObjectPtr<USoundBase> PickupSound;
	TSoftObjectPtr<USoundBase> DropSound;
	if (SourceSlot)
	{
		SourceSlot->GetItemSounds(PickupSound, DropSound);
	}

	bool bDidSomething = false;

	switch (Action)
	{
	case ESinItemContextAction::Split:
		if (ASinGameHUD* SinHUD = USinCommonLibrary::GetSinGameHUD(this))
		{
			SinHUD->ShowSplitStackDialog(this);
		}
		break;

	case ESinItemContextAction::Examine:
		if (ASinGameHUD* SinHUD = USinCommonLibrary::GetSinGameHUD(this))
		{
			SinHUD->ShowItemExamineDialog(this);
		}
		break;

	case ESinItemContextAction::Equip:
		if (Inventory && EntryId.IsValid())
		{
			bDidSomething = Inventory->EquipEntryToBestSlot(EntryId);
		}
		break;

	case ESinItemContextAction::Unequip:
		if (Inventory && EntryId.IsValid())
		{
			bDidSomething = Inventory->MoveEntryToBestContainer(EntryId);
		}
		break;

	case ESinItemContextAction::Drop:
	case ESinItemContextAction::Close:
	default:
		break;
	}

	if (Action == ESinItemContextAction::Equip
		|| Action == ESinItemContextAction::Unequip)
	{
		if (bDidSomething)
		{
			if (!DropSound.IsNull())
			{
				USinCommonLibrary::PlaySoftSound2D(this, DropSound);
			}
		}
		else if (!ActionFailedSound.IsNull())
		{
			USinCommonLibrary::PlaySoftSound2D(this, ActionFailedSound);
		}
	}
	else if (!DefaultClickSound.IsNull())
	{
		USinCommonLibrary::PlaySoftSound2D(this, DefaultClickSound);
	}

	if (SourceSlot && SourceSlot->ContextAnchor)
	{
		SourceSlot->ContextAnchor->Close();
	}
}

bool USinItemContextMenu::IsEntryInSpecializedContainer() const
{
	if (!SourceInventory || !Entry) {return false;}
	
	const FSinInventoryContainerState* Container = SourceInventory->FindContainerStateById(Entry->ContainerId); if(!Container){return false;}
	return !Container->bAllowAutoAdd;
}

bool USinItemContextMenu::CanSmartEquipEntry() const
{
	if (!ItemDefinition){return false;}
	if (ItemDefinition->FindFragmentByClass<USinItemFragment_Equipment>()) {return true;}
	
	return ItemDefinition->ItemTags.HasTag(TAG_Item_Equipment_Utility)
		|| ItemDefinition->ItemTags.HasTag(TAG_Item_Equipment_Ammo)
		|| ItemDefinition->ItemTags.HasTag(TAG_Item_Equipment_Weapon);	
}

void USinItemContextMenu::BuildDefaultButtons()
{
	if (!ButtonBox || !ContextMenuButtonClass || !Entry || !ItemDefinition){return;} ButtonBox->ClearChildren();
	
	if (Entry->StackCount > 1){AddContextButton(FText::FromString(TEXT("Split")), ESinItemContextAction::Split);}
	
	const bool bIsInSpecializedContainer = IsEntryInSpecializedContainer();
	
	if (bIsInSpecializedContainer)
	{
		AddContextButton(FText::FromString(TEXT("Unequip")), ESinItemContextAction::Unequip);
	}
	else if (CanSmartEquipEntry())
	{
		AddContextButton(FText::FromString(TEXT("Equip")), ESinItemContextAction::Equip);
	}
	
	AddContextButton(FText::FromString(TEXT("Examine")), ESinItemContextAction::Examine);
	AddContextButton(FText::FromString(TEXT("Drop")), ESinItemContextAction::Drop);
	AddContextButton(FText::FromString(TEXT("Close")), ESinItemContextAction::Close);
}
