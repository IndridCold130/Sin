// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GameItemWeapon.h"

TMap<TSubclassOf<USin_GAS_Buff>, int32> UGameItemWeapon::GetBuffs_Implementation()
{
	FSinWeapon* LocalRow = DataTable->FindRow<FSinWeapon>(ItemID, TEXT("ContextString"), true);
	if (LocalRow)
	{
		return LocalRow->Buffs;
	}
	return TMap<TSubclassOf<USin_GAS_Buff>, int32>();
}

TMap<TSubclassOf<USinGameplayAbility>, int32> UGameItemWeapon::GetBuffProcs_Implementation()
{
	FSinWeapon* LocalRow = DataTable->FindRow<FSinWeapon>(ItemID, TEXT("ContextString"), true);
	if (LocalRow)
	{
		return LocalRow->BuffProcs;
	}
	return TMap<TSubclassOf<USinGameplayAbility>, int32>();
}

TSoftObjectPtr<UTexture2D> UGameItemWeapon::GetIcon_Implementation()
{
	FSinWeapon* LocalRow = DataTable->FindRow<FSinWeapon>(ItemID, TEXT("ContextString"), true);
	if (LocalRow)
	{
		return LocalRow->Icon;
	}
	return nullptr;
}
