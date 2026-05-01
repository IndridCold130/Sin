// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SinCharacter.h"
#include "SinGameplayAbility.generated.h"

class USinASC;

/**
 * 
 */
UENUM(BlueprintType)
enum class ELyraAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

UCLASS()
class SIN_API USinGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class USinASC;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Passive_Abilities")
	void UpdateAbilityLevel(int32 NewLevel);
	virtual void UpdateAbilityLevel_Implementation(int32 NewLevel);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USinASC> SinASC;
	/** Gets the spec for this ability from the owner's ASC */
	FGameplayAbilitySpec* GetAbilitySpec();
	UPROPERTY(BlueprintReadOnly, Category = "Ability Level")
	int32 DeltaLevel = 1;
	/** The max level this ability can reach */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Level")
	int32 MaxLevel = 20;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Level")
	int32 StoredLevel = 1;
	USinGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintPure)
	ASinCharacter* GetSinOwner();

protected:

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sin|Ability Activation")
	ELyraAbilityActivationPolicy ActivationPolicy;

	ASinCharacter* SinOwner;

	/** SniggedySnigets */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};

UCLASS()
class SIN_API USinPassiveAbility : public USinGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	TSoftObjectPtr <UTexture> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool bShowInHUD = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool bShowUnderStatus = false;
};
