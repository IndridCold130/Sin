// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SinHUD_Widget.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "SinPlayerController.h"
#include "Widgets/SinToolTip_Interact.h"
#include "Widgets/EquipmentPanel.h"
#include "UE5Coro/LatentAwaiters.h"

void USinHUD::NativeOnInitialized()
{
	InventoryPanel = CreateWidget<UInventoryPanel>(this, PlayerInventoryClass);
	InventoryPanel->MasterHUD = this;
	AActor* OwnerPawn = GetOwningPlayerPawn();
	if (OwnerPawn)
	{
		PlayerEquipment = Cast<UEquipment>(OwnerPawn->GetComponentByClass(UEquipment::StaticClass()));
		PlayerInventory = Cast<USinInventory>(OwnerPawn->GetComponentByClass(USinInventory::StaticClass()));
		TArray<UActorComponent*> Quickbars = (OwnerPawn->GetComponentsByTag(USinBelt::StaticClass(), "Quickbar"));
		for (auto Component = Quickbars.CreateConstIterator(); Component; ++Component)
		{
			USinBelt* LocalBelt = Cast<USinBelt>(*Component);
			if (LocalBelt->ComponentHasTag("MainHand"))
			{
				MainArmaments = LocalBelt;
			}
			if (LocalBelt->ComponentHasTag("OffHand"))
			{
				SecondaryArmaments = LocalBelt;
			}
		}
	}
}

void USinHUD::NativeDestruct()
{
	InventoryPanel->RemoveFromRoot();
	Super::NativeDestruct();
}

void USinHUD::TogglePlayerInventory(UInventory* Container)
{
	if (!EquipmentBorder->HasAnyChildren() && !SpecialBorder->HasAnyChildren())
	{
		EquipmentBorder->AddChild(InventoryPanel);
		ISinPlayerInterface* Interface = Cast<ISinPlayerInterface>(GetOwningPlayer());
			if (Interface)
			{
				Interface->Execute_SinInputMode(GetOwningPlayer(), this, EInputMode::VE_GameAndUI);
			}
	}
	else
	{
		EquipmentBorder->RemoveChildAt(0);
		InteractionBorder->RemoveChildAt(0);
		SpecialBorder->RemoveChildAt(0);
		HUD_Switcher->SetActiveWidgetIndex(0);
		ISinPlayerInterface* Interface = Cast<ISinPlayerInterface>(GetOwningPlayer());
		if (Interface)
		{
			Interface->Execute_SinInputMode(GetOwningPlayer(), this, EInputMode::VE_GameOnly);
		}
	}
}

void USinHUD::LoadSinMenu_Implementation(ASinPlayerController* SinPlayer, FGameplayTag MenuTag, AActor* Interactable, bool bAsync)
{
	if (!MenuTagMap.Contains(MenuTag)||bLoadingMenu==true) { return; }
	if (ActiveMenuName == MenuTagMap[MenuTag])
	{
		if (ActiveMenu == MenuTag || ActiveMenu !=TAG_Menu_Escape&&MenuTag == TAG_Menu_Carousel_Inventory) { CloseMenu(SinPlayer); return; }
		else 
		{
			USinMenuBase* Menu = Cast<USinMenuBase>(SpecialBorder->GetChildAt(0));
			if (Menu)
			{
				ActiveMenu = MenuTag;
				Menu->SwapSubmenu(MenuTag);
			}
		}
	}
	ActiveMenu = MenuTag;
	ActiveMenuName = MenuTagMap[MenuTag];
	bLoadingMenu = true;
	//  FIRST: Check if the asset is already loaded
	FPrimaryAssetId AssetId(FPrimaryAssetType(TEXT("UI")), ActiveMenuName);
	UObject* LoadedObject = UAssetManager::Get().GetPrimaryAssetObject(AssetId);
	if (LoadedObject)
	{
		//  If already loaded, instantly create the menu
		if (UClass* MenuClass = Cast<UClass>(LoadedObject))
		{
			OnMenuLoaded(SinPlayer, ActiveMenuName, Interactable, MenuTag);
			return; // Exit early to avoid unnecessary loading
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Loaded object is not a valid menu class: %s"), *ActiveMenuName.ToString());
		}
	}
	UMenuAssetManager::Get().
		LoadMenu(ActiveMenuName, FStreamableDelegate::CreateUObject(this, &USinHUD::OnMenuLoaded, SinPlayer, ActiveMenuName, Interactable, MenuTag), bAsync);
}

