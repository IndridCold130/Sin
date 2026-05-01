// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SinGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintNativeEvent)
		void InstanceBeginPlay();
		virtual void InstanceBeginPlay_Implementation();
};

