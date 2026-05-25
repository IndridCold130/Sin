// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/InventoryPanel.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Widgets/InvSlotVisual.h"
#include "Inventory/GameItemBase.h"
#include "InventorySlot.generated.h"

//class UBorder;
class USinItemDefinition;
class USinItemContextMenu;
class UMenuAnchor;
class USinEquipmentPanel;

/**
 * 
 */
UCLASS()
class SIN_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UInvSlotVisual* SlotImg;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
		TSubclassOf<UInvSlotVisual> VisualClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Drag Visual")
	FVector2D DragVisualSizeOverride = FVector2D(100.0f, 100.0f);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UBorder* IconBorder;
	
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		USizeBox* SlotSizeBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		USizeBox* IconSizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		int32 SlotIndex;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	UGameItemBase* SlottedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Hovered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		FGameplayTag SlotType = TAG_Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		TSoftObjectPtr<UTexture2D> PreviewIconOverride;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> PreviewIcons;
	UFUNCTION(BlueprintPure)
		TSoftObjectPtr<UTexture2D> GetDefaultPreviewIcon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		UInventoryPanel* MasterPanel;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Design")
		void ApplyVisualSettings(FVector2D InSlotSize, FVector2D InIconSize);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void RefreshSlot(UGameItemBase* Item);
		virtual void RefreshSlot_Implementation(UGameItemBase* Item);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void SetPreviewIcon(bool bShow);
		virtual void SetPreviewIcon_Implementation(bool bShow);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
		void OnHovered(bool Hover);
		virtual void OnHovered_Implementation(bool Hover);

		UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Transient, Category = "Animations", meta = (BindWidgetAnim))
			UWidgetAnimation* Compatible;

		UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Transient, Category = "Animations", meta = (BindWidgetAnim))
			UWidgetAnimation* Incompatible;

		virtual void NativeConstruct() override;
protected:
	void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	//virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	
	//NEW SYSTEMZ
public:
	
	UPROPERTY(BlueprintReadWrite, Category="Inventory|Old")
	int32 LegacySlotType = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Inventory|New")
	FGuid EntryId;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|DragDrop")
	bool HandleInventoryDrop(UInventoryDragDropOperation* Operation);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|DragDrop")
	bool CanAcceptDraggedSlot(UDragDropOperation* Operation) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory|New")
	void RefreshSlotV2(const FSinInventoryEntry& Entry);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory|New")
	void ClearSlotV2();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Tooltip")
	void ToggleItemTooltip(bool bShow);
	
	UFUNCTION(BlueprintPure, Category="Inventory|Tooltip")
	USinItemDefinition* GetTooltipItemDefinition() const;
	
	FTimerHandle TooltipDelayHandle;
	
	void BeginTooltipHover();
	void EndTooltipHover();
	void ShowTooltipDelayed();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|DragDrop")
	void SetDraggingVisualState(bool bDragging);

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory|DragDrop")
	void BP_SetDraggingVisualState(bool bDragging);
	
	UPROPERTY(BlueprintReadWrite, Category="Inventory|New")
	FGuid ContainerId;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UMenuAnchor* ContextAnchor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Context Menu")
	TSubclassOf<USinItemContextMenu> ContextMenuClass;
	
	UFUNCTION()
	void HandleContextMenuOpenChanged(bool bIsOpen);

	UFUNCTION()
	UUserWidget* CreateContextMenuWidget();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	TObjectPtr<USinEquipmentPanel> EquipmentPanel;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Equipment")
	void InitEquipmentSlot(
		USinEquipmentPanel* InEquipmentPanel,
		const FGuid& InContainerId,
		int32 InSlotIndex
	);
	
	//UFUNCTION(BlueprintCallable, Category="Inventory")
	//void SetSlotData(const FSinInventoryEntry& Entry);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void RefreshSlotByLocation(const FGuid& InContainerId, int32 InSlotIndex);
	
	UFUNCTION(BlueprintPure, Category="Inventory")
	UInventory* GetOwningInventory() const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	bool UsesNewInventorySystem() const;
	
	bool IsAnyContextMenuOpen() const;
	
	//NEW SYSTEMZ
};

