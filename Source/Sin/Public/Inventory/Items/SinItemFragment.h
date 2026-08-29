#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "SinItemFragment.generated.h"

UENUM(BlueprintType)
enum class ESinItemEffectApplicationPolicy : uint8
{
	Never UMETA(DisplayName="Never"),

	WhileEquipped UMETA(DisplayName="While Equipped"),

	WhileInInventory UMETA(DisplayName="While In Inventory"),
	
	WhileInQuickslot UMETA(DisplayName="While In Quickslot"),

	Always UMETA(DisplayName="Always")
};

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment : public UObject
{
	GENERATED_BODY()

public:

	// Optional: override in child fragments if needed later
	virtual void OnInstanced() {}
};
