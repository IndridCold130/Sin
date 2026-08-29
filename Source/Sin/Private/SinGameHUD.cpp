// Fill out your copyright notice in the Description page of Project Settings.


#include "SinGameHUD.h"

#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_Display.h"
#include "Widgets/SinItemDefinitionTooltip.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Components/Widget.h"
#include "Inventory/Widgets/SinItemContextMenu.h"
#include "Widgets/HUD/SinItemExamineDialog.h"
#include "Widgets/HUD/SinSplitStackDialog.h"

void ASinGameHUD::BeginPlay()
{
	Super::BeginPlay();
	SinController =Cast<ASinPlayerController>(GetOwningPlayerController());
	if (SinController)
	{
		SinController->OnClientPossessedDelegate.AddDynamic(this, &ASinGameHUD::GenerateHUD);
		ASinCharacter* LocalCharacter = Cast<ASinCharacter>(SinController->GetPawn());
		if (LocalCharacter)
		{
			GenerateHUD();
		}
		//SinController->OnClientPossessedDelegate.AddDynamic(this, &ASinGameHUD::GenerateHUD);
	}
}

void ASinGameHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASinGameHUD::GenerateHUD_Implementation()
{
	if (SinController->MainHUD)
	{
		SinController->MainHUD->RemoveFromParent();
		SinController->MainHUD = nullptr;
	}
	HUD = CreateWidget<USinHUD>(SinController, HUD_Subclass);
	if (HUD)
	{
		HUD->AddToViewport();
		SinController->MainHUD = HUD;
	}
}

USinItemTooltip* ASinGameHUD::GetOrCreateTooltip()
{
	if (!ActiveTooltip) // If tooltip doesn't exist, create it
	{
		UClass* LoadedClass = TooltipWidgetClass.LoadSynchronous();
		if (LoadedClass)
		{
			ActiveTooltip = CreateWidget<USinItemTooltip>(GetWorld(), LoadedClass);
			ActiveTooltip->AddToViewport(33);
		}
	}
	return ActiveTooltip;
}

void ASinGameHUD::ShowTooltip(bool bShow, UUserWidget* Master, UGameItemBase* GameItem)
{
	if (!GetOrCreateTooltip()) { return; }
	else {
		if (!bShow) {
			GetOrCreateTooltip()->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
		GetOrCreateTooltip()->ShowTooltip(Master, GameItem);
	}
}

void ASinGameHUD::ShowTooltipV2(
	bool bShow,
	UUserWidget* Master,
	USinItemDefinition* ItemDefinition)
{
	USinItemTooltip* Tooltip = GetOrCreateTooltip();
	if (!Tooltip)
	{
		return;
	}

	if (!bShow || !ItemDefinition)
	{
		Tooltip->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	USinItemDefinitionTooltip* ItemTooltip = Cast<USinItemDefinitionTooltip>(Tooltip);
	if (!ItemTooltip)
	{
		UE_LOG(LogTemp, Warning, TEXT("TooltipWidgetClass is not a USinItemDefinitionTooltip."));
		Tooltip->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	ItemTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemTooltip->ShowItemDefinitionTooltip(Master, ItemDefinition);
}

void ASinGameHUD::ShowItemExamineDialog(USinItemContextMenu* ContextMenu)
{
	if (!ContextMenu || !ItemExamineDialogClass)
	{
		return;
	}
	CloseActiveModalDialog();
	
	USinItemExamineDialog* Dialog =
		CreateWidget<USinItemExamineDialog>(GetOwningPlayerController(), ItemExamineDialogClass);
	if (!Dialog){return;}
	
	Dialog->InitFromContextMenu(ContextMenu);
	Dialog->AddToViewport(120); ActiveModalDialog = Dialog;
	Dialog->SetIsFocusable(true);
	Dialog->SetUserFocus(GetOwningPlayerController());
	Dialog->SetKeyboardFocus();
}

void ASinGameHUD::ShowSplitStackDialog(USinItemContextMenu* ContextMenu)
{
	if (!ContextMenu || !SplitStackDialogClass){return;} CloseActiveModalDialog();
	
	USinSplitStackDialog* Dialog = CreateWidget<USinSplitStackDialog>(GetOwningPlayerController(), SplitStackDialogClass);
	if (!Dialog){return;}
	
	Dialog->InitFromContextMenu(ContextMenu);
	Dialog->AddToViewport(120); ActiveModalDialog = Dialog;
	Dialog->SetIsFocusable(true);
	Dialog->SetUserFocus(GetOwningPlayerController());
	Dialog->SetKeyboardFocus();
}

void ASinGameHUD::CloseActiveModalDialog()
{
	if (ActiveModalDialog)
	{
		ActiveModalDialog->RemoveFromParent();
		ActiveModalDialog = nullptr;
	}
}

void ASinGameHUD::ShowSplitStackDialogForSlotDrop(UInventory* SourceInventory, UInventory* TargetInventory, const FGuid& SourceEntryId,
	const FGuid& TargetContainerId, int32 TargetSlotIndex)
{
	USinSplitStackDialog* Dialog = CreateWidget<USinSplitStackDialog>(GetOwningPlayerController(), SplitStackDialogClass);
	if (!Dialog){return;}
	Dialog->InitFromDrag(SourceInventory, TargetInventory, SourceEntryId,
	TargetContainerId, TargetSlotIndex);
	Dialog->AddToViewport(120); ActiveModalDialog = Dialog;
	Dialog->SetIsFocusable(true);
	Dialog->SetUserFocus(GetOwningPlayerController());
	Dialog->SetKeyboardFocus();
}

bool ASinGameHUD::HasActiveModalDialog() const
{
	return ActiveModalDialog && ActiveModalDialog->IsInViewport();
}

bool ASinGameHUD::TryCloseTopModalDialog()
{
	if (!HasActiveModalDialog())
	{
		return false;
	}

	CloseActiveModalDialog();
	return true;
}

void ASinGameHUD::CloseMenu()
{
	if (TryCloseTopModalDialog()) {return;}
	HUD->CloseMenu(Cast<ASinPlayerController>(GetOwningPlayerController()));
}

void USinItemTooltip::ShowTooltip_Implementation(UUserWidget* Master, UGameItemBase* GameItem)
{
}

void USinItemTooltip::NativeConstruct()
{
	Super::NativeConstruct();
}

void USinItemTooltip::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Collapsed);
}

FVector2D USinItemTooltip::CalculateTooltipPosition(UWidget* Anchor, FVector2D TooltipSize, float ToolTipPadding) const
{
	if (!Anchor)
	{
		return FVector2D(ToolTipPadding, ToolTipPadding);
	}

	FGeometry AnchorGeometry = Anchor->GetCachedGeometry();

	FVector2D PixelPosition;
	FVector2D ViewportPosition;

	USlateBlueprintLibrary::LocalToViewport(
		GetOwningPlayer(),
		AnchorGeometry,
		FVector2D::ZeroVector,
		PixelPosition,
		ViewportPosition
	);

	const FVector2D AnchorSize = AnchorGeometry.GetLocalSize();

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float X = ViewportPosition.X + AnchorSize.X + ToolTipPadding;

	if (X + TooltipSize.X + ToolTipPadding > ViewportSize.X)
	{
		X = ViewportPosition.X - TooltipSize.X - ToolTipPadding;
	}

	float Y = ViewportPosition.Y;

	Y = FMath::Clamp(
		Y,
		ToolTipPadding,
		ViewportSize.Y - TooltipSize.Y - ToolTipPadding
	);

	X = FMath::Clamp(
		X,
		ToolTipPadding,
		ViewportSize.X - TooltipSize.X - ToolTipPadding
	);

	return FVector2D(X, Y);
}
