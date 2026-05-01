// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Misc/SinGPTs.h"
#include "SinGlobalStructs.h"
#include "SinPlayerInterface.generated.h"

class ASinCharacter;
class UUserWidget;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USinPlayerInterface : public UInterface
{
	GENERATED_BODY()

};
/**
 * 
 */
class SIN_API ISinPlayerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
		void SinInputMode(UUserWidget* Focus, EInputMode InputMode);

};
