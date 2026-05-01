// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/GameItemBase.h"
#include "GameItemEquipment.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API UGameItemEquipment : public UGameItemBase
{
	GENERATED_BODY()

		void Initialize_Implementation() override;
		virtual float GetItemBaseValue() override;
		int32 GetItemIntProperty_Implementation(EItemIntStat Stat) override;
		float GetItemFloatProperty_Implementation(EItemFloatStat Stat) override;
		TSoftObjectPtr<UTexture2D> GetIcon_Implementation() override;
		float GetItemDurabilityPercentage_Implementation() override;
		void HandleDurabilityDamage_Implementation(ASinCharacter* HitCharacter) override;
		virtual TMap<TSubclassOf<USin_GAS_Buff>, int32> GetBuffs_Implementation() override;
		virtual TMap<TSubclassOf<USinGameplayAbility>, int32> GetBuffProcs_Implementation() override;
		virtual TArray< FItemGrantedPerk> GetBonusPerks_Implementation() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FEquipmentItem EquipmentData;

	UPROPERTY(ReplicatedUsing = OnRep_Durability, SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Item")
		float Durability;
	
	//UPROPERTY(ReplicatedUsing = OnRep_ItemPropertyChanged, SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Item")
	UPROPERTY(ReplicatedUsing = OnRep_Durability, SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Item")
		float MaxDurability;

		UFUNCTION(Category = "Item Float Stats")
		void OnRep_Durability(const float& DeltaValue);

		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
