// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SinCharacter.h"
#include "SinCharacterCreation.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinCharacterCreation : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
		TObjectPtr<ASinCharacter> SinCharacter;
	
};
