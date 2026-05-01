// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SinGameplayAbility.h"

#include "GAS/SinASC.h"

void USinGameplayAbility::UpdateAbilityLevel_Implementation(int32 NewLevel)
{
	FGameplayAbilitySpec* Spec = GetAbilitySpec();
	if (Spec) { DeltaLevel = Spec->Level;  Spec->Level = NewLevel; }
}

FGameplayAbilitySpec* USinGameplayAbility::GetAbilitySpec()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return nullptr;
	return ASC->FindAbilitySpecFromClass(GetClass());
}

USinGameplayAbility::USinGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

ASinCharacter* USinGameplayAbility::GetSinOwner()
{
	if (SinOwner)
	{
		return SinOwner;
	}
	return SinOwner = Cast<ASinCharacter>(GetOwningActorFromActorInfo());
}

void USinGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SinASC = Cast<USinASC>(ActorInfo->AbilitySystemComponent);
	if (!SinASC)
	{
		K2_EndAbility();
	}
	FGameplayAbilitySpec* Spec = GetAbilitySpec(); if (Spec) { DeltaLevel = Spec->Level; StoredLevel = Spec->InputID; }
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