bool USinHUD::IsMenuLoaded(FGameplayTag MenuTag, bool bRequireActive) const
{
	if (!MenuTag.IsValid())
	{
		return false;
	}

	if (bLoadingMenu)
	{
		return false;
	}

	if (!MenuTagMap.Contains(MenuTag))
	{
		return false;
	}

	if (bRequireActive && ActiveMenu != MenuTag)
	{
		return false;
	}

	if (!SpecialBorder || !SpecialBorder->HasAnyChildren())
	{
		if (!InteractionBorder || !InteractionBorder->HasAnyChildren())
		{
			return false;
		}
	}

	const FName ExpectedMenuName = MenuTagMap[MenuTag];

	if (ActiveMenuName != ExpectedMenuName)
	{
		return false;
	}

	const bool bInSpecialBorder =
	SpecialBorder &&
	SpecialBorder->HasAnyChildren() &&
	Cast<USinMenuBase>(SpecialBorder->GetChildAt(0));

	const bool bInInteractionBorder =
		InteractionBorder &&
		InteractionBorder->HasAnyChildren() &&
		Cast<USinMenuBase>(InteractionBorder->GetChildAt(0));

	return bInSpecialBorder || bInInteractionBorder;
}

void USinHUD::OnMenuLoaded(ASinPlayerController* SinPlayer, FName Menu, AActor* Interactable, FGameplayTag MenuTag)
{
	FPrimaryAssetId AssetId(FPrimaryAssetType(TEXT("UI")), Menu);
	UObject* LoadedObject = UAssetManager::Get().GetPrimaryAssetObject(AssetId);
	if (UClass* MenuClass = Cast<UClass>(LoadedObject))
	{
		USinMenuBase* NewMenu = CreateWidget<USinMenuBase>(this, MenuClass);
		if (NewMenu)
		{
			NewMenu->InitializeSinMenu(SinPlayer, Interactable, this);
			//if (Menu != FName(TEXT("WBP_Quickloot")))
		}
	}
	bLoadingMenu = false;
}

void USinHUD::CloseMenu(ASinPlayerController* SinPlayer)
{
	if (!SinPlayer) { return; }
	if (SpecialBorder->HasAnyChildren())
	{
		ActiveMenu = FGameplayTag::EmptyTag;
		HUD_Switcher->SetActiveWidgetIndex(0);
		SpecialBorder->ClearChildren();
		SinPlayer->SinInputMode(this, EInputMode::VE_GameOnly);
		if (!ActiveMenuName.IsNone())
		{
			FPrimaryAssetId AssetId(FPrimaryAssetType(TEXT("UI")), ActiveMenuName);
			UAssetManager::Get().UnloadPrimaryAsset(AssetId);
			ActiveMenuName = NAME_None;
			UE_LOG(LogTemp, Log, TEXT("Unloaded Menu: %s"), *ActiveMenuName.ToString());
			bLoadingMenu = false;
		}
	}
	else
	{
		ActiveMenu = TAG_Menu_Escape;
		ActiveMenuName = MenuTagMap[TAG_Menu_Escape];
		bLoadingMenu = true;
		AActor* Empty = nullptr;
		UMenuAssetManager::Get().LoadMenu(ActiveMenuName, FStreamableDelegate::CreateUObject(this, &USinHUD::OnMenuLoaded, SinPlayer, ActiveMenuName, Empty, ActiveMenu));
	}
}

