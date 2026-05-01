// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "SinButtonBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialClicked, USinButtonBase*, SinButton);
/**
 * 
 */
UCLASS()
class SIN_API USinButtonBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* SpecialButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonBase", meta = (ExposeOnSpawn = "true"))
		FName ButtonID = FName(TEXT("ID"));

	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FOnSpecialClicked OnSpecialClicked;

	UFUNCTION()
		void SpecialBind();

protected:

	void NativeOnInitialized() override;
	
};
