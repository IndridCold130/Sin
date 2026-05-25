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
	SourceInventory = OwnerPanel->DataHolder; if (!SourceInventory){return;}
	SourceSlot->EndTooltipHover();
	
	Entry = SourceInventory->FindEntryById(SourceSlot->EntryId);
	if (!Entry || !Entry->ItemDefinition){return;}
	
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

	default:
		break;
	}
	if (SourceSlot){SourceSlot->ContextAnchor->Close();}
}

void USinItemContextMenu::BuildDefaultButtons()
{
	if (!ButtonBox || !ContextMenuButtonClass){return;} ButtonBox->ClearChildren();
	
	if (Entry->StackCount > 1)
	{
		AddContextButton(FText::FromString(TEXT("Split")), ESinItemContextAction::Split);
	}
	
	if (ItemDefinition->FindFragmentByClass<USinItemFragment_Equipment>())
	{
		AddContextButton(FText::FromString(TEXT("Equip")), ESinItemContextAction::Equip);
	}
	AddContextButton(FText::FromString(TEXT("Examine")), ESinItemContextAction::Examine);
	AddContextButton(FText::FromString(TEXT("Drop")), ESinItemContextAction::Drop);
	AddContextButton(FText::FromString(TEXT("Close")), ESinItemContextAction::Close);
}
