#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SinInventoryContainerTypes.generated.h"

USTRUCT(BlueprintType)
struct FSinInventorySlotRule
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 SlotIndex = INDEX_NONE;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TSoftObjectPtr<UTexture2D> EmptySlotIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FGameplayTag SlotTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FGameplayTagContainer AcceptedItemTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FGameplayTagContainer BlockedItemTags;
	
	bool AcceptsItemTags(const FGameplayTagContainer& ItemTags) const
	{
		if (!BlockedItemTags.IsEmpty() && ItemTags.HasAny(BlockedItemTags))
		{
			return false;
		}

		if (AcceptedItemTags.IsEmpty())
		{
			return true;
		}

		return ItemTags.HasAny(AcceptedItemTags);
	}
};

USTRUCT(BlueprintType)
struct FSinInventoryContainerState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid ContainerId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTag ContainerTag;

	// Optional whitelist
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTagContainer AcceptedItemTags;

	// Optional blacklist
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTagContainer BlockedItemTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FSinInventorySlotRule> SlotRules;
	
	const FSinInventorySlotRule* FindSlotRule(int32 SlotIndex) const
	{
		for (const FSinInventorySlotRule& Rule : SlotRules)
		{
			if (Rule.SlotIndex == SlotIndex)
			{
				return &Rule;
			}
		}

		return nullptr;
	}
	
	bool AcceptsItemTagsAtSlot(
	const FGameplayTagContainer& ItemTags,
	int32 SlotIndex) const
	{
		if (!AcceptsItemTags(ItemTags))
		{
			return false;
		}

		const FSinInventorySlotRule* SlotRule = FindSlotRule(SlotIndex);
		if (!SlotRule)
		{
			return true;
		}

		return SlotRule->AcceptsItemTags(ItemTags);
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="0"))
	int32 SlotCount = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	bool bBottomless = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ClampMin="1"))
	int32 MinVisibleSlots = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 SortPriority = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|UI")
	bool bShowInContainerTabs = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Rules")
	bool bAllowAutoAdd = true;

	bool AcceptsItemTags(const FGameplayTagContainer& ItemTags) const
	{
		// 1. Hard reject
		if (!BlockedItemTags.IsEmpty() && ItemTags.HasAny(BlockedItemTags))
		{
			return false;
		}

		// 2. No whitelist → accept everything else
		if (AcceptedItemTags.IsEmpty())
		{
			return true;
		}

		// 3. Whitelist check
		return ItemTags.HasAny(AcceptedItemTags);
	}
};


UCLASS(BlueprintType)
class SIN_API USinInventoryContainerSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FSinInventoryContainerState> Containers;
};