#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SinItemFragment_Equipment.h"
#include "SinItemDefinition.generated.h"

class USinItemFragment;

UCLASS(BlueprintType)
class SIN_API USinItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FGameplayTagContainer ItemTags;
	
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category="Item")
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
};