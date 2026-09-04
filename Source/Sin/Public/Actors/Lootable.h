// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable.h"
#include "Lootable.generated.h"

class UInventory;
class USinLootContainerSettings;

/**
 * 
 */
UCLASS()
class SIN_API ALootable : public AInteractable
{
	GENERATED_BODY()

public:

	ALootable();

		UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UInventory> Loot;
	
			void Focus(ASinPlayerController* Controller);
		virtual void Focus_Implementation(ASinPlayerController* Controller);

			void EndFocus(ASinPlayerController* Controller);
		virtual void EndFocus_Implementation(ASinPlayerController* Controller);


			void Interact(ASinPlayerController* Caller);
		virtual void Interact_Implementation(ASinPlayerController* Caller);

		UInventory* GetInventory();
		virtual UInventory* GetInventory_Implementation();

		FString GetToolTipInfo();
		virtual FString GetToolTipInfo_Implementation();
	
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot")
		TObjectPtr<USinLootContainerSettings> LootSettings;

		UPROPERTY(SaveGame)
		bool bHasGeneratedLoot = false;

		void EnsureLootGenerated();
};
