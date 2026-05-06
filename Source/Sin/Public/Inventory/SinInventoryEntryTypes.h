#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SinInventoryEntryTypes.generated.h"

class USinItemDefinition;

USTRUCT(BlueprintType)
struct FSinInventoryEntry
{
	GENERATED_BODY()

public:

	// Unique runtime identity for this inventory entry.
	// Important later for moving, saving, replication, durability, upgrades, etc.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGuid EntryId;

	// Static item definition / template.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<USinItemDefinition> ItemDefinition = nullptr;

	// Which virtual container this item belongs to.
	// Example: Inventory.Backpack, Inventory.Quest, Inventory.Bag.Herb
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FGameplayTag ContainerTag;

	// Slot inside the container.
	// For bottomless/list-style inventories this can still be used as display order.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 SlotIndex = INDEX_NONE;

	// Current stack amount.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1"))
	int32 StackCount = 1;

public:

	bool IsValid() const
	{
		return ItemDefinition != nullptr && EntryId.IsValid();
	}

	bool IsInContainer(FGameplayTag InContainerTag) const
	{
		return ContainerTag == InContainerTag;
	}

	bool IsInSlot(FGameplayTag InContainerTag, int32 InSlotIndex) const
	{
		return ContainerTag == InContainerTag && SlotIndex == InSlotIndex;
	}
};