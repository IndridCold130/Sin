// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvSlotVisual.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class SIN_API UInvSlotVisual : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* Icon;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void SetItemIconImage(TSoftObjectPtr<UTexture2D> SoftImg);
	virtual void SetItemIconImage_Implementation(TSoftObjectPtr<UTexture2D> SoftImg);

	//UFUNCTION(BlueprintCallable, Category = "Appearance")
	//UMG_API virtual void SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize = false);

protected:
	void NativeOnInitialized() override;
};
