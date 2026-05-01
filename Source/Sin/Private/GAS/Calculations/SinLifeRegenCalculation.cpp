// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/SinLifeRegenCalculation.h"

#include "GAS/SinAttributeHealth.h"
#include "Misc/SinGPTs.h"

struct HealthRegenStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthMax);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthRegen);

	HealthRegenStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeHealth, Health, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeHealth, HealthMax, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeHealth, HealthRegen, Source, false);
	}
};

static const HealthRegenStatics& RegenStatics()
{
	static HealthRegenStatics HRStatics;
	return HRStatics;
}

USinLifeRegenCalculation::USinLifeRegenCalculation()
{
	RelevantAttributesToCapture.Add(HealthRegenStatics().HealthDef);
	RelevantAttributesToCapture.Add(HealthRegenStatics().HealthMaxDef);
	RelevantAttributesToCapture.Add(HealthRegenStatics().HealthRegenDef);
}

void USinLifeRegenCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	float CurrentHealth = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(RegenStatics().HealthDef, EvaluationParameters, CurrentHealth);
	float MaxHealth = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(RegenStatics().HealthMaxDef, EvaluationParameters, MaxHealth);
	if (CurrentHealth >= MaxHealth)
	{
		return;
	}
	float RegenMod = 0.f;
	USinASC* TrueSourceASC = Cast<USinASC>(ExecutionParams.GetSourceAbilitySystemComponent());
	RegenMod = CalculateHealthRegen(Spec, TrueSourceASC);
	if (RegenMod <= 0.f)
	{
		return;
	}
	float CalculatedRegen = MaxHealth * RegenMod;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(RegenStatics().HealthProperty, EGameplayModOp::Additive, CalculatedRegen));
}

float USinLifeRegenCalculation::CalculateHealthRegen(const FGameplayEffectSpec& Spec, USinASC* ASC) const
{
	float LocalTimer = Spec.GetSetByCallerMagnitude(TAG_Health_Damage);
	if (LocalTimer > 0.f)
	{
		ASC->SustainedHealthDamage(LocalTimer - Spec.GetPeriod());
		return 0.f;
	}
	float FinalValue = 0.002f;
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Buff);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Gear);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Curse);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Perk);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Racial);
	return FinalValue + Spec.GetSetByCallerMagnitude(TAG_Health_Regen);
}
