// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/SinGameplayEffect.h"
#include "Misc/SinLibrary.h"

USinGameplayEffect::USinGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USinGameplayEffect::PostInitProperties()
{
	Super::PostInitProperties();
}

void USinGameplayEffect::SinGameplayEffectInitialized_Implementation(USinASC* AbilitySystem, FActiveGameplayEffectHandle Handle) const
{
}

void USinGameplayEffect::SinGameplayEffectEnd_Implementation(USinASC* AbilitySystem, FActiveGameplayEffectHandle Handle, int32 Stacks) const
{
	AbilitySystem->RemoveActiveGameplayEffect(Handle, Stacks);
}

void USinGameplayEffect::CanApplyEffect_Implementation(USinASC* ASC, FGameplayTag Source, int32 WantedStacks, bool& NewApplication, int32& Stacks) const
{
	// BOILER PLATE
	if (!ASC) { return; } NewApplication = false; Stacks = WantedStacks; if (SourceRestrictions.IsEmpty()) { return; };
	// BOILER PLATE
	ASC->BuffManager;
}

/// U SIN CLASS
USinClassBase::USinClassBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USinClassBase::LevelUp_Implementation(const FActiveGameplayEffectHandle& Handle, USinASC* ASC)
{
	int LocalStack = ASC->GetCurrentStackCount(Handle);
	if (LocalStack == 1)
	{
		if (ClassAttribute)
		{
			ASC->AddNewAttribute(ClassAttribute, true);
		}
	}
}

FGameplayTag USinClassBase::GetClassTag()
{
	FGameplayTag ClassTag;
	FGameplayTagContainer OwnedTags = CachedAssetTags;

	for (const FGameplayTag& Tag : OwnedTags)
	{
		if (Tag.MatchesTag(TAG_Class) && Tag != TAG_Class)
		{
			return Tag; // Return the first subclass found
		}
	}

	return FGameplayTag(); // Return invalid if none found
}
