// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Misc/SinGPTs.h"

#include "SinButton.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSinButtonClicked, USinButton*, ButtonSpecial);
/**
 * 
 */
UCLASS()
class SIN_API USinButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* SinButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinButton", meta = (ExposeOnSpawn = "true"))
	FName ButtonID = FName(TEXT("ID"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinButton", meta = (ExposeOnSpawn = "true"))
	FGameplayTag ButtonTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinButton", meta = (ExposeOnSpawn = "true"))
	int32 ButtonIndex = 0;

	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnSinButtonClicked OnSinButtonClicked;

	UFUNCTION()
	void SinButtonBind();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SinButton")
	void LockSinButton(bool Lock);
	virtual void LockSinButton_Implementation(bool Lock);
	/** Sets the label for the button. Can be overridden in Blueprints. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SinButton")
	void SetSinButtonLabel(const FText& Label);
	virtual void SetSinButtonLabel_Implementation(const FText& Label);

protected:

	void NativeOnInitialized() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SinButton", meta = (ExposeOnSpawn = "true"))
	TSoftObjectPtr<USoundBase> OnClickSound;
};
