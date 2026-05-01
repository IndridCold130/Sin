// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Misc/SinGPTs.h"
#include "SinWeaponInterface.generated.h"

class ASinCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USinWeaponInterface : public UInterface
{
	GENERATED_BODY()

};

/**
 * 
 */
class SIN_API ISinWeaponInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FX")
		void CollisionFX(AActor* Victim, FHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void AbsoluteWeaponDamageCall(ASinCharacter* WeaponOwner, FCharStats& Stats);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		float GetKnockbackPower();
};
