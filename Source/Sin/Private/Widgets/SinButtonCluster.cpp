// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SinButtonCluster.h"

#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Widgets/SinHUD_Widget.h"
#include "SinPlayerController.h"

void USinButtonCluster::NativeConstruct()
{
	Super::NativeConstruct();
	GenerateClusterButtons(ID_Table);
}

void USinButtonCluster::GenerateClusterButtons_Implementation(UDataTable* Table)
{
	if(Table)
	{
		IDs = Table->GetRowNames();
		ButtonPanel->ClearChildren();
		for (auto It = IDs.CreateConstIterator(); It; ++It)
		{
			USinButton* LocalButton;
			LocalButton = CreateWidget<USinButton>(this, ButtonClass);
			LocalButton->ButtonIndex = It.GetIndex();
			LocalButton->ButtonID = *It;
			ButtonPanel->AddChild(LocalButton);
		}
	}
}

void USinButtonCluster::MakeSelection_Implementation(USinButton* Selection)
{
	if(ActiveButton)
	{
		ActiveButton->LockSinButton(false);
	}
	if (Selection)
	{
		ActiveButton = Selection;
		ActiveButton->LockSinButton(true);
	}
	else
	{
		ActiveButton = nullptr;
	}
}

//========================================================
// CAROUSEL
//========================================================

void USinCarousel::NativeOnInitialized()
{
	CreateButtonContainer();
	if (ButtonClass.IsValid())
	{
		PopulateCarousel();
	}
}

void USinCarousel::NativeConstruct()
{
	Super::NativeConstruct();
	PickSubmenuByTag(GetMainHUD()->ActiveMenu);
}

void USinCarousel::CreateButtonContainer()
{
	// Check if the RootContainer is valid
	if (!RootContainer)
	{
		UE_LOG(LogTemp, Error, TEXT("USinCarousel: RootContainer is null! Make sure it's bound in the Blueprint."));
		return;
	}
	RootContainer->ClearChildren();
	if (bUseVerticalBox)
	{
		ButtonContainer = NewObject<UVerticalBox>(this);
	}
	else
	{
		ButtonContainer = NewObject<UHorizontalBox>(this);
	}
	if (!ButtonContainer)
	{
		UE_LOG(LogTemp, Error, TEXT("USinCarousel: Failed to create ButtonContainer!"));
		return;
	}
	RootContainer->AddChild(ButtonContainer);
	//UPanelSlot* PanelSlot = Cast<UPanelSlot>(SetHorizontalAlignment(ButtonContainer));
	USizeBox* FriendlyBox = Cast<USizeBox>(RootContainer);
	if (IsValid(FriendlyBox))
	{
		USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(ButtonContainer->Slot);
		if (SizeBoxSlot)
		{
			SizeBoxSlot->SetHorizontalAlignment(HorizontalAlignment);
			SizeBoxSlot->SetVerticalAlignment(VerticalAlignment);
		}
	}
}

void USinCarousel::AddButtonsToContainer()
{
	// Ensure the ButtonContainer and ButtonWidgetClass are valid
	if (!ButtonContainer || !ButtonClass.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("USinCarousel: ButtonContainer or ButtonWidgetClass is invalid!"));
		return;
	}
	// Clear previous button instances
	ButtonInstances.Empty();
	int32 MapIndex = 0;
	for (const FSinButtonData& ButtonData : ButtonDefinitions)
	{
		UClass* Button = ButtonClass.LoadSynchronous();
		if (!Button)
		{
			UE_LOG(LogTemp, Warning, TEXT("USinCarousel: Failed to load ButtonWidgetClass!"));
			continue;
		}
		USinButton* NewButton = CreateWidget<USinButton>(GetWorld(), Button);
		if (!NewButton)
		{
			UE_LOG(LogTemp, Warning, TEXT("USinCarousel: Failed to create button instance!"));
			continue;
		}
		NewButton->ButtonID = ButtonData.ButtonID;
		NewButton->ButtonTag = ButtonData.ButtonTag;
		NewButton->SetSinButtonLabel(ButtonData.ButtonLabel);
		NewButton->ButtonIndex = MapIndex;
		MapIndex++;
		if (bUseVerticalBox)
		{
			UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(ButtonContainer->AddChild(NewButton));
			if (VerticalSlot)
			{
				// Customize slot settings if needed (e.g., padding, alignment)
				VerticalSlot->SetPadding(FMargin(5.0f));
				VerticalSlot->SetHorizontalAlignment(HAlign_Center);
			}
		}
		else
		{
			UHorizontalBoxSlot* HorizontalSlot = Cast<UHorizontalBoxSlot>(ButtonContainer->AddChild(NewButton));
			if (HorizontalSlot)
			{
				// Customize slot settings if needed (e.g., padding, alignment)
				HorizontalSlot->SetPadding(FMargin(5.0f));
				HorizontalSlot->SetVerticalAlignment(VAlign_Center);
			}
		}
		// Store the button instance for later use
		ButtonInstances.Add(NewButton);
		// Bind the button's OnSinButtonClicked delegate to the carousel's HandleButtonClicked function
		BindButtonEvent(NewButton);
	}
}

