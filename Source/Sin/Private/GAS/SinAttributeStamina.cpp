// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributeStamina.h"

#include "GAS/SinASC.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Misc/SinGPTs.h"
#include "SinCharacter.h"

USinAttributeStamina::USinAttributeStamina()
	: Stamina(100.f)
	, StaminaMax(100.f)
{
}

void USinAttributeStamina::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void USinAttributeStamina::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetStaminaMaxAttribute())
	{AdjustAttributeForMaxChange(Stamina, StaminaMax, NewValue, GetStaminaAttribute());}
	else if (Attribute == GetStaminaAttribute())
	{
		StaminaRegenCheck(Attribute, NewValue);
	}
}

bool USinAttributeStamina::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	Super::PreGameplayEffectExecute(Data);
	return true;
}

void USinAttributeStamina::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ASinCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ASinCharacter>(TargetActor);
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
// First clamp it
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetStaminaMax()));
		if (TargetCharacter)
		{
			// Call for all health changes
		}

	}
}

void USinAttributeStamina::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeStamina, Stamina, OldValue);
}

void USinAttributeStamina::OnRep_StaminaMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeStamina, StaminaMax, OldValue);
}

void USinAttributeStamina::OnRep_StaminaRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeStamina, StaminaRegen, OldValue);
}

void USinAttributeStamina::StaminaRegenCheck(const FGameplayAttribute& IncomingStamina, float NewValue)
{
	if (NewValue >= StaminaMax.GetCurrentValue())
	{
		if (!GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Stamina_Full))
		{
			GetOwningAbilitySystemComponent()->AddLooseGameplayTag(TAG_Stamina_Full);
		}
		return;
	}
	if (NewValue < StaminaMax.GetCurrentValue() && GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Stamina_Full))
	{
		GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(TAG_Stamina_Full);
	}
	if (NewValue < Stamina.GetCurrentValue())
	{
		USinASC* ZeroASC = Cast<USinASC>(GetOwningAbilitySystemComponent());
		ZeroASC->SustainedStaminaDamage(1.0f);
	}
}

void USinAttributeStamina::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinAttributeStamina, Stamina);
	DOREPLIFETIME(USinAttributeStamina, StaminaMax);
	DOREPLIFETIME(USinAttributeStamina, StaminaRegen);
}