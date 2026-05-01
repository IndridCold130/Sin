// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Effects/SinGameplayEffect.h"
#include "Sin_GAS_Buff.generated.h"

class UTexture;
/**
 * 
 */
UCLASS()
class SIN_API USin_GAS_Buff : public USinGameplayEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	TSoftObjectPtr <UTexture> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool bShowIcon = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool StacksAsRanks = false;
	
};
