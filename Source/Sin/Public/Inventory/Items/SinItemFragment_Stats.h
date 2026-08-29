#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "GAS/SinStatTypes.h"
#include "SinItemFragment_Stats.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_Stats : public USinItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Application")
	ESinItemEffectApplicationPolicy ApplicationPolicy = ESinItemEffectApplicationPolicy::WhileEquipped;
	
	// Flat bonuses, e.g. +5 Strength, +20 Health, +12 Armor.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	TMap<FGameplayTag, float> Additives;

	// Multiplicative/percentage bonuses, e.g. +10% Max Health, +15% Fire Damage.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	//FCharStats PassiveMults;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	TMap<FGameplayTag, float> Multiplicatives;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats", meta=(ClampMin="0.0"))
	float ArmorRating = 0.0f;
};
