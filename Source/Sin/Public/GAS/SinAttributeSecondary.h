// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "SinAttributeSecondary.generated.h"

// macros
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class SIN_API USinAttributeSecondary : public UAttributeSet
{
	GENERATED_BODY()

public:
	USinAttributeSecondary();

	UPROPERTY(BlueprintReadOnly, Category = "SecondaryAttributes", ReplicatedUsing = OnRep_MovementSpeed);
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(USinAttributeSecondary, MovementSpeed);
	UFUNCTION()
	virtual void OnRep_MovementSpeed(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "SecondaryAttributes", ReplicatedUsing = OnRep_AttackSpeed);
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(USinAttributeSecondary, AttackSpeed);
	UFUNCTION()
	virtual void OnRep_AttackSpeed(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "SecondaryAttributes", ReplicatedUsing = OnRep_Toughness);
	FGameplayAttributeData Toughness;
	ATTRIBUTE_ACCESSORS(USinAttributeSecondary, Toughness);
	UFUNCTION()
	virtual void OnRep_Toughness(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "SecondaryAttributes", ReplicatedUsing = OnRep_Avoidance);
	FGameplayAttributeData Avoidance;
	ATTRIBUTE_ACCESSORS(USinAttributeSecondary, Avoidance);
	UFUNCTION()
	virtual void OnRep_Avoidance(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "SecondaryAttributes", ReplicatedUsing = OnRep_Resilience);
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(USinAttributeSecondary, Resilience);
	UFUNCTION()
	virtual void OnRep_Resilience(FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
