// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIN_API II_Interactable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Focus(ASinPlayerController* Controller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void EndFocus(ASinPlayerController* Controller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Interact(ASinPlayerController* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		UInventory* GetInventory();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		UEquipment* GetEquipment();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FString GetToolTipInfo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FString GetInteractableLabel();
};
