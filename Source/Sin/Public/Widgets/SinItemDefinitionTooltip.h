#pragma once

#include "CoreMinimal.h"
#include "SinGameHUD.h"
#include "SinItemDefinitionTooltip.generated.h"

class USinItemDefinition;
class UTextBlock;
class UImage;

UCLASS()
class SIN_API USinItemDefinitionTooltip : public USinItemTooltip
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Tooltip")
	void ShowItemDefinitionTooltip(UUserWidget* Master, USinItemDefinition* ItemDefinition);

	virtual void ShowItemDefinitionTooltip_Implementation(
		UUserWidget* Master,
		USinItemDefinition* ItemDefinition
	);

protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<USinItemDefinition> Definition;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ShortDescriptionText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> LongDescriptionText;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UImage> IconImage;
};