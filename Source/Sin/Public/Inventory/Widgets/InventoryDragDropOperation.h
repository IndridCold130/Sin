#pragma once

#include "Input/DragAndDrop.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"

class UInventorySlot;

UCLASS()
class SIN_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UInventorySlot> SourceSlot;

	UPROPERTY(BlueprintReadWrite)
	FGuid EntryId;

	UPROPERTY(BlueprintReadWrite)
	FGuid SourceContainerId;
	
	UPROPERTY(BlueprintReadWrite, Category="Inventory|DragDrop")
	bool bWantsStackSplit = false;
};
