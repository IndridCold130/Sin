// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SinGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API ASinGameMode : public AGameMode
{
	GENERATED_BODY()

	public:

	/* Override To Read In Pawn From Custom Controller */
		//UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
};
