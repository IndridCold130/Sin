// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GameItemEquipment.h"
#include "Inventory/Equipment.h"

void UGameItemEquipment::Initialize_Implementation()
{
	FEquipmentItem* LocItemData = DataTable->FindRow<FEquipmentItem>(ItemID, TEXT("ContextString"), true);
	if (LocItemData)
	{
		EquipmentData = *LocItemData;
		Durability = EquipmentData.Durability;
		MaxDurability = EquipmentData.MaxDurability;
		//Owner->OnSignalItemAdded.Broadcast(Owner, CurrentIndex, this, PreviousIndex, PreviousOwner);
	}
}

float UGameItemEquipment::GetItemBaseValue()
{
	return EquipmentData.Price;
}

int32 UGameItemEquipment::GetItemIntProperty_Implementation(EItemIntStat Stat)
{
	FEquipmentItem* LocItemData = DataTable->FindRow<FEquipmentItem>(ItemID, TEXT("ContextString"), true);
	if (LocItemData)
	{
		switch (Stat)
		{
		case EItemIntStat::MaxStack:
		{
			return LocItemData->MaxStack;
		}
		case EItemIntStat::Stack:
		{
			return Stack;
		}
		default: return 0;
		}
	}
	return 0;
}

float UGameItemEquipment::GetItemFloatProperty_Implementation(EItemFloatStat Stat)
{
	switch (Stat)
	{
	case EItemFloatStat::Durability:
	{
		return Durability;
	}
	case EItemFloatStat::MaxDurability:
	{
		return MaxDurability;
	}
	default: return Super::GetItemFloatProperty_Implementation(Stat);
	}
}

TSoftObjectPtr<UTexture2D> UGameItemEquipment::GetIcon_Implementation()
{
	return EquipmentData.Icon;
}

float UGameItemEquipment::GetItemDurabilityPercentage_Implementation()
{
	if (MaxDurability < 1)
	{
		return 1.0;
	}
		return Durability / MaxDurability;
}

void UGameItemEquipment::HandleDurabilityDamage_Implementation(ASinCharacter* HitCharacter)
{
	Durability = Durability-FMath::FRandRange(0.5f, 1.25f);
}

TMap<TSubclassOf<USin_GAS_Buff>, int32> UGameItemEquipment::GetBuffs_Implementation()
{
	return EquipmentData.Buffs;
}

TMap<TSubclassOf<USinGameplayAbility>, int32> UGameItemEquipment::GetBuffProcs_Implementation()
{
	return EquipmentData.BuffProcs;
}

TArray<FItemGrantedPerk> UGameItemEquipment::GetBonusPerks_Implementation()
{
	return EquipmentData.BonusPerks;
}

void UGameItemEquipment::OnRep_Durability(const float& DeltaValue)
{
}

void UGameItemEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UGameItemBase, Stack);
	//DOREPLIFETIME(UGameItemEquipment, Durability);
	DOREPLIFETIME(ThisClass, Durability);
	DOREPLIFETIME(ThisClass, MaxDurability);
}