void USinHUD::OnFocusCleared()
{
	if (InteractionBorder->HasAnyChildren())
	{
		ActiveMenuName = NAME_None;
		InteractionBorder->ClearChildren();
	}
}

void USinHUD::ToggleToolTip(bool Show, FString Info)
{
	if (!Show)
	{
		ToolTip->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	ToolTip->GetSinToolTipInfo(Info);
	ToolTip->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USinHUD::ToggleInteractableMenu(EInteractionMenu Menu, UInventory* Container)
{
	TogglePlayerInventory(InventoryPanel->DataHolder);
	GetOwningPlayer()->SetInputMode(FInputModeGameAndUI());
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	switch (Menu)
	{
	case EInteractionMenu::Loot:
	{
	}
	break;
	case EInteractionMenu::Merchant:
	{
		UMerchantWindow* MerchantPanel = CreateWidget<UMerchantWindow>(this, MerchantClass);
		if (MerchantPanel)
		{
			MerchantPanel->MerchantInventory = Container;
			InteractionBorder->AddChild(MerchantPanel);
		}
	}
	break;
	case EInteractionMenu::Mirror:
	{
		UUserWidget* LocMirror = CreateWidget<UUserWidget>(this, CreationMirrorClass);
		if (LocMirror)
		{
			SpecialBorder->AddChild(LocMirror);
			HUD_Switcher->SetActiveWidgetIndex(1);
		}
	}
	break;
	case EInteractionMenu::Bonfire:
	{
	}
	}
}

UMenuAssetManager& UMenuAssetManager::Get()
{
	return *Cast<UMenuAssetManager>(GEngine->AssetManager);
}

UE5Coro::TCoroutine<> UMenuAssetManager::LoadMenu(FName MenuName, FStreamableDelegate OnLoadedCallback, bool bAsync)
{
	FPrimaryAssetId AssetId(FPrimaryAssetType(TEXT("UI")), MenuName);
	UE_LOG(LogTemp, Log, TEXT("Loading Menu Asset: %s"), *AssetId.ToString());
	UE_LOG(LogTemp, Log, TEXT("Trying to Load Asset: Primary Type = %s, Name = %s"),
		*FPrimaryAssetType(TEXT("UI")).ToString(), *MenuName.ToString());
	FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
	UE_LOG(LogTemp, Log, TEXT("Trying to Load Asset at Path: %s"), *AssetPath.ToString());
	if (bAsync)
	{
		co_await UE5Coro::Latent::AsyncLoadPrimaryAsset(AssetId);
		OnLoadedCallback.ExecuteIfBound();
	}
	else 
	{
		const TSubclassOf<USinMenuBase> SinMenuClass = Cast<UClass>(AssetPath.TryLoad());
		USinMenuBase* CDO = SinMenuClass.GetDefaultObject();
		if (CDO)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded menu synchronously: %s"), *MenuName.ToString());
			OnLoadedCallback.ExecuteIfBound();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load menu synchronously: %s"), *MenuName.ToString());
		}
	}
	co_return;  //  Ensure the function always returns a coroutine
}

USinMenuBase::USinMenuBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) // Always call Super
{
	 PreferredInputMode = EInputMode::VE_GameAndUI; // Default value
}

// ISUNMENUBASE CLASS
USinHUD* USinMenuBase::GetMainHUD()
{
	if (MainHUD) { return MainHUD; }
	else 
	{
		ASinPlayerController* Controller = Cast<ASinPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			return MainHUD = Controller->MainHUD;
		}
	}
	return nullptr;
}

void USinMenuBase::InitializeSinMenu_Implementation(ASinPlayerController* Player, AActor* Interactable, USinHUD* HUD)
{
	SinController = Player; MainHUD = HUD; InteractionTarget = Interactable;
	MainHUD->SpecialBorder->AddChild(this);
	MainHUD->HUD_Switcher->SetActiveWidgetIndex(DesiredWidgetIndex);
	Player->SinInputMode(this, PreferredInputMode);
}

void USinMenuBase::SwapSubmenu_Implementation(FGameplayTag Tag)
{
}
