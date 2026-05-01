// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/SinAttributeHealth.h"
#include "AbilitySystemComponent.h"
#include "Misc/SinGPTs.h"

#include "SinAttributePrimary.generated.h"

class USinAttributeSecondary;
class USinAttributeHealth;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class SIN_API USinAttributePrimary : public UAttributeSet
{
	GENERATED_BODY()

public:
	USinAttributePrimary();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	const USinAttributeSecondary* GetSecondaryAttributeSet();
	const USinAttributeHealth* GetHealthAttributeSet();

	// Secondary Stat Calculating Funcs //
	void CalculateHealthBonus(FActiveGameplayEffectHandle Handle);
	void CalculateStaminaBonus(FActiveGameplayEffectHandle Handle);
	void CalculateManaBonus(FActiveGameplayEffectHandle Handle);
	void CalculateToughnessBonus(FActiveGameplayEffectHandle Handle);
	void CalculateAvoidanceBonus(FActiveGameplayEffectHandle Handle);
	void CalculatePoiseBonus(FActiveGameplayEffectHandle Handle);
	void CalculateResilienceBonus(FActiveGameplayEffectHandle Handle);
	// Secondary Stat Calculating Funcs //

	UPROPERTY(BlueprintReadOnly, Category = "PrimaryAttributes", ReplicatedUsing = OnRep_Strength);
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(USinAttributePrimary, Strength);

	UFUNCTION()
	virtual void OnRep_Strength(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "PrimaryAttributes", ReplicatedUsing = OnRep_Dexterity);
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(USinAttributePrimary, Dexterity);

	UFUNCTION()
	virtual void OnRep_Dexterity(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "PrimaryAttributes", ReplicatedUsing = OnRep_Constitution);
	FGameplayAttributeData Constitution;
	ATTRIBUTE_ACCESSORS(USinAttributePrimary, Constitution);

	UFUNCTION()
	virtual void OnRep_Constitution(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "PrimaryAttributes", ReplicatedUsing = OnRep_Sorcery);
	FGameplayAttributeData Sorcery;
	ATTRIBUTE_ACCESSORS(USinAttributePrimary, Sorcery);

	UFUNCTION()
	virtual void OnRep_Sorcery(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "PrimaryAttributes", ReplicatedUsing = OnRep_Faith);
	FGameplayAttributeData Faith;
	ATTRIBUTE_ACCESSORS(USinAttributePrimary, Faith);

	UFUNCTION()
	virtual void OnRep_Faith(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "PrimaryAttributes", ReplicatedUsing = OnRep_Charisma);
	FGameplayAttributeData Charisma;
	ATTRIBUTE_ACCESSORS(USinAttributePrimary, Charisma);

	UFUNCTION()
	virtual void OnRep_Charisma(FGameplayAttributeData& OldValue);

protected:
	UPROPERTY()
	TObjectPtr<USinASC> ASC;
	UFUNCTION()
	USinASC* GetASC();
	
};
