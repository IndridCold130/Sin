// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "SinAttributeStamina.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class SIN_API USinAttributeStamina : public UAttributeSet
{
	GENERATED_BODY()

public:
	USinAttributeStamina();
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(USinAttributeStamina, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaMax)
	FGameplayAttributeData StaminaMax;
	ATTRIBUTE_ACCESSORS(USinAttributeStamina, StaminaMax)

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegen)
	FGameplayAttributeData StaminaRegen;
	ATTRIBUTE_ACCESSORS(USinAttributeStamina, StaminaRegen)

protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_StaminaMax(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_StaminaRegen(const FGameplayAttributeData& OldValue);
	//*StaminaChecks/
	void StaminaRegenCheck(const FGameplayAttribute& IncomingStamina, float NewValue);
	bool bOutOfStamina;
	
};
