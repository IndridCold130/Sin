// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/HUD/SinHUD_Indicators.h"
#include "Interfaces/SinPlayerInterface.h"
#include "Inventory/Equipment.h"
#include "Misc/SinGPTs.h"
#include "UE5Coro.h"
//#include "Widgets/SinButtonCluster.h"
#include "Components/Border.h"
#include "Engine/AssetManager.h"

#include "SinHUD_Widget.generated.h"

class UCanvasPanel;
class UBorder;
class UInventoryPanel;
class UEquipmentPanel;
class USinBelt;
class USinToolTip_Interact;
class UMerchantWindow;
class USinCarousel;

USTRUCT(BlueprintType)
struct FMenuAsset : public FTableRowBase
{
	GENERATED_BODY()

	/** Unique Menu Identifier (Tag-Based) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag MenuTag;

	/** Soft reference to the menu widget class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<USinMenuBase> MenuClass;
};

UCLASS()
class SIN_API UMenuAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	/** Returns the global Asset Manager instance */
	static UMenuAssetManager& Get();

	/** Loads UI menu assets */
	UE5Coro::TCoroutine<> LoadMenu(FName MenuName, FStreamableDelegate OnLoadedCallback, bool bAsync = true);
};

/**
 * 
 */
UCLASS()
class SIN_API USinHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus")
	TMap<FGameplayTag, FName> MenuTagMap =
	{
		{ TAG_Menu_Escape, TEXT("Menu_Escape") },
		{ TAG_Menu_Loot, TEXT("WBP_LootContainer") },
		{ TAG_Menu_Quickloot, TEXT("WBP_Quickloot") },
		{ TAG_Menu_Level, TEXT("WBP_LevelMenu") },
		{ TAG_Menu_Carousel, TEXT("Menu_Carousel") },
		{ TAG_Menu_Carousel_Inventory, TEXT("Menu_Carousel") },
		{ TAG_Menu_Carousel_Spellbook, TEXT("Menu_Carousel") }
	};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UWidgetSwitcher> HUD_Switcher;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UCanvasPanel> HUDCanvas;

		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UBorder> SpecialBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UBorder> EquipmentBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UBorder> InteractionBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<USinToolTip_Interact> ToolTip;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<USinHUD_Indicators> StatIndicators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UMerchantWindow> MerchantClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Classes")
		TSubclassOf<UUserWidget> CreationMirrorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<USinToolTip_Interact> InterToolTipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<USinHUD_Indicators> IndicatorsClass;

		TObjectPtr<UInventory> PlayerInventory;
	
		TObjectPtr <UEquipment> PlayerEquipment;

		TObjectPtr<USinBelt> MainArmaments;

		TObjectPtr<USinBelt> SecondaryArmaments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UInventoryPanel> PlayerInventoryClass;

	UPROPERTY()
		UInventoryPanel* InventoryPanel;


	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void TogglePlayerInventory(UInventory* Container);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Carousel")
		void LoadSinMenu(ASinPlayerController* SinPlayer, FGameplayTag SubmenuTag, AActor* Interactable, bool bAsync);
		virtual void LoadSinMenu_Implementation(ASinPlayerController* SinPlayer, FGameplayTag SubmenuTag, AActor* Interactable, bool bAsync = false);
	
	UFUNCTION(BlueprintPure, Category="Sin|Menu")
		bool IsMenuLoaded(FGameplayTag MenuTag, bool bRequireActive = true) const;

		UFUNCTION(Category = "Carousel")
		void OnMenuLoaded(ASinPlayerController* SinPlayer, FName Menu, AActor* Interactable = nullptr, FGameplayTag MenuTag = TAG_Menu_Carousel_Character);
		UFUNCTION(BlueprintCallable, Category = "Carousel")
		void CloseMenu(ASinPlayerController* SinPlayer);
		UFUNCTION(BlueprintCallable)
		void OnFocusCleared();

		void ToggleToolTip(bool Show, FString Info = TEXT("nil"));

	UFUNCTION(BlueprintCallable)
		void ToggleInteractableMenu(EInteractionMenu Menu, UInventory* Container);

	UPROPERTY(BlueprintReadWrite, Category = "Sin HUD")
		FGameplayTag ActiveMenu;
		UPROPERTY(BlueprintReadWrite, Category = "Sin HUD")
		FName ActiveMenuName;

protected:

	void NativeOnInitialized() override;

	void NativeDestruct() override;

	bool bLoadingMenu = false;
};

UCLASS()
class SIN_API USinMenuBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// Constructor
	USinMenuBase(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USinHUD> MainHUD;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ASinPlayerController> SinController;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InteractionTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DesiredWidgetIndex = 1;
	UFUNCTION()
	USinHUD* GetMainHUD();
	UFUNCTION(BlueprintNativeEvent)
	void InitializeSinMenu(ASinPlayerController* Player, AActor* Interactable, USinHUD* HUD);
	virtual void InitializeSinMenu_Implementation(ASinPlayerController* Player, AActor* Interactable, USinHUD* HUD);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputMode PreferredInputMode;
	UFUNCTION(BlueprintNativeEvent, Category = "Submenu")
	void SwapSubmenu(FGameplayTag Tag);
	virtual void SwapSubmenu_Implementation(FGameplayTag Tag);
protected:
	// virtual void NativeOnInitialized() override;
};
