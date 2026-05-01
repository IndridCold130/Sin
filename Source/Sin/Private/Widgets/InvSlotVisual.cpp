// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InvSlotVisual.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"

void UInvSlotVisual::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInvSlotVisual::SetItemIconImage_Implementation(TSoftObjectPtr<UTexture2D> SoftImg)
{
	Icon->SetBrushFromSoftTexture(SoftImg);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInvSlotVisual::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//Icon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Icon"));
}
