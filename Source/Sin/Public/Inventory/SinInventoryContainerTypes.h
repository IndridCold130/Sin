#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SinInventoryContainerTypes.generated.h"

USTRUCT(BlueprintType)
struct FSinInventoryContainerState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTag ContainerTag;

	// Optional whitelist
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTagContainer AcceptedItemTags;

	// Optional blacklist
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTagContainer BlockedItemTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="0"))
	int32 SlotCount = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	bool bBottomless = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 SortPriority = 0;

public:

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