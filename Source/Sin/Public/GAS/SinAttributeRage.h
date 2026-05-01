// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/SinASC.h"

#include "SinAttributeRage.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class SIN_API USinAttributeRage : public UAttributeSet
{
	GENERATED_BODY()

public:
	USinAttributeRage();
	UFUNCTION()
	virtual void HandleDamageDone(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AActor* Victim, AActor* DamageCauser);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(BlueprintReadOnly, Category = "Rage", ReplicatedUsing = OnRep_Rage)
	FGameplayAttributeData Rage;
	ATTRIBUTE_ACCESSORS(USinAttributeRage, Rage)

	UPROPERTY(BlueprintReadOnly, Category = "Rage", ReplicatedUsing = OnRep_RageMax)
	FGameplayAttributeData RageMax;
	ATTRIBUTE_ACCESSORS(USinAttributeRage, RageMax)
	
protected:
	//void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	UFUNCTION()
	virtual void OnRep_Rage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_RageMax(const FGameplayAttributeData& OldValue);
	void AdjustRage(float Additive);
private:
	// Timer handle
	FTimerHandle RageHandle;
public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
};
