// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/SinRegenManaCalculation.h"

#include "GAS/SinAttributeMana.h"
#include "Misc/SinGPTs.h"

struct ManaRegenStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ManaMax);
	//DECLARE_ATTRIBUTE_CAPTUREDEF(ManaRegen);

	ManaRegenStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeMana, Mana, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeMana, ManaMax, Source, false);
		//DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeMana, ManaRegen, Source, false);
	}
};

static const ManaRegenStatics& ManaStatics()
{
	static ManaRegenStatics HRStatics;
	return HRStatics;
}

USinRegenManaCalculation::USinRegenManaCalculation()
{
	RelevantAttributesToCapture.Add(ManaRegenStatics().ManaDef);
	RelevantAttributesToCapture.Add(ManaRegenStatics().ManaMaxDef);
	//RelevantAttributesToCapture.Add(RegenStatics().ManaRegenDef);
}

void USinRegenManaCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	float Mana= 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ManaStatics().ManaDef, EvaluationParameters, Mana);
	float ManaMax = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ManaStatics().ManaMaxDef, EvaluationParameters, ManaMax);
	if (Mana >= ManaMax)
	{
		return;
	}
	float Percentage = Mana / ManaMax;
	USinASC* ASC = Cast<USinASC>(ExecutionParams.GetSourceAbilitySystemComponent());
	float CalculatedRegen = CalculateRegen(Spec, ASC, Percentage)*ManaMax;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ManaStatics().ManaProperty, EGameplayModOp::Additive, CalculatedRegen));
}

float USinRegenManaCalculation::CalculateRegen(const FGameplayEffectSpec& Spec, USinASC* ASC, float Percentage) const
{
	float LocalTimer = Spec.GetSetByCallerMagnitude(TAG_Mana_Damage);
	if (LocalTimer > 0.f)
	{
		ASC->SustainedManaDamage(LocalTimer - Spec.GetPeriod());
		return 0.f;
	}
	float BaseRegenPerSecond = 0.0083f;
	float ManaBaseLine = BaseRegenPerSecond * Spec.GetPeriod();
	return ManaBaseLine;
}
