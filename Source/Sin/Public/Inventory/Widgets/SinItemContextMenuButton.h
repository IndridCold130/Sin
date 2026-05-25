#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SinItemContextMenuButton.generated.h"

/**
 * 
 */

class URichTextBlock;
class UBorder;

UENUM(BlueprintType)
enum class ESinItemContextAction : uint8
{
	Equip,
	Unequip,
	Split,
	Drop,
	Examine,
	Close
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContextMenuButtonClicked,ESinItemContextAction,Action);

UCLASS()
class SIN_API USinItemContextMenuButton : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable, Category="Context Menu")
	FOnContextMenuButtonClicked OnClickedAction;

	UPROPERTY(BlueprintReadOnly, Category="Context Menu")
	ESinItemContextAction Action;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<URichTextBlock> Text_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_Root;

	UFUNCTION(BlueprintCallable, Category="Context Menu")
	void InitContextButton(FText InLabel,ESinItemContextAction InAction);

protected:

	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void HandleClicked();
};
