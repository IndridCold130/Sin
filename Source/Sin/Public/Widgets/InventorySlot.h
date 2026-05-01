// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/InventoryPanel.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"
#include "Widgets/InvSlotVisual.h"
#include "Inventory/GameItemBase.h"
#include "InventorySlot.generated.h"

//class UBorder;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UInvSlotVisual> VisualClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UBorder* IconBorder;

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
	//virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	//virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};


/**
	 * Called when a mouse button is double clicked.  Override this in derived classes.
	 *
	 * @param  InMyGeometry  Widget geometry
	 * @param  InMouseEvent  Mouse button event
	 * @return  Returns whether the event was handled, along with other possible actions
	 */

