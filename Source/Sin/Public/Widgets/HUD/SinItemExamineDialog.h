// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SinItemExamineDialog.generated.h"

class USinItemContextMenu;
class URichTextBlock;
class UImage;
class USinItemFragment_Display;

/**
 * 
 */
UCLASS()

class SIN_API USinItemDialogue : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void InitFromContextMenu(USinItemContextMenu* ContextMenu);
	
	UPROPERTY(BlueprintReadOnly, Category="Item Dialog")
	TObjectPtr<const USinItemFragment_Display> CachedDisplayFragment;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_ItemIcon;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_Title;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rich Text")
	FName TitleStyleName = "Examine.Title";
	
protected:

	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;
	
	virtual FReply NativeOnPreviewKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;
};

UCLASS()

class SIN_API USinItemExamineDialog : public USinItemDialogue
{
	GENERATED_BODY()
	
public:
	
	virtual void InitFromContextMenu(USinItemContextMenu* ContextMenu) override;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rich Text")
	FName DescriptionStyleName = "Examine.Body";
};
