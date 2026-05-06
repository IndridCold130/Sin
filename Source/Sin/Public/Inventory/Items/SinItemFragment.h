#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "SinItemFragment.generated.h"

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment : public UObject
{
	GENERATED_BODY()

public:

	// Optional: override in child fragments if needed later
	virtual void OnInstanced() {}
};
