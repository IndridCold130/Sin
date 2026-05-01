// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributeSecondary.h"

#include "Components/SinCMC.h"

USinAttributeSecondary::USinAttributeSecondary()
	: MovementSpeed(1.f)
	, AttackSpeed(1.f)
	, Toughness(0.f)
{
}

void USinAttributeSecondary::OnRep_MovementSpeed(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeSecondary, MovementSpeed, OldValue);
}

void USinAttributeSecondary::OnRep_AttackSpeed(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeSecondary, AttackSpeed, OldValue);
}

void USinAttributeSecondary::OnRep_Toughness(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeSecondary, Toughness, OldValue);
}

void USinAttributeSecondary::OnRep_Avoidance(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeSecondary, Avoidance, OldValue);
}

void USinAttributeSecondary::OnRep_Resilience(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeSecondary, Resilience, OldValue);
}

void USinAttributeSecondary::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinAttributeSecondary, MovementSpeed);
	DOREPLIFETIME(USinAttributeSecondary, AttackSpeed);
	DOREPLIFETIME(USinAttributeSecondary, Toughness);
	DOREPLIFETIME(USinAttributeSecondary, Resilience);
	DOREPLIFETIME(USinAttributeSecondary, Avoidance);
}
