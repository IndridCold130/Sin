// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "SinCharacter.h"
#include "SinBelt.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSignaltemEquipSignature, bool, Equipped, UInventory*, NewInventory, int32, Index, UGameItemBase*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSignalItemActivateSignature, bool, Activate, UGameItemBase*, Item);
/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIN_API USinBelt : public UInventory
{
	GENERATED_BODY()

public:
	USinBelt();
	bool SlotRules(UGameItemBase* Item, int32 Slot) override;

	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite);
	int32 ActiveItemIndex = 0;

	UFUNCTION(BlueprintCallable)
		int32 AttemptSwapActiveIndex();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void AttemptSwapActiveIndexPRC(int32 NewIndex);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void AttemptSwapActiveIndexMulti(int32 NewIndex);

	UFUNCTION()
		void HandleActiveItem(bool Added, int32 NewIndex, UGameItemBase* Item,int32 OldIndex, UInventory* SrcInventory);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = QuickSlots)
		FSignaltemEquipSignature OnSignalQuickSlotEquip;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = QuickSlots)
		FSignalItemActivateSignature OnItemActivated;

	UFUNCTION(Category = "Gear")
		void ItemAdded(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory);

	UFUNCTION(Category = "Gear")
		void ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory);

		UFUNCTION(Category = "Gear")
		void ActivateBelt(bool Activate);

		UPROPERTY(SaveGame, Replicated, BlueprintReadOnly, Category = "Gear")
		bool Activated;

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	TObjectPtr<USinASC> ASC;
	virtual void HandleClient_Implementation(bool Added, int32 NewIndex, UGameItemBase* Item, int32 OldIndex, UInventory* SrcInventory) override;
};
