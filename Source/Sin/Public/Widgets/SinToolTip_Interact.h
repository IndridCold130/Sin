 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SinToolTip_Interact.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinToolTip_Interact : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tooltip")
	void GetSinToolTipInfo(const FString& Info);
	virtual void GetSinToolTipInfo_Implementation(const FString& Info);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> LabelText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> PromptText;
};
