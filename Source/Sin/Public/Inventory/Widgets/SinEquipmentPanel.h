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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment")
	FGameplayTag UtilityContainerTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTag WeaponContainerTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTag AmmoContainerTag;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Equipment")
	void InitializeEquipmentSlots();
	
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void RefreshEquipmentSlots();
	
	UFUNCTION()
	void HandleEquipmentContainerChanged(UInventory* Inventory, FGuid ContainerId);
	
	UFUNCTION()
	void HandleInventoryEntryAdded(UInventory* Inventory, const FSinInventoryEntry& Entry);

	UFUNCTION()
	void HandleInventoryEntryRemoved(UInventory* Inventory, const FSinInventoryEntry& RemovedEntry);

	UFUNCTION()
	void HandleInventoryEntryChanged(UInventory* Inventory, const FSinInventoryEntry& OldEntry, const FSinInventoryEntry& NewEntry);
	
	UFUNCTION()
	UInventorySlot* GetSlotWidgetByLocation(const FGuid& ContainerId, int32 SlotIndex) const;
	
	UFUNCTION()
	void RefreshSlotIfRelevant(const FGuid& ContainerId, int32 SlotIndex);

	UFUNCTION()
	void RefreshEntryLocation(const FSinInventoryEntry& Entry);
	
	UFUNCTION()
	bool IsRelevantEntry(const FSinInventoryEntry& Entry) const;
	
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
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> UtilitySlot1;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> UtilitySlot2;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Inventory|Equipment")
	TObjectPtr<UInventorySlot> UtilitySlot3;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> WeaponSlot0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> WeaponSlot1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> WeaponSlot2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> WeaponSlot3;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> AmmoSlot1;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> AmmoSlot2;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory|Equipment")
	TObjectPtr<UInventorySlot> AmmoSlot3;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void ClearEquipmentData();


	virtual void NativeDestruct() override;
};
