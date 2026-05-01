// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS/SinASC.h"

#include "SinRegenStaminaCalculation.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinRegenStaminaCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	USinRegenStaminaCalculation();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	UFUNCTION()
	float CalculateRegen(const FGameplayEffectSpec& Spec, USinASC* ASC, float Percentage, float GuardRegenMod) const;
private:
	FGameplayTagContainer BlockedTags;
};
