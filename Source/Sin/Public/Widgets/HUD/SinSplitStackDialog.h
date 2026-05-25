// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/HUD/SinItemExamineDialog.h"

#include "SinSplitStackDialog.generated.h"

/**
 * 
 */

class USinItemFragment_Inventory;
struct FSinInventoryEntry;
class USlider;
class URichTextBlock;
class UInventory;
struct FGuid;
class USinItemDefinition;

UCLASS()
class SIN_API USinSplitStackDialog : public USinItemDialogue
{
	GENERATED_BODY()
	
public:
	virtual void InitFromContextMenu(USinItemContextMenu* ContextMenu) override;
	
	void InitFromDrag(UInventory* Inventory, const FGuid& IncomingSourceEntryId,
	const FGuid& TargetContainerId, int32 TargetSlotIndex);
	
	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	TObjectPtr<const USinItemFragment_Inventory> CachedInventoryFragment;
	
	const FSinInventoryEntry* ItemEntry;
	FGuid TargetContainerIdCached;
	FGuid SourceEntryIdCached;
	int32 TargetSlotIndexCached;
	
	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	FGuid TargetContainerIdForSplit;

	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	int32 TargetSlotIndexForSplit = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	TObjectPtr<USinItemDefinition> ItemDefinition;
	
	UPROPERTY(BlueprintReadWrite)
	int32 MaxStack;
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStack;
	UPROPERTY(BlueprintReadWrite)
	int32 DeduceAmount;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rich Text")
	FName MaxStackStyleName = "Split.MaxStack";
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<USlider> Slider_Amount;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_Amount;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_DeducedAmount;
	
	UFUNCTION()
	void HandleSliderChanged(float Value);
	
	UFUNCTION(BlueprintCallable)
	void HandleValueChanged(float Value);
	
	void RefreshAmountText();
	
	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	TObjectPtr<UInventory> SourceInventory;

	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	FGuid SourceEntryId;

	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	FGuid SourceContainerId;

	UFUNCTION(BlueprintCallable, Category="Item Dialog")
	void SetSplitSource(
		UInventory* InInventory,
		const FGuid& InEntryId,
		const FGuid& InContainerId
	);
	
	UFUNCTION(BlueprintCallable, Category="Item Dialog")
	void SplitStack();
};
