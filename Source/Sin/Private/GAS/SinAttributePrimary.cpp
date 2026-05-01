// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributePrimary.h"

#include "GAS/SinASC.h"
#include "GAS/SinAttributeSecondary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "SinCharacter.h"

USinAttributePrimary::USinAttributePrimary()
{
	//ASC = Cast<USinASC>(GetOwningAbilitySystemComponent());
}

void USinAttributePrimary::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void USinAttributePrimary::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (OldValue == NewValue)
	{
		return;
	}
	if (GetASC())
	{
		bool HandleFound;
		FActiveGameplayEffectHandle LocalHandle = ASC->GetFirstGameEffectHandleBasedOnTag(TAG_Attribute_Progression, HandleFound);
		if (HandleFound)
		{
			if (Attribute == GetConstitutionAttribute())
			{
				CalculateHealthBonus(LocalHandle);
				CalculateStaminaBonus(LocalHandle);
				return;
			}
			if (Attribute == GetStrengthAttribute())
			{
				CalculateHealthBonus(LocalHandle);
				return;
			}
			if (Attribute == GetDexterityAttribute())
			{

			}
		}
	}
}

bool USinAttributePrimary::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return true;
}

void USinAttributePrimary::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
}

const USinAttributeSecondary* USinAttributePrimary::GetSecondaryAttributeSet()
{
	//UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ASC)
	{
		const USinAttributeSecondary* SecondarySet = Cast<USinAttributeSecondary>(ASC->GetAttributeSet(USinAttributeSecondary::StaticClass()));
		return SecondarySet;
	}
	return nullptr;
}

const USinAttributeHealth* USinAttributePrimary::GetHealthAttributeSet()
{
	return nullptr;
}

void USinAttributePrimary::CalculateHealthBonus(FActiveGameplayEffectHandle Handle)
{
	float LocalStrength = Strength.GetCurrentValue() * 1.f;
	GetASC()->UpdateActiveGameplayEffectSetByCallerMagnitude
	(Handle, TAG_Attribute_Major_HealthMax, Constitution.GetCurrentValue() * 3.0f + LocalStrength);
}

void USinAttributePrimary::CalculateStaminaBonus(FActiveGameplayEffectHandle Handle)
{
	float LocalDex = Dexterity.GetCurrentValue() * 1.f;
	(Handle, TAG_Attribute_Major_StaminaMax, Constitution.GetCurrentValue() * 0.5f + LocalDex);
}

void USinAttributePrimary::CalculateManaBonus(FActiveGameplayEffectHandle Handle)
{
	float LocalSorc = Sorcery.GetCurrentValue() * 2.f;
	float LocalFaith= Faith.GetCurrentValue() * 1.f;
	float LocalCharisma = Charisma.GetCurrentValue() * 0.5f;
}

void USinAttributePrimary::CalculateToughnessBonus(FActiveGameplayEffectHandle Handle)
{
	float LocalStrength = Strength.GetCurrentValue() * 0.02f;
	(Handle, TAG_Attribute_Major_StaminaMax, LocalStrength);
}

void USinAttributePrimary::CalculateAvoidanceBonus(FActiveGameplayEffectHandle Handle)
{
}

void USinAttributePrimary::CalculatePoiseBonus(FActiveGameplayEffectHandle Handle)
{
}

void USinAttributePrimary::CalculateResilienceBonus(FActiveGameplayEffectHandle Handle)
{
}

void USinAttributePrimary::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinAttributePrimary, Strength);
	DOREPLIFETIME(USinAttributePrimary, Dexterity);
	DOREPLIFETIME(USinAttributePrimary, Constitution);
	DOREPLIFETIME(USinAttributePrimary, Sorcery);
	DOREPLIFETIME(USinAttributePrimary, Faith);
	DOREPLIFETIME(USinAttributePrimary, Charisma);
}

void USinAttributePrimary::OnRep_Strength(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributePrimary, Strength, OldValue);
}

void USinAttributePrimary::OnRep_Dexterity(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributePrimary, Dexterity, OldValue);
}

void USinAttributePrimary::OnRep_Constitution(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributePrimary, Constitution, OldValue);
}

void USinAttributePrimary::OnRep_Sorcery(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributePrimary, Sorcery, OldValue);
}

void USinAttributePrimary::OnRep_Faith(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributePrimary, Faith, OldValue);
}

void USinAttributePrimary::OnRep_Charisma(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributePrimary, Charisma, OldValue);
}

USinASC* USinAttributePrimary::GetASC()
{
	if (ASC)
	{
		return ASC;
	}
	ASC = Cast<USinASC>(GetOwningAbilitySystemComponent());
	return ASC;
}
