// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/GameItemEquipment.h"
#include "GameItemWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API UGameItemWeapon : public UGameItemEquipment
{
	GENERATED_BODY()

	virtual TMap<TSubclassOf<USin_GAS_Buff>, int32> GetBuffs_Implementation() override;
	virtual TMap<TSubclassOf<USinGameplayAbility>, int32> GetBuffProcs_Implementation() override;

public:

	TSoftObjectPtr<UTexture2D> GetIcon_Implementation() override;
	
};
