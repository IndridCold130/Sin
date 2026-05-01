// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/InventoryPanel.h"
#include "Inventory/Equipment.h"
#include "Inventory/SinBelt.h"
#include "EquipmentPanel.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API UEquipmentPanel : public UInventoryPanel
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void MoveItemClient(UInventoryPanel* Panel, int32 OldIndex, int32 DstIndex, FGameplayTag IncomingSlotType, FGameplayTag LocalSlotType) override;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UEquipment> EquipmentData;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<USinBelt> MainArmaments;

	UFUNCTION(BlueprintCallable)
		void InitializeSlots();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> EquipmentContainer;

		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> JewelleryContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> WeaponSlotContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> OffHandSlotContainer;
	
};
