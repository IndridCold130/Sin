// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/InventoryPanel.h"
#include "SinPlayerInventoryView.generated.h"

/**
 * 
 */
class USinEquipmentPanel;

UCLASS()
class SIN_API UPlayerInventoryView : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
	void RecalculateWeight(UInventory* Holder, float& BackpackWeight, float& EquipLoad);
	virtual void RecalculateWeight_Implementation(UInventory* Holder, float& BackpackWeight, float& EquipLoad);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void ShowEquipmentView();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void ShowStatsView();
	
protected:

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory")
	TObjectPtr<UInventoryPanel> MainInventoryPanel;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory")
	TObjectPtr<USinEquipmentPanel> EquipmentInventoryPanel;
};
