// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Misc/SinLibrary.h"

#include "SinPhysicalDamageCalculation.generated.h"

class USinASC;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FPhysicalDamageProfile
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	float Slash = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Pierce = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Strike = 0.f;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, float> DamageValues;

	FORCEINLINE float Total() const { return Slash + Pierce + Strike; }
	FORCEINLINE float GetValue(const FGameplayTag& InValue) const { if (const float* Found = DamageValues.Find(InValue)) { return *Found; }return 0.0f; }
	FORCEINLINE void SetValue(const FGameplayTag& InValue, float NewAmount) { DamageValues.FindOrAdd(InValue) = NewAmount; }
};

USTRUCT(BlueprintType)
struct FPhysicalDefenseProfile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Armor = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Slash = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Pierce = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Strike = 0.f;

	FORCEINLINE float TotalMitigationFor(const FPhysicalDamageProfile& Damage) const
	{
		return
			Damage.GetValue(TAG_Attribute_Secondary_DamageType_Physical_Slash)* (1.f - Armor) * (1.f - Slash) +
			//Damage.Slash * (1.f - Armor) * (1.f - Slash) +
			Damage.GetValue(TAG_Attribute_Secondary_DamageType_Physical_Pierce)* (1.f - Armor) * (1.f - Slash) +
			//Damage.Pierce * (1.f - Armor) * (1.f - Pierce) +
			Damage.GetValue(TAG_Attribute_Secondary_DamageType_Physical_Strike)* (1.f - Armor) * (1.f - Slash);
			//Damage.Strike * (1.f - Armor) * (1.f - Strike);
	}
};

USTRUCT(BlueprintType)
struct FBlockResult
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FPhysicalDamageProfile ReducedDamage;

	UPROPERTY(BlueprintReadOnly)
	float StaminaCost = 0.f;

	UPROPERTY()
	bool bPerfectBlock = false; // Reserved for future logic
};

UCLASS()
class SIN_API USinPhysicalDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	// Constructor and overrides
public:
	USinPhysicalDamageCalculation();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool OffHand = false;
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	float CalculateRawPhysicalDamage(USinASC* ASC, const FGameplayEffectSpec& DamageSpec, float& Pierce, float& Slash, float& Strike, float& Penetration) const;
	void CalculateShieldMitigation(USinASC* ASC, const FGameplayEffectSpec& DamageSpec, float& Pierce, float& Slash, float& Strike) const;
	float CalculateMitigatedPhysicalDamage(USinASC* ASC, const FGameplayEffectSpec& DamageSpec, float& Pierce, float& Slash, float& Strike, float& Penetration) const;

	// REBRANDERMENT
	FPhysicalDefenseProfile CalculateDefenses(USinASC* VictimASC, float DefenderAvoidance =1.0f, float ArmorPenetration = 0.f) const;
	FPhysicalDamageProfile CalculateRawDamage(const FGameplayEffectSpec& WeaponSpec, float DamageMod = 1.f) const;
	// SUPPLEMENTAL FUNCIONS
	float CalculateAttackerArP(USinASC* AttackerASC) const;
	float CalculateArmorCoefficient(USinASC* VictimASC) const;
	float CalculateShieldCoefficient(USinASC* VictimASC) const;
	float CalculateAvoidance(USinASC* VictimASC) const;
	float CalculateDamageBonus(USinASC* AttackerASC, EPrimaryAttribute Attribute, float AttributeValue, float AttributeScaler = 1.f) const;
	float CalculateShieldRatingBonus(USinASC* AttackerASC) const;
	float CalculateBlockStaminaFactor(USinASC* VictimASC, float RawDamage) const;
	FBlockResult BlockFilter(USinASC* ASC, const FGameplayEffectSpec& DefenseSpec, FPhysicalDamageProfile RawDamage) const;
	bool IsAttackerInFront(AActor* Attacker, AActor* Defender, float ViewAngleDegrees) const;
};
