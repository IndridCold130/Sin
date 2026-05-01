// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/SinRegenStaminaCalculation.h"

#include "GAS/SinAttributeStamina.h"
#include "Misc/SinGPTs.h"

struct StaminaRegenStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Stamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaMax);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaRegen);

	StaminaRegenStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeStamina, Stamina, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeStamina, StaminaMax, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeStamina, StaminaRegen, Source, false);
	}
};

static const StaminaRegenStatics& CerebralStatics()
{
	static StaminaRegenStatics HRStatics;
	return HRStatics;
}

USinRegenStaminaCalculation::USinRegenStaminaCalculation()
{
	BlockedTags.AddTagFast(TAG_Substate_Dodge);
	BlockedTags.AddTagFast(TAG_Substate_Attack);
	RelevantAttributesToCapture.Add(StaminaRegenStatics().StaminaDef);
	RelevantAttributesToCapture.Add(StaminaRegenStatics().StaminaMaxDef);
	RelevantAttributesToCapture.Add(StaminaRegenStatics().StaminaRegenDef);
}

void USinRegenStaminaCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	USinASC* TrueSourceASC = Cast<USinASC>(ExecutionParams.GetSourceAbilitySystemComponent());
	//FGameplayTagContainer Tagalicious = TrueSourceASC->GetOwnedGameplayTags();
	if (TrueSourceASC->HasAnyMatchingGameplayTags(BlockedTags))
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(CerebralStatics().StaminaProperty, EGameplayModOp::Additive, 0.0f));
		return;
	}
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	float Stamina = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CerebralStatics().StaminaDef, EvaluationParameters, Stamina);
	float StaminaMax = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CerebralStatics().StaminaMaxDef, EvaluationParameters, StaminaMax);
	if (Stamina >= StaminaMax)
	{
		return;
	}
	float Percentage = Stamina / StaminaMax;
	float Regen = 0.f;
	float GuardRegenMod = TrueSourceASC->HasMatchingGameplayTag(TAG_Substate_Block) ? 0.4 : 1.0;
	Regen = CalculateRegen(Spec, TrueSourceASC, Percentage, GuardRegenMod);
	if (Regen <= 0.f)
	{
		return;
	}
	float CalculatedRegen = StaminaMax * Regen*GuardRegenMod;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(CerebralStatics().StaminaProperty, EGameplayModOp::Additive, CalculatedRegen));
}

float USinRegenStaminaCalculation::CalculateRegen(const FGameplayEffectSpec& Spec, USinASC* ASC, float Percentage, float GuardRegenMod) const
{
	float LocalTimer = Spec.GetSetByCallerMagnitude(TAG_Stamina_Damage);
	if (LocalTimer > 0.f)
	{
		ASC->SustainedStaminaDamage(LocalTimer - Spec.GetPeriod());
		return 0.f;
	}
	float FinalValue = 0.05f;
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Buff);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Gear);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Curse);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Perk);
	FinalValue += Spec.GetSetByCallerMagnitude(TAG_Racial);
	return FinalValue + Spec.GetSetByCallerMagnitude(TAG_Stamina_Regen);
}
