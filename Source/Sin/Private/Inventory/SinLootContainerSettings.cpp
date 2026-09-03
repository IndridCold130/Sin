// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/SinLootContainerSettings.h"
#include "Inventory/Inventory.h"
#include "Inventory/Items/SinItemDefinition.h"

const FSinLootTableEntry* USinLootTable::PickWeightedEntry(
	const TSet<USinItemDefinition*>& ExcludedItems) const
{
	float TotalWeight = 0.f;

	for (const FSinLootTableEntry& Entry : Entries)
	{
		if (!Entry.Item || Entry.Weight <= 0.f)
		{
			continue;
		}
		if (ExcludedItems.Contains(Entry.Item))
		{
			continue;
		}
		TotalWeight += Entry.Weight;
	}

	if (TotalWeight <= 0.f)
	{
		return nullptr;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);

	for (const FSinLootTableEntry& Entry : Entries)
	{
		if (!Entry.Item || Entry.Weight <= 0.f)
		{
			continue;
		}
		if (ExcludedItems.Contains(Entry.Item))
		{
			continue;
		}

		if (Roll <= Entry.Weight)
		{
			return &Entry;
		}
		Roll -= Entry.Weight;
	}

	return nullptr;
}

bool USinLootTable::GenerateInto(UInventory* TargetInventory) const
{
	if (!TargetInventory || Entries.IsEmpty())
	{
		return false;
	}

	const int32 DropCount = FMath::RandRange(MinDrops, FMath::Max(MinDrops, MaxDrops));

	TSet<USinItemDefinition*> AlreadyDropped;
	bool bAddedAnything = false;

	for (int32 i = 0; i < DropCount; ++i)
	{
		const FSinLootTableEntry* Picked = PickWeightedEntry(AlreadyDropped);
		if (!Picked || !Picked->Item)
		{
			break;
		}

		const int32 StackMin = FMath::Max(1, Picked->MinStack);
		const int32 StackMax = FMath::Max(StackMin, Picked->MaxStack);
		const int32 StackCount = FMath::RandRange(StackMin, StackMax);

		if (TargetInventory->AddItemToInventory(Picked->Item, StackCount))
		{
			bAddedAnything = true;
		}

		if (Picked->bUnique)
		{
			AlreadyDropped.Add(Picked->Item);
		}
	}

	return bAddedAnything;
}

bool USinLootContainerSettings::GenerateInto(UInventory* TargetInventory) const
{
	if (!TargetInventory)
	{
		return false;
	}

	bool bAddedAnything = false;

	for (const FSinLootTableWeight& Guaranteed : GuaranteedTables)
	{
		if (!Guaranteed.Table)
		{
			continue;
		}

		const int32 Rolls = FMath::RandRange(
			Guaranteed.MinRolls,
			FMath::Max(Guaranteed.MinRolls, Guaranteed.MaxRolls));

		for (int32 i = 0; i < Rolls; ++i)
		{
			bAddedAnything |= Guaranteed.Table->GenerateInto(TargetInventory);
		}
	}

	const int32 TablePickCount = FMath::RandRange(
		MinRandomTables,
		FMath::Max(MinRandomTables, MaxRandomTables));

	TArray<FSinLootTableWeight> Pool = WeightedTables;
	for (int32 i = 0; i < TablePickCount && Pool.Num() > 0; ++i)
	{
		float TotalWeight = 0.f;
		for (const FSinLootTableWeight& Candidate : Pool)
		{
			if (Candidate.Table && Candidate.Weight > 0.f)
			{
				TotalWeight += Candidate.Weight;
			}
		}
		if (TotalWeight <= 0.f)
		{
			break;
		}

		float Roll = FMath::FRandRange(0.f, TotalWeight);
		int32 PickedIndex = INDEX_NONE;

		for (int32 Index = 0; Index < Pool.Num(); ++Index)
		{
			const FSinLootTableWeight& Candidate = Pool[Index];
			if (!Candidate.Table || Candidate.Weight <= 0.f)
			{
				continue;
			}

			if (Roll <= Candidate.Weight)
			{
				PickedIndex = Index;
				break;
			}
			Roll -= Candidate.Weight;
		}

		if (PickedIndex == INDEX_NONE)
		{
			break;
		}

		const FSinLootTableWeight Picked = Pool[PickedIndex];
		Pool.RemoveAt(PickedIndex);

		const int32 Rolls = FMath::RandRange(
			Picked.MinRolls,
			FMath::Max(Picked.MinRolls, Picked.MaxRolls));

		for (int32 r = 0; r < Rolls; ++r)
		{
			bAddedAnything |= Picked.Table->GenerateInto(TargetInventory);
		}
	}

	return bAddedAnything;
}
