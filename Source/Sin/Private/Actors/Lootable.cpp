// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Lootable.h"
#include "Inventory/Inventory.h"
#include "SinPlayerController.h"

ALootable::ALootable()
{
	Loot = CreateDefaultSubobject<UInventory>(("Loot"));
}


void ALootable::Focus_Implementation(ASinPlayerController* Controller)
{
	Super::Focus_Implementation(Controller);
	//Controller->MainHUD->ToggleToolTip(true, GetToolTipInfo_Implementation());
}

void ALootable::EndFocus_Implementation(ASinPlayerController* Controller)
{
	Super::EndFocus_Implementation(Controller);
	//Controller->MainHUD->ToggleToolTip(false);
}

void ALootable::Interact_Implementation(ASinPlayerController* Caller)
{
	Super::Interact_Implementation(Caller);
	Caller->LoadSinMenu(TAG_Menu_Loot, this, true);
	//Caller->MainHUD->ToggleInteractableMenu(EInteractionMenu::Loot, Loot);
}

UInventory* ALootable::GetInventory_Implementation()
{
	TObjectPtr<UInventory> Inventory = Cast<UInventory>(this->GetComponentByClass(UInventory::StaticClass()));
	if (Inventory)
	{
		return Inventory;
	}
	return nullptr;
}

FString ALootable::GetToolTipInfo_Implementation()
{
	return Super::GetToolTipInfo_Implementation();
	//return TEXT("Default Value");
}
