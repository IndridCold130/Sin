// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InvSlotVisual.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"

void UInvSlotVisual::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInvSlotVisual::SetItemIconImage_Implementation(const TSoftObjectPtr<UTexture2D>& SoftImg)
{
	if (!Icon)
	{
		UE_LOG(LogTemp, Error, TEXT("DragVisual Icon widget is null. Check BindWidget name / Is Variable."));
		return;
	}

	if (SoftImg.IsNull())
	{
		Icon->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	Icon->SetVisibility(ESlateVisibility::Visible);
	Icon->SetRenderOpacity(1.0f);
	Icon->SetBrushFromSoftTexture(SoftImg, true);

	SetVisibility(ESlateVisibility::Visible);
	SetRenderOpacity(1.0f);
}

void UInvSlotVisual::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//Icon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Icon"));
}
