// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributeProgression.h"

#include "GAS/SinASC.h"

void USinAttributeProgression::OnRep_Level(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeProgression, Level, OldValue);
}

void USinAttributeProgression::OnRep_Experience(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeProgression, Experience, OldValue);
}

void USinAttributeProgression::OnRep_AbilityScorePoints(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeProgression, AbilityScorePoints, OldValue);
}

void USinAttributeProgression::OnRep_SpentAbilityScorePoints(FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeProgression, SpentAbilityScorePoints, OldValue);
}

void USinAttributeProgression::ExperienceAdd(float Amount)
{
	SetAbilityScorePoints(Experience.GetCurrentValue() + Amount);
}

void USinAttributeProgression::AbilityPointsAdd(float Amount)
{
	SetAbilityScorePoints(AbilityScorePoints.GetCurrentValue() + Amount);
}

void USinAttributeProgression::AbilityPointsReset()
{
	SetAbilityScorePoints(0.0f);
}

void USinAttributeProgression::AbilityPointsCommit(float Points)
{
	SetAbilityScorePoints(FMath::Max(AbilityScorePoints.GetCurrentValue()-Points, 0));
}

USinAttributeProgression::USinAttributeProgression()
{
}

void USinAttributeProgression::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetLevelAttribute())
	{
		float Difference = NewValue - Level.GetCurrentValue();
		if (Difference > 0.0f)
		{
			SetAbilityScorePoints(AbilityScorePoints.GetCurrentValue()+Difference * 3);
		}
	}
}

void USinAttributeProgression::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinAttributeProgression, Level);
	DOREPLIFETIME(USinAttributeProgression, Experience);
	DOREPLIFETIME(USinAttributeProgression, AbilityScorePoints);
	DOREPLIFETIME(USinAttributeProgression, SpentAbilityScorePoints);
}

