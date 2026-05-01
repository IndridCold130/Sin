// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributeRage.h"

//#include "GAS/SinASC.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Misc/SinGPTs.h"
#include "SinCharacter.h"

USinAttributeRage::USinAttributeRage()
	: Rage(0.f)
	, RageMax(100.f)
{
}

void USinAttributeRage::OnRep_Rage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeRage, Rage, OldValue);
}

void USinAttributeRage::OnRep_RageMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeRage, RageMax, OldValue);
}

void USinAttributeRage::AdjustRage(float Additive)
{
	const float OldRage = GetRage();
	SetRage(FMath::Clamp(OldRage + Additive, 0.0f, GetRageMax()));
}

void USinAttributeRage::HandleDamageDone(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AActor* Victim, AActor* DamageCauser)
{
	AdjustRage(6.f);
}

void USinAttributeRage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinAttributeRage, Rage);
	DOREPLIFETIME(USinAttributeRage, RageMax);
}


