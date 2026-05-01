// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/SinHUD_Indicators.h"
#include "GAS/SinASC.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void USinHUD_Indicators::OnBuffAdded_Implementation(UAbilitySystemComponent* AbilitySystem, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	ASC->OnGameplayEffectStackChangeDelegate(Handle)->AddUObject(this, &USinHUD_Indicators::OnActiveGameplayEffectStackChanged);
	ASC->OnGameplayEffectTimeChangeDelegate(Handle)->AddUObject(this, &USinHUD_Indicators::OnActiveGameplayEffectTimeChanged);
}

void USinHUD_Indicators::OnBuffRemoved_Implementation(FActiveGameplayEffectHandle Handle)
{
}

void USinHUD_Indicators::OnActiveGameplayEffectTimeChanged_Implementation(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration)
{
}

void USinHUD_Indicators::OnActiveGameplayEffectStackChanged_Implementation(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount)
{
}

void USinHUD_Indicators::OnGameplayAbilityGiven_Implementation(const FGameplayAbilitySpec& AbilitySpec, UGameplayAbility* AbilityInstance, bool bAdded)
{
}

void USinHUD_Indicators::OnAbilityLevelUpdated_Implementation(UGameplayAbility* Ability, int32 NewLevel, int32 OldLevel)
{
}

void USinHUD_Indicators::OnBuffRemovedInternal(const FActiveGameplayEffect& Effect)
{
	OnBuffRemoved(Effect.Handle);
}

void USinHUD_Indicators::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ASC = Cast<USinASC>(GetOwningPlayerPawn()->GetComponentByClass(USinASC::StaticClass()));
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &USinHUD_Indicators::OnBuffAdded);
	ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &USinHUD_Indicators::OnBuffRemovedInternal);
	ASC->OnAbilityGiven.AddDynamic(this, &USinHUD_Indicators::OnGameplayAbilityGiven);
	ASC->OnAbilityLevelChanged.AddDynamic(this, &USinHUD_Indicators::OnAbilityLevelUpdated);
}
