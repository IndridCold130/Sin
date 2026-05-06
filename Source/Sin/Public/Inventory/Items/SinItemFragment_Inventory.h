#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "SinItemFragment_Inventory.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_Inventory : public USinItemFragment
{
	GENERATED_BODY()

public:

	// Maximum amount per stack. Ignored if bStackable is false.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1"))
	int32 MaxStackSize = 1;
	
	UFUNCTION(BlueprintPure, Category="Inventory")
	bool IsStackable() const
	{
		return MaxStackSize > 1;
	}

	// Optional item weight. Useful later for carry capacity.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="0.0"))
	float Weight = 0.0f;

	// Base vendor value / gold value.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="0"))
	int32 Value = 0;

	// Kept 1x1 for now, but future-proofs you for spatial/jigsaw inventory.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1", UIMin="1"))
	FIntPoint InventorySize = FIntPoint(1, 1);

	// Optional sorting/order hint for UI.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="0"))
	int32 SortPriority = 0;
	
	// Optional, flexible replacement for ItemType enum
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	FGameplayTag ItemCategory;
};
