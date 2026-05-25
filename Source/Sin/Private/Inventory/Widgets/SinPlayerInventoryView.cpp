// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Widgets/SinPlayerInventoryView.h"
#include "SinPlayerController.h"
#include "SinCharacter.h"

void UPlayerInventoryView::RecalculateWeight_Implementation(UInventory* Holder, float& BackpackWeight, float& EquipLoad)
{
	if (!Holder) { BackpackWeight = 0.0f; EquipLoad = 0.0f; return; }
	if (ASinCharacter* SinChar = Cast<ASinCharacter>(Holder->GetOwner()))
	{
		BackpackWeight = SinChar->GetBackpackWeight();
		EquipLoad = SinChar->GetEquipLoad();
	}
}

void UPlayerInventoryView::ShowEquipmentView()
{
}

void UPlayerInventoryView::ShowStatsView()
{
}
