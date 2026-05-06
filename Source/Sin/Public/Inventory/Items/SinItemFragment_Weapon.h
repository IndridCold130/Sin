#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "GAS/SinStatTypes.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundBase.h"
#include "SinItemFragment_Weapon.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_Weapon : public USinItemFragment
{
	GENERATED_BODY()

public:

	// Examples:
	// Weapon.Type.Sword
	// Weapon.Type.Axe
	// Weapon.Type.Staff
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FGameplayTag WeaponType;

	// Examples:
	// Weapon.Size.OneHanded
	// Weapon.Size.TwoHanded
	// Weapon.Size.Light
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FGameplayTag WeaponSize;

	// Raw weapon damage values.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Damage")
	FCharStats Damage;

	// Attribute scaling, e.g. Strength C, Dexterity B.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Scaling")
	TArray<FStatScalingInfo> Scaling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Combat", meta=(ClampMin="0.0"))
	float BlockPower = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Combat", meta=(ClampMin="0.0"))
	float BaseKnockbackPower = 0.0f;

	// Moveset identifiers used by your animation/combat system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Moveset")
	FName MovesetID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Moveset")
	FName MovesetIDTwoHand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Moveset")
	FName MainHandSuffixID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Moveset")
	FName OffHandSuffixID;

	// Optional sheath/socket tag or identifier.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Visual")
	FName SheathID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Audio")
	TSoftObjectPtr<USoundBase> DrawSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Audio")
	TSoftObjectPtr<USoundBase> SheatheSound;
};