#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundBase.h"
#include "SinItemFragment_Display.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_Display : public USinItemFragment
{
	GENERATED_BODY()

public:

	// Name shown in inventory, loot menu, tooltips, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	FText DisplayName;

	// Short tooltip / quick description.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display", meta=(MultiLine="true"))
	FText ShortDescription;

	// Longer lore/mechanical description.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display", meta=(MultiLine="true"))
	FText LongDescription;

	// Inventory / loot / hotbar icon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	TSoftObjectPtr<UTexture2D> Icon;

	// Optional sound when picked up.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display|Audio")
	TSoftObjectPtr<USoundBase> PickupSound;
};