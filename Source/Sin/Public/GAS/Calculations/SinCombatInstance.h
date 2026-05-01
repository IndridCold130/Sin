// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Calculations/SinPhysicalDamageCalculation.h"
#include "SinCombatInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSinCombatInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		TMap<FGameplayTag, float> DamagePhysicalRaw;
	UPROPERTY(BlueprintReadOnly)
		TMap<FGameplayTag, float> BlockedDamage;
	UPROPERTY(BlueprintReadOnly)
		TMap<FGameplayTag, float> MitigatedDamage;
	UPROPERTY(BlueprintReadOnly)
		float FinalDamage;
};
