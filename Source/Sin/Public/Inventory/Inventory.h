// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/GameItemBase.h"
#include "Inventory/GameItemEquipment.h"
#include "Inventory/GameItemWeapon.h"
#include "SinGlobalStructs.h"
#include "Misc/SinGPTs.h"
#include "Interfaces/I_Inventory.h"
#include "EMSCompSaveInterface.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSignaltemShiftedSignature, UInventory*, NewInventory, int32, Index, UGameItemBase*, Item, int32, SrcIndex, UInventory*, SrcInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWalletUpdatedSignature, const FCharStats&, NewValue, const FCharStats&, DeltaValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryRefreshedSignature, UInventory*, Inventory);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIN_API UInventory : public UActorComponent, public II_Inventory, public IEMSCompSaveInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();
	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	// CORE FUNCTIONS
	UFUNCTION(BlueprintPure, Category = "Inventory")
		void FindFirstFreeSlot(bool& Found, int32& FoundIndex, int32 StartingIndex = 0);
	// Server, Reliable,

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool IsValidID(FName ID, EPrimaryItemType Type);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		int32 InventoryItemCount();

		UFUNCTION(BlueprintPure, Category = "Inventory")
		UGameItemBase* FindFirstItemByTag(FGameplayTag Tag, bool Exact = false);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		TArray<int32> GetAllActiveIndexes();

	UFUNCTION(BlueprintPure, Category = "Inventory")
		bool HasSpace();

	UFUNCTION(BlueprintPure, Category = "Inventory")
		virtual bool HasWallet();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual bool StoreCurrency(FCharStat Currency);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetInventoryAbsoluteWeight();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 TransferAllTo(UInventory* DestinationInventory);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
		void CreateItemPRC(FName ID, EPrimaryItemType Type, int32 Quantity, int32 Index);
		void CreateItemPRC_Implementation(FName ID, EPrimaryItemType Type, int32 Quantity, int32 Index);
		// Function to set the variable with custom logic
	UFUNCTION(BlueprintPure, Category = "Currency")
		virtual bool CanAfford(FCharStat BaseCost);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Currency")
		void GiveMoney(FCharStat Currency);
		virtual void GiveMoney_Implementation(FCharStat Currency);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Currency")
		void AttemptMerchantTransaction(UGameItemBase* Item);
		virtual void AttemptMerchantTransaction_Implementation(UGameItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void CreateItem(FName ID, EPrimaryItemType Type, int32 Quantity, bool& Created, UGameItemBase*& CreatedItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void TryAddItem(UGameItemBase* Item, int32 TargetIndex, bool& Success, int32 SrcIndex = -1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void AddItem(UGameItemBase* Item, int32 TargetIndex, UInventory* SrcInventory, int32 OldIndex);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveItem(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void StackItem(UGameItemBase* IncomingItem, UGameItemBase* TargetItem, int32 MaxStack);
	UFUNCTION(BlueprintCallable,Server, Reliable, WithValidation, Category = "Inventory")
		void MoveItemDeux(UGameItemBase* Item, int32 DstIndex);
	UFUNCTION(BlueprintPure, Category = "Inventory")
		virtual bool SlotRules(UGameItemBase* Item, int32 Slot);
		//bool SlotRules_Implementation(UGameItemBase* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void HasItemAtIndex(int32 Index, bool& HasItem, UGameItemBase*& Item);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Inventory")
		virtual void HandleClient(bool Added, int32 NewIndex, UGameItemBase* Item,int32 OldIndex, UInventory* SrcInventory);

	// DELEGATES

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Inventory)
		FSignaltemShiftedSignature OnSignalItemAdded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Inventory)
		FSignaltemShiftedSignature OnSignalItemRemoved;

	// CORE VARS

	UPROPERTY(BlueprintReadOnly)
		TMap<EPrimaryItemType, class UDataTable*> ItemDataTables;

#define LOCTEXT_NAMESPACE "SinNamespace"
	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FText InventoryName = LOCTEXT("InventoryKey", "Inventory");
#undef LOCTEXT_NAMESPACE

	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 InventorySize = 40;

	UPROPERTY(SaveGame, ReplicatedUsing = OnRep_Container, EditAnywhere, BlueprintReadWrite)
		TArray<UGameItemBase*> Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FGameplayTag InventoryType = TAG_Item;

	UFUNCTION()
		void OnRep_Container();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
	bool CanModifyInventoryV2();

public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
		TObjectPtr<UDataTable> BasicItems;
		//TSoftObjectPtr<UDataTable> BasicItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
		TObjectPtr<UDataTable> EquipmentDT;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
		TObjectPtr<UDataTable> WeaponsData;
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIN_API USinInventory : public UInventory
{
	GENERATED_BODY()

public:
	USinInventory();

	UPROPERTY(SaveGame, ReplicatedUsing = OnRep_Wallet, EditAnywhere, BlueprintReadWrite, Category = "Currency")
		FCharStats SinWallet;
	
		virtual bool HasWallet() override;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Inventory)
		FWalletUpdatedSignature OnWalletUpdated;

	UFUNCTION()
		void OnRep_Wallet(const FCharStats& DeltaValue)
		{
			this->OnWalletUpdated.Broadcast(SinWallet, DeltaValue);
		}
		
		virtual bool StoreCurrency(FCharStat Currency) override;
		virtual bool CanAfford(FCharStat BaseCost) override;
		virtual void GiveMoney_Implementation(FCharStat Currency) override;
		virtual void AttemptMerchantTransaction_Implementation(UGameItemBase* Item) override;
};


// MERCHANT INTERFACE

UINTERFACE(MinimalAPI)
class UMerchant : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SIN_API IMerchant
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};