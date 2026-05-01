// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Misc/SinGPTs.h"
#include "SinGlobalStructs.h"
#include "I_CombatInterface.generated.h"

class ASinCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_CombatInterface : public UInterface
{
	GENERATED_BODY()
		
};

/**
 *
 */
class SIN_API II_CombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Identity")
		void GetCharacterIdentity(FGameplayTagContainer& Identity);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Identity")
		void AddIdentityTag(FGameplayTag Tag, bool Add);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Identity")
		void GetAppearance(FSinCharAppearance& Appearance, bool& Found);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool AdjustMovementSpeedMult(EAttributeSource Source, bool Apply, float Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		FRotator GetCharacterLookRotation(FRotator Rotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool I_IsInState(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool I_CanEnterState(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool IsInSubstateCall(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool EnterSubstateCall(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool I_CanEnterSubstate(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void HaltMovement(bool Stop);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		ASinCharacter* GetCurrentTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void ToggleAttack(bool Toggle, EAttackType AttackType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void HandleDeath(AController* Killer, AActor* Tool);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stats")
		void GrantExperienceCall(float XP, AActor* Killed);


};
