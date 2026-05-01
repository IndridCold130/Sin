// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Misc/SinGPTs.h"
#include "SinGlobalStructs.h"
#include "SinPlayerController.h"
#include "SinCharacter.h"
#include "Widgets/SinHUD_Widget.h"
#include "SinGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API ASinGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void GenerateHUD();
	virtual void GenerateHUD_Implementation();

UPROPERTY()
TObjectPtr<ASinPlayerController> SinController;

UPROPERTY()
TObjectPtr<USinHUD> UHUD;

UPROPERTY(EditAnywhere, BlueprintReadWrite)
TSubclassOf<USinHUD> UHUD_Subclass;

/** Soft reference to the tooltip widget class (set in the editor) */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
TSoftClassPtr<USinItemTooltip> TooltipWidgetClass;

/** The single instance of the tooltip widget */
UPROPERTY(Transient)
USinItemTooltip* ActiveTooltip;

/** Function to create or retrieve the tooltip widget */
UFUNCTION(BlueprintCallable, Category = "UI")
USinItemTooltip* GetOrCreateTooltip();

/** Function to show the tooltip at a specific location */
UFUNCTION(BlueprintCallable, Category = "UI")
void ShowTooltip(bool bShow, UUserWidget* Master, UGameItemBase* GameItem);
};

UCLASS()
class SIN_API USinItemTooltip : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Sets the item name and description in the tooltip */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tooltip")
    void ShowTooltip(UUserWidget* Master, UGameItemBase* GameItem);
    virtual void ShowTooltip_Implementation(UUserWidget* Master, UGameItemBase* GameItem);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;
    /** The text block for the item name */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Label;
};

