// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Widgets/SinItemContextMenuButton.h"
#include "Components/RichTextBlock.h"

void USinItemContextMenuButton::InitContextButton(FText InLabel, ESinItemContextAction InAction)
{
	Action = InAction;

	if (Text_Label)
	{
		Text_Label->SetText(InLabel);
	}
}

void USinItemContextMenuButton::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply USinItemContextMenuButton::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	HandleClicked(); return FReply::Handled();
}

void USinItemContextMenuButton::HandleClicked()
{
	OnClickedAction.Broadcast(Action);
}
