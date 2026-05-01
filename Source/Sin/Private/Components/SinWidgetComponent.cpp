// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SinWidgetComponent.h"
#include "SinPlayerController.h"
#include "SinCharacter.h"

USinWidgetComponent::USinWidgetComponent()
{
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	Space = EWidgetSpace::Screen;
	WidgetClass = USinHoverStatus::StaticClass();
	bDrawAtDesiredSize = true;

}

void USinWidgetComponent::ToggleStatusBarVisibility(bool bDamaged, bool bLockOn)
{
	if (bDamaged)
	{
		if (!IsVisible())
		{
			SetVisibility(true);
			SinHover->LastDamageEvent(ToggledByDamage ? HealthWidgetOwner->LastDamageAmount : 0.0f);
			ToggledByDamage = false;
		}
		if (bLockedOn)
		{
			return;
		}
		GetWorld()->GetTimerManager().ClearTimer(VisibilityTimer);
		GetWorld()->GetTimerManager().SetTimer(VisibilityTimer, this, &USinWidgetComponent::StatusBarTimer, 3.5f, false);
		return;
	}

	// Toggle Toggle

	ToggledByDamage = bDamaged;
	bLockedOn = bLockOn;
	if (!IsVisible())
	{
		SinHover->LastDamageEvent(ToggledByDamage ? HealthWidgetOwner->LastDamageAmount : 0.0f);
		SetVisibility(true);
		if (bLockOn)
		{
			return;
		}
		GetWorld()->GetTimerManager().SetTimer(VisibilityTimer, this, &USinWidgetComponent::StatusBarTimer, 3.5f, false);
		return;
	}
	if (bLockOn)
	{
		GetWorld()->GetTimerManager().ClearTimer(VisibilityTimer);
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(VisibilityTimer);
	GetWorld()->GetTimerManager().SetTimer(VisibilityTimer, this, &USinWidgetComponent::StatusBarTimer, 3.5f, false);
	return;
}

void USinWidgetComponent::StatusBarTimer()
{
	SetVisibility(false);
}

void USinWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	AbilityComponent = Cast <USinASC>(GetOwner()->GetComponentByClass(USinASC::StaticClass()));
	if (GetNetMode() != NM_DedicatedServer)
	{
		SinHover = Cast< USinHoverStatus>(GetWidget());
		SinHover->AbilityComponent = AbilityComponent;
		HealthWidgetOwner = Cast<ASinCharacter>(AbilityComponent->GetOwner());
		if (HealthWidgetOwner)
		{
			HealthWidgetOwner ->OnSignalHitTarget.AddDynamic(this, &ThisClass::OnDamageDone);
		}
	}
}

void USinWidgetComponent::OnDamageDone()
{
	if(!HealthWidgetOwner->IsLocallyControlled()||!HealthWidgetOwner->IsPlayerControlled())
	{
		ToggledByDamage = true;
		ToggleStatusBarVisibility(true, bLockedOn);
	}
}
