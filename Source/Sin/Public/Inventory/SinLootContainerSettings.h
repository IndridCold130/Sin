// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SinLootContainerSettings.generated.h"

class USinItemDefinition;
class UInventory;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSinLootTableEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USinItemDefinition> Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnique = false;
};

UCLASS(BlueprintType)
class SIN_API USinLootTable : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot")
	TArray<FSinLootTableEntry> Entries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot", meta=(ClampMin="0"))
	int32 MinDrops = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot", meta=(ClampMin="0"))
	int32 MaxDrops = 3;

	const FSinLootTableEntry* PickWeightedEntry(
		const TSet<USinItemDefinition*>& ExcludedItems) const;

	bool GenerateInto(UInventory* TargetInventory) const;
};

USTRUCT(BlueprintType)
struct FSinLootTableWeight
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<USinLootTable> Table = nullptr;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0.0"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere)
	int32 MinRolls = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxRolls = 1;
};

UCLASS()
class SIN_API USinLootContainerSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Never compete. Always generate from these.
	UPROPERTY(EditAnywhere, Category="Loot|Guaranteed")
	TArray<FSinLootTableWeight> GuaranteedTables;

	// Compete with each other. Pick PickTableCount of these.
	UPROPERTY(EditAnywhere, Category="Loot|Random")
	TArray<FSinLootTableWeight> WeightedTables;

	UPROPERTY(EditAnywhere, Category="Loot|Random")
	int32 MinRandomTables = 1;

	UPROPERTY(EditAnywhere, Category="Loot|Random")
	int32 MaxRandomTables = 1;
	
	bool GenerateInto(UInventory* TargetInventory) const;
};