void USinCarousel::PopulateCarousel(int32 AutoSelectIndex)
{
	if (!ButtonContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("USinCarousel: ButtonContainer is null, creating one."));
		CreateButtonContainer();
	}
	AddButtonsToContainer();
	if (ButtonInstances.IsValidIndex(AutoSelectIndex))
	{
		HandleButtonClicked(ButtonInstances[AutoSelectIndex]);
	}
}

void USinCarousel::BindButtonEvent(USinButton* Button)
{
	if (Button)
	{
		Button->OnSinButtonClicked.AddDynamic(this, &USinCarousel::HandleButtonClicked);
	}
}

void USinCarousel::HandleButtonClicked_Implementation(USinButton* ClickedButton)
{
	if (!ClickedButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("USinCarousel: HandleButtonClicked called with a null button."));
		return;
	}
	//UE_LOG(LogTemp, Log, TEXT("USinCarousel: Button clicked - Index: %d"), ClickedButton->GetButtonIndex());
	for (USinButton* Button : ButtonInstances)
	{
		if (Button == ClickedButton)
		{
			if (Button->GetVisibility() != ESlateVisibility::HitTestInvisible)
			{
				Button->LockSinButton(true); // Lock the clicked button (custom logic in USinButton)
				Content->ClearChildren();
				SubMenuActivate(true, ClickedButton);
				if (ActiveButton) { ActiveButton->LockSinButton(false); }
				ActiveButton = ClickedButton;
				if (MainHUD)
				{
					MainHUD->ActiveMenu = Button->ButtonTag;
				}
			}
			else { return; }
		}
		else
		{
			Button->LockSinButton(false); // Unlock all other buttons
			SubMenuActivate(false, ClickedButton);
		}
	}
}

void USinCarousel::SubMenuActivate_Implementation(bool bActivate, USinButton* Button)
{
}

TSoftClassPtr<UUserWidget> USinCarousel::PickSubmenuByTag(FGameplayTag Tag)
{
	MainHUD->SpecialBorder->AddChild(this);
	MainHUD->ActiveMenu = Tag;
	for (auto It = ButtonDefinitions.CreateConstIterator(); It; ++It)
	{
		if (It->ButtonTag == Tag) 
		{
			if (ButtonInstances.IsValidIndex(It.GetIndex())) {
				HandleButtonClicked(ButtonInstances[It.GetIndex()]);
				ASinPlayerController* Controller = Cast<ASinPlayerController>(GetOwningPlayer());
				Controller->SinInputMode(this, EInputMode::VE_GameAndUI);
				return It->SubmenuClass;
			}
		}
	}
	return nullptr;
}

void USinCarousel::RotateSubmenu(bool bRight)
{
	if (ButtonInstances.IsEmpty()) { return; }
	int32 CurrentIndex = ButtonInstances.Find(ActiveButton);
	bRight ? FMath::Max(CurrentIndex = (CurrentIndex+1)%ButtonInstances.Num(), 0) : 
		CurrentIndex =(CurrentIndex + ButtonInstances.Num() - 1) % ButtonInstances.Num();
	HandleButtonClicked(ButtonInstances[CurrentIndex]);
}

void USinCarousel::SwapSubmenu_Implementation(FGameplayTag Tag)
{
	PickSubmenuByTag(Tag);
}
