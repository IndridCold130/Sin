// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/GameItemBase.h"
#include "Interfaces/I_Inventory.h"
#include "GameItemContainer.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API UGameItemContainer : public UGameItemBase, public II_Inventory
{
	GENERATED_BODY()
	
};
