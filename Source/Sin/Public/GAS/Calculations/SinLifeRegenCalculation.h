// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS/SinASC.h"

#include "SinLifeRegenCalculation.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinLifeRegenCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	USinLifeRegenCalculation();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	UFUNCTION()
	float CalculateHealthRegen(const FGameplayEffectSpec& Spec, USinASC* ASC) const;
	
};
