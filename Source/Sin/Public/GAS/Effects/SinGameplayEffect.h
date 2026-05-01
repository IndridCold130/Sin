// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GAS/SinASC.h"
#include "GameplayEffectUIData_TextOnly.h"

#include "SinGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, int32> SourceRestrictions;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayEffect")
	void CanApplyEffect(USinASC* ASC, FGameplayTag Source, int32 WantedStacks, bool& NewApplication, int32& Stacks) const;
	virtual void CanApplyEffect_Implementation(USinASC* ASC, FGameplayTag Source, int32 WantedStacks, bool& NewApplication, int32& Stacks) const;
	USinGameplayEffect(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayEffect")
	void SinGameplayEffectInitialized(USinASC* AbilitySystem, FActiveGameplayEffectHandle Handle) const;
	virtual void SinGameplayEffectInitialized_Implementation(USinASC* AbilitySystem, FActiveGameplayEffectHandle Handle) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayEffect")
	void SinGameplayEffectEnd(USinASC* AbilitySystem, FActiveGameplayEffectHandle Handle, int32 Stacks =-1) const;
	virtual void SinGameplayEffectEnd_Implementation(USinASC* AbilitySystem, FActiveGameplayEffectHandle Handle, int32 Stacks = -1) const;

private:
	//virtual class UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;
	//virtual void OnGameplayEffectChanged() override;
	//virtual void PostLoad() override;
};

UCLASS()
class SIN_API USinClassBase : public USinGameplayEffect
{
	GENERATED_BODY()
public:
	USinClassBase(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayEffect")
	void LevelUp(const FActiveGameplayEffectHandle& Handle, USinASC* ASC);
	virtual void LevelUp_Implementation(const FActiveGameplayEffectHandle& Handle, USinASC* ASC);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG_Class|Granted")
	TSubclassOf<UAttributeSet> ClassAttribute;
	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	FGameplayTag GetClassTag();
};

UCLASS(Blueprintable)
class USinGameplayEffectUIData : public UGameplayEffectUIData_TextOnly
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText EffectDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UTexture2D* EffectIcon;
};
