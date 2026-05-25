#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "InventoryContainerButton.generated.h"

class UButton;
class UTextBlock;
class UInventoryPanel;

UCLASS()
class SIN_API UInventoryContainerButton : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void InitContainerButton(UInventoryPanel* InOwnerPanel, const FGuid& InContainerId, FGameplayTag InContainerTag, const FText& InDisplayName);

protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Container;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_ContainerName;

	UPROPERTY()
	TObjectPtr<UInventoryPanel> OwnerPanel;

	UPROPERTY(BlueprintReadOnly, Category="Container")
	FGameplayTag ContainerTag;
	
public:	
	UPROPERTY(BlueprintReadOnly, Category="Container")
	FGuid ContainerId;
	
	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	FText DisplayName;

	UFUNCTION()
	void HandleButtonClicked();
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	bool bSelected = false;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory")
	void BP_OnSelectedChanged(bool bInSelected);
	
	UFUNCTION(BlueprintPure, Category="Inventory|DragDrop")
	bool CanAcceptDragOperation(UDragDropOperation* Operation) const;
	
protected:
	
	virtual void NativeOnDragEnter(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation) override;

	virtual void NativeOnDragLeave(
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
};