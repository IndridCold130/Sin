// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinAttributeHealth.h"

#include "GAS/SinASC.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Misc/SinGPTs.h"
#include "SinCharacter.h"

USinAttributeHealth::USinAttributeHealth()
	: Health(200.f)
	, HealthMax(200.f)
	, Damage(0.f)
{
	bOutOfHealth = false;
}

void USinAttributeHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinAttributeHealth, Health);
	DOREPLIFETIME(USinAttributeHealth, HealthMax);
	DOREPLIFETIME(USinAttributeHealth, HealthRegen);	
	DOREPLIFETIME(USinAttributeHealth, Damage);
}

void USinAttributeHealth::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
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

void USinAttributeHealth::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthMaxAttribute())
	{AdjustAttributeForMaxChange(Health, HealthMax, NewValue, GetHealthAttribute());}
	else if (Attribute == GetHealthAttribute())
	{
		HealthRegenCheck(Attribute, NewValue);
	}
}

bool USinAttributeHealth::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	Super::PreGameplayEffectExecute(Data);
	return true;
}

void USinAttributeHealth::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}
	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ASinCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ASinCharacter>(TargetActor);
	}
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Get the Source actor
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ASinCharacter* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			// Use the controller to find the source pawn
			if (SourceController)
			{
				SourceCharacter = Cast<ASinCharacter>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ASinCharacter>(SourceActor);
			}
			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}
		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0)
		{
			// Apply the health change and then clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetHealthMax()));

			if (TargetCharacter)
			{
				// This is proper damage
				TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);
				// Call for all health changes
				TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
				USinASC* AggressorASC = Cast<USinASC>(SourceActor->GetComponentByClass(USinASC::StaticClass()));
				if (AggressorASC)
				{
					AggressorASC->SignalDamageDone.Broadcast(LocalDamageDone, HitResult, SourceTags, TargetCharacter, SourceActor);
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
		// First clamp it
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetHealthMax()));

		if (TargetCharacter)
		{
			// Call for all health changes
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
}

void USinAttributeHealth::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeHealth, Health, OldValue);
}

void USinAttributeHealth::OnRep_HealthMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeHealth, HealthMax, OldValue);
}

void USinAttributeHealth::OnRep_HealthRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeHealth, HealthRegen, OldValue);
}

void USinAttributeHealth::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USinAttributeHealth, Damage, OldValue);
}

void USinAttributeHealth::HealthRegenCheck(const FGameplayAttribute& IncomingHealth, float NewValue)
{
	// check if health is basically full first //
	if (NewValue >= HealthMax.GetCurrentValue())
	{
		if (!GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Health_Full))
		{
			GetOwningAbilitySystemComponent()->AddLooseGameplayTag(TAG_Health_Full);
		}
		return;
	}
	// TWO CONDITIONS //
	if (NewValue < HealthMax.GetCurrentValue() && GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(TAG_Health_Full))
	{
		GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(TAG_Health_Full);
	}
	if (NewValue < Health.GetCurrentValue())
	{
		USinASC* ZeroASC = Cast<USinASC>(GetOwningAbilitySystemComponent());
		ZeroASC->SustainedHealthDamage(6.f);
	}
}
