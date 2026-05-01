// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "SinInputUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()
public:
	//~ Begin UEnhancedInputUserSettings interface
	virtual void ApplySettings() override;
	
};
