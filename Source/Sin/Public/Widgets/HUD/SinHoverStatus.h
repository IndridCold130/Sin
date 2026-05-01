// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/SinASC.h"
#include "SinHoverStatus.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinHoverStatus : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HealthIndicator")
		void Health(float Value, float DeltaValue);
		virtual void Health_Implementation(float Value, float DeltaValue);

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<USinASC> AbilityComponent;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HealthIndicator")
		void LastDamageEvent(float Damage);
		virtual void LastDamageEvent_Implementation(float Damage);
};
