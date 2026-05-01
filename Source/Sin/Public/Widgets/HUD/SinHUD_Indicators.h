// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SinHUD_Indicators.generated.h"

class USinASC;
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;

//DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectAppliedClientDelegate, UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle);
/**
 * 
 */
UCLASS()
class SIN_API USinHUD_Indicators : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USinASC> ASC;
	//(UAbilitySystemComponent* AbilitySystem, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	UFUNCTION(BlueprintNativeEvent, Category = "GAS")
	void OnBuffAdded(UAbilitySystemComponent* AbilitySystem, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	virtual void OnBuffAdded_Implementation(UAbilitySystemComponent* AbilitySystem, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	UFUNCTION()
	void OnBuffRemovedInternal(const FActiveGameplayEffect& Effect);
	UFUNCTION(BlueprintNativeEvent, Category = "GAS")
	void OnBuffRemoved(FActiveGameplayEffectHandle Handle);
	virtual void OnBuffRemoved_Implementation(FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintNativeEvent, Category = "GAS")
	void OnActiveGameplayEffectTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);
	virtual void OnActiveGameplayEffectTimeChanged_Implementation(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration);
	UFUNCTION(BlueprintNativeEvent, Category = "GAS")
	void OnGameplayAbilityGiven(const FGameplayAbilitySpec& AbilitySpec, UGameplayAbility* AbilityInstance, bool bAdded);
	virtual void OnGameplayAbilityGiven_Implementation(const FGameplayAbilitySpec& AbilitySpec, UGameplayAbility* AbilityInstance, bool bAdded);
	UFUNCTION(BlueprintNativeEvent, Category = "GAS")
	void OnAbilityLevelUpdated(UGameplayAbility* Ability, int32 NewLevel, int32 OldLevel);
	virtual void OnAbilityLevelUpdated_Implementation(UGameplayAbility* Ability, int32 NewLevel, int32 OldLevel);
	UFUNCTION(BlueprintNativeEvent, Category = "GAS")
	void OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount);
	virtual void OnActiveGameplayEffectStackChanged_Implementation(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount);
protected:
	void NativeOnInitialized() override;
	
};
