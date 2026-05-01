// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributeMana.h"

#include "GAS/SinASC.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "SinCharacter.h"

USinAttributeMana::USinAttributeMana()
	: Mana(40.f)
	, ManaMax(40.f)
{
}

void USinAttributeMana::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetManaMaxAttribute())
	{AdjustAttributeForMaxChange(Mana, ManaMax, NewValue, GetManaAttribute());}
	else if (Attribute == GetManaAttribute())
	{
		ManaRegenCheck(Attribute, NewValue);
		//HealthRegenCheck(Attribute, NewValue);
	}
}

bool USinAttributeMana::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	Super::PreGameplayEffectExecute(Data);
	return true;
}

void USinAttributeMana::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ASinCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ASinCharacter>(TargetActor);
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
// First clamp it
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetManaMax()));
		if (TargetCharacter)
		{
			// Call for all health changes
		}

	}
}

void USinAttributeMana::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

	void USinAttributeMana::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
	{

		DOREPLIFETIME(USinAttributeMana, Mana);
		DOREPLIFETIME(USinAttributeMana, ManaMax);
	}

	void USinAttributeMana::OnRep_Mana(const FGameplayAttributeData & OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeMana, Mana, OldValue);
	}

	void USinAttributeMana::OnRep_ManaMax(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeMana, ManaMax, OldValue);
	}

	void USinAttributeMana::ManaRegenCheck(const FGameplayAttribute& IncomingMana, float NewValue)
	{
		if (NewValue >= ManaMax.GetCurrentValue())
		{
			if (!GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Mana_Full))
			{
				GetOwningAbilitySystemComponent()->AddLooseGameplayTag(TAG_Mana_Full);
			}
			return;
		}
		if (NewValue < ManaMax.GetCurrentValue() && GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Mana_Full))
		{
			GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(TAG_Mana_Full);
		}
		if (NewValue < Mana.GetCurrentValue())
		{
			USinASC* ZeroASC = Cast<USinASC>(GetOwningAbilitySystemComponent());
			ZeroASC->SustainedManaDamage(4.0f);
		}
	}
