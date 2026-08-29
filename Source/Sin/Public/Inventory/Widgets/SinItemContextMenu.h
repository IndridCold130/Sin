// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SinItemContextMenu.generated.h"

class UContentWidget;
class UInventoryPanel;
class UInventorySlot;
class UBorder;
class UCanvasPanel;
class USinItemContextMenuButton;
class UVerticalBox;
class USinItemDefinition;
struct FSinInventoryEntry;
class UInventory;

UCLASS()
class SIN_API USinItemContextMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable, Category="Context Menu")
	void InitContextMenu(UInventoryPanel* InOwnerPanel, UInventorySlot* InSourceSlot);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UContentWidget>ContextBox;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UVerticalBox> ButtonBox;
	
	UPROPERTY(BlueprintReadOnly, Category="Context Menu")
	TObjectPtr<UInventoryPanel> OwnerPanel;

	UPROPERTY(BlueprintReadOnly, Category="Context Menu")
	TObjectPtr<UInventorySlot> SourceSlot;
	
	const FSinInventoryEntry* Entry;
	
	UPROPERTY(BlueprintReadOnly, Category="Context Menu")
	TObjectPtr<USinItemDefinition> ItemDefinition;
	
	UPROPERTY(BlueprintReadOnly, Category="Context Menu")
	TObjectPtr<UInventory> SourceInventory;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Context Menu")
	TSubclassOf<USinItemContextMenuButton> ContextMenuButtonClass;
	//virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry,const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	UFUNCTION(BlueprintCallable, Category="Context Menu")
	void BuildDefaultButtons();
	UFUNCTION(BlueprintCallable, Category="Context Menu")
	void AddContextButton(const FText& Label, ESinItemContextAction Action);
	UFUNCTION()
	void HandleContextButtonClicked(ESinItemContextAction Action);
	bool IsEntryInSpecializedContainer() const;
	bool CanSmartEquipEntry() const;
};
