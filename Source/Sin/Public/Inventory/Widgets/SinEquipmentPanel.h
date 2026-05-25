// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "SinEquipmentPanel.generated.h"

class UInventorySlot;
class UInventory;
/**
 * 
 */
UCLASS()
class SIN_API USinEquipmentPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void SetEquipmentData(AActor* Owner);
	
	UPROPERTY(BlueprintReadOnly, Category="Equipment")
	TObjectPtr<AActor> OwningActor;

	UPROPERTY(BlueprintReadOnly, Category="Equipment")
	TObjectPtr<UInventory> DataHolder = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment")
	FGameplayTag InventoryType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment")
	FGameplayTag EquipmentContainerTag;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Equipment")
	void InitializeEquipmentSlots();
	
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void RefreshEquipmentSlots();
	
	UFUNCTION()
	void HandleEquipmentContainerChanged(UInventory* Inventory, FGuid ContainerId);
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> HeadSlot;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> ChestSlot;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> HandsSlot;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> LegsSlot;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> FeetSlot;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> AmuletSlot;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> RingSlot1;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> RingSlot2;
};
