// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UniformGridPanel.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/SinHUD_Widget.h"
#include "Misc/SinGPTs.h"
#include "InventoryPanel.generated.h"

class UInventory;
class UInventorySlot;

/**
 * 
 */
UCLASS()
class SIN_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:

		UPROPERTY(BlueprintReadOnly)
			TObjectPtr<USinHUD> MasterHUD;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
			bool bItemList = false;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Design, meta = (ExposeOnSpawn = "true"))
			int32 InventoryDivider = 8;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Design)
			FMargin SlotPadding;

		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
			TObjectPtr<UUniformGridPanel> SlotGrid;

		UPROPERTY(BlueprintReadOnly)
			TObjectPtr<UInventory>DataHolder;

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			TSubclassOf<UInventorySlot> InventorySlotClass;

		UPROPERTY(BlueprintReadOnly)
			UInventorySlot* CachedSlot;

		UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void CacheLastHoveredSlot(UInventorySlot* HoveredSlot);

		UFUNCTION(BlueprintPure, Category = "Inventory")
		bool HasSpace(FGameplayTag InventoryTag);

		TMap<FGameplayTag, TArray<UInventorySlot*>> InvSlotData;
		TArray<UInventorySlot* > SlotData;

		UFUNCTION(BlueprintPure, Category = "Inventory")
			virtual bool HasItemInInventorySlot(int32 SlotIndex, FGameplayTag SlotType);

		UFUNCTION(BlueprintPure, Category = "Inventory")
			virtual bool ToggleToolTip(int32 SlotIndex, FGameplayTag SlotType);

		UFUNCTION(BlueprintCallable, Category = "Inventory")
			virtual void MoveItemClient(UInventoryPanel* Panel, int32 OldIndex, int32 DstIndex, FGameplayTag IncomingSlotType, FGameplayTag LocalSlotType);

		UFUNCTION(BlueprintCallable, Category = "Inventory")
			void ItemAdded(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory);

		UFUNCTION(BlueprintCallable, Category = "Inventory")
			void ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory);

		UFUNCTION()
			bool OnDoubleClickedSlot(FGameplayTag SlotType, int32 SlotIndex);
		UFUNCTION()
			bool MoveItemOnDoubleClick(ASinPlayerController* Controller, UGameItemBase* Item, UInventory* NewInventory, int32 SlotIndex);

		UFUNCTION()
			void AttemptSplitStack(FGameplayTag SlotType, int32 SlotIndex, int32 Amount= -1);
	
		virtual void NativePreConstruct() override;
		virtual void NativeOnInitialized() override;
		virtual void NativeConstruct() override;
		virtual void NativeDestruct() override;

		UFUNCTION(BlueprintCallable, Category = "Inventory")
			void SetInventoryData(AActor* Owner);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ExposeOnSpawn = "true"))
	FGameplayTag InventoryType = TAG_Item;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ManageInventorySlots(int32 Slots, bool bPreview = false);
	UInventorySlot* FindSlotWidgetByInventoryIndex(int32 InventoryIndex) const;
	void GetWidgetsOfClassUnderParent(TSubclassOf<UInventorySlot> WidgetClass, TArray<UInventorySlot*>& FoundWidgets);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TSoftObjectPtr<UTexture2D> DefaultPreviewIcon;
};



/**
 *
 */
UCLASS()
class SIN_API UMerchantWindow : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventory* MerchantInventory;
};

/**
 *
 */
UCLASS()
class SIN_API UPlayerInventoryView : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
	void RecalculateWeight(UInventory* Holder, float& BackpackWeight, float& EquipLoad);
	virtual void RecalculateWeight_Implementation(UInventory* Holder, float& BackpackWeight, float& EquipLoad);
};
	
