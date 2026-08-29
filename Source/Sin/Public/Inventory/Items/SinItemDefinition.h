#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SinItemFragment_Equipment.h"
#include "SinItemDefinition.generated.h"

class USinItemFragment;
class USinItemFragment_Stats;
class USinItemFragment_GrantedEffects;
class USinItemFragment_GrantedPerks;
class USinItemFragment_Equipment;

UCLASS(BlueprintType)
class SIN_API USinItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FGameplayTagContainer ItemTags;
	
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category="Fragments")
	TArray<TObjectPtr<USinItemFragment>> Fragments;

	template <typename T>
	const T* FindFragmentByClass() const
	{
		for (USinItemFragment* Fragment : Fragments)
		{
			if (T* Typed = Cast<T>(Fragment))
			{
				return Typed;
			}
		}
		return nullptr;
	}
	UFUNCTION(BlueprintPure, Category="Item|Fragments", meta=(DeterminesOutputType="FragmentClass"))
	USinItemFragment* FindFragmentByClass(TSubclassOf<USinItemFragment> FragmentClass) const;
	
	UFUNCTION(BlueprintPure, Category="Item|Fragments")
	USinItemFragment_Stats* GetStatsFragment() const;

	UFUNCTION(BlueprintPure, Category="Item|Fragments")
	USinItemFragment_GrantedEffects* GetGrantedEffectsFragment() const;

	UFUNCTION(BlueprintPure, Category="Item|Fragments")
	USinItemFragment_GrantedPerks* GetPerksFragment() const;
	
	UFUNCTION(BlueprintPure, Category="Item|Fragments")
	USinItemFragment_Equipment* GetEquipmentFragment() const;
	
};