// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/SinItemExamineDialog.h"

#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Inventory/Widgets/SinItemContextMenu.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_Display.h"
#include "Inventory/Items/SinItemFragment_Inventory.h"
#include "Misc/SinCommonLibrary.h"

void USinItemDialogue::InitFromContextMenu(USinItemContextMenu* ContextMenu)
{
	CachedDisplayFragment =
	ContextMenu->ItemDefinition->FindFragmentByClass<USinItemFragment_Display>();
	if (!CachedDisplayFragment)
	{
		return;
	}
	Image_ItemIcon->SetBrushFromSoftTexture(CachedDisplayFragment->Icon);
	RichText_Title->SetText(USinCommonLibrary::MakeRichText(TitleStyleName,CachedDisplayFragment->DisplayName));
}

void USinItemDialogue::NativeConstruct()
{
	Super::NativeConstruct(); SetIsFocusable(true);
}

FReply USinItemDialogue::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{RemoveFromParent(); return FReply::Handled();}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply USinItemDialogue::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{RemoveFromParent(); return FReply::Handled();}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void USinItemExamineDialog::InitFromContextMenu(USinItemContextMenu* ContextMenu)
{
	Super::InitFromContextMenu(ContextMenu);
	
	RichText_Description->SetText(USinCommonLibrary::MakeRichTextParagraphs(DescriptionStyleName,CachedDisplayFragment->LongDescriptionParagraphs));
}
