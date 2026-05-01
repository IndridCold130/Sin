// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SinButton.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Misc/SinGPTs.h"
#include "Widgets/SinHUD_Widget.h"

#include "SinButtonCluster.generated.h"

class ASinPlayerController;
class USinHUD;
/**
 * 
 */
UCLASS()
class SIN_API USinButtonCluster : public USinMenuBase
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintNativeEvent)
		void GenerateClusterButtons(UDataTable* Table);
		virtual void GenerateClusterButtons_Implementation(UDataTable* Table);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void MakeSelection(USinButton* Selection);
		virtual void MakeSelection_Implementation(USinButton* Selection);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
			TObjectPtr<UPanelWidget> ButtonPanel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonCluster", meta = (ExposeOnSpawn = "true"))
			TSubclassOf<USinButton> ButtonClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonCluster")
			TArray<FName> IDs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonCluster", meta = (ExposeOnSpawn = "true"))
			UDataTable* ID_Table;
	UPROPERTY(BlueprintReadWrite, Category = "ButtonCluster")
			TArray<USinButton*> Buttons;
	UPROPERTY(BlueprintReadWrite, Category = "ButtonCluster")
			USinButton* ActiveButton;
};

USTRUCT(BlueprintType)
struct FSinButtonData
{
	GENERATED_BODY()
	/** The unique identifier for this button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarouselButton")
	FName ButtonID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarouselButton")
	FGameplayTag ButtonTag;
	/** The display text for this button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarouselButton")
	FText ButtonLabel;
	/** Soft class reference to the submenu associated with this button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	TSoftClassPtr<UUserWidget> SubmenuClass;
};
/**
 *
 */
UCLASS()
class SIN_API USinCarousel : public USinMenuBase
{
	GENERATED_BODY()

public:
	/** Called when the widget is constructed */
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	UPROPERTY()
	USinButton* ActiveButton;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	TObjectPtr<USinHUD> HUD_Widget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel | Setup", meta = (ExposeOnSpawn = "true"))
	FGameplayTag ActiveMenuTag = TAG_Menu_Carousel_Inventory;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPanelWidget* RootContainer;
	/** The soft reference to the button widget class. This allows designers to set their custom button class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel | Setup")
	TSoftClassPtr<USinButton> ButtonClass;
	/** Array of button definitions that designers can reorder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel | Setup")
	TArray<FSinButtonData> ButtonDefinitions;
	/** Enum to let the designer choose between a vertical or horizontal box to hold the buttons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel | Setup")
	bool bUseVerticalBox = true;
	/** The dynamically created panel widget (either VerticalBox or HorizontalBox). */
	UPROPERTY(Transient)
	TObjectPtr<UPanelWidget> ButtonContainer;
	/** Called to dynamically create the ButtonContainer (Vertical or Horizontal Box). */
	void CreateButtonContainer();
	/** Helper to dynamically add buttons to the ButtonContainer. */
	void AddButtonsToContainer();
	/** Called to populate the carousel dynamically based on designer setup. */
	UFUNCTION(BlueprintCallable, Category = "Carousel | Setup")
	void PopulateCarousel(int32 AutoSelectIndex = -1);
	/** The alignment of the ButtonContainer within its RootContainer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel | Alignment")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Center;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carousel | Alignment")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Center;
	/** Function to bind each button's OnSinButtonClicked event. */
	void BindButtonEvent(USinButton* Button);
	/** Function to handle a button click event. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleButtonClicked(USinButton* ClickedButton);
	virtual void HandleButtonClicked_Implementation(USinButton* ClickedButton);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SubMenuActivate(bool bActivate, USinButton* Button);
	virtual void SubMenuActivate_Implementation(bool bActivate, USinButton* Button);
	UFUNCTION(BlueprintCallable)
		TSoftClassPtr<UUserWidget> PickSubmenuByTag(FGameplayTag Tag);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UPanelWidget> Content;
	UFUNCTION(BlueprintCallable)
	void RotateSubmenu(bool bRight);
	virtual void SwapSubmenu_Implementation(FGameplayTag Tag) override;
private:
	/** Cache the instantiated buttons for future access if needed. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<USinButton>> ButtonInstances;
	/** Helper function to align ButtonContainer within RootContainer */
	//void SetContainerAlignment(UWidget* Widget);
};