// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "SinItemFragment_Usable.generated.h"

class USinGameplayAbility;
/**
 * 
 */
UCLASS()
class SIN_API USinItemFragment_Usable : public USinItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable")
	TSubclassOf<USinGameplayAbility> GrantedAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable")
	ESinItemEffectApplicationPolicy ApplicationPolicy =
		ESinItemEffectApplicationPolicy::WhileInQuickslot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable")
	bool bConsumeOnUse = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable")
	bool bMustBeQuickslotted = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable|Charges", meta=(ClampMin="1"))
	int32 MaxCharges = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable", meta=(ClampMin="1"))
	int32 ChargesPerUse = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable|Cooldown", meta=(ClampMin="0.0"))
	float RecastTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable|Charges")
	bool bUsesCharges = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Usable|Charges")
	bool bRefillable = false;
};
