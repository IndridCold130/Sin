// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/SinEnhancedInput.h"

#include "Containers/Map.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SinEnhancedInput)

USinEnhancedInput::USinEnhancedInput(const FObjectInitializer& ObjectInitializer)
{
}

void USinEnhancedInput::AddInputMappings(const USinEnhancedInput* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULocalPlayer>();
	check(LocalPlayer);
}

void USinEnhancedInput::RemoveInputMappings(const USinEnhancedInput* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
}
