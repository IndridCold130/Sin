// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "SinAttributeProgression.generated.h"

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
class SIN_API USinAttributeProgression : public UAttributeSet
{
	GENERATED_BODY()

public:
	USinAttributeProgression();
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	//virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_Level);
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(USinAttributeProgression, Level);
	UFUNCTION()
	virtual void OnRep_Level(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_Experience);
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(USinAttributeProgression, Experience);
	UFUNCTION()
	virtual void OnRep_Experience(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_AbilityScorePoints);
	FGameplayAttributeData AbilityScorePoints;
	ATTRIBUTE_ACCESSORS(USinAttributeProgression, AbilityScorePoints);
	UFUNCTION()
	virtual void OnRep_AbilityScorePoints(FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_SpentAbilityScorePoints);
	FGameplayAttributeData SpentAbilityScorePoints;
	ATTRIBUTE_ACCESSORS(USinAttributeProgression, SpentAbilityScorePoints);
	UFUNCTION()
	virtual void OnRep_SpentAbilityScorePoints(FGameplayAttributeData& OldValue);

	UFUNCTION(BlueprintCallable, Category = "GAS_Progression")
	void ExperienceAdd(float Amount);
	UFUNCTION(BlueprintCallable, Category = "GAS_Progression")
	void AbilityPointsAdd(float Amount);
	UFUNCTION(BlueprintCallable, Category = "GAS_Progression")
	void AbilityPointsReset();
	UFUNCTION(BlueprintCallable, Category = "GAS_Progression")
	void AbilityPointsCommit(float Points);
	
};
