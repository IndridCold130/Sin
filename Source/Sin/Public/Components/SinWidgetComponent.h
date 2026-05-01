// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Widgets/HUD/SinHoverStatus.h"
#include "GAS/SinASC.h"
#include "SinWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIN_API USinWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

	USinWidgetComponent();

public:

	UPROPERTY()
		USinHoverStatus* SinHover;

	UFUNCTION(BlueprintCallable)
		void ToggleStatusBarVisibility(bool bDamaged, bool bLockOn);
		void StatusBarTimer();

	UPROPERTY()
	TObjectPtr<USinASC> AbilityComponent;

	UFUNCTION()
		void OnDamageDone();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	TObjectPtr<ASinCharacter> HealthWidgetOwner;

	bool ToggledByDamage = false;
	bool bLockedOn = false;

	UPROPERTY()
		FTimerHandle VisibilityTimer;
};
