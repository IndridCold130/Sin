// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
//#include "Inventory/GameItemBase.h"
#include "Equipment.generated.h"


/**
 * 
 */
UCLASS()
class SIN_API UEquipment : public UInventory
{
	GENERATED_BODY()

public:

	UEquipment();

	bool SlotRules(UGameItemBase* Item, int32 Slot) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		TMap<int32, FGameplayTag> EquipmentSlotRules;
	
};
