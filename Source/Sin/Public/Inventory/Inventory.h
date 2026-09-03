// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/GameItemBase.h"
#include "Inventory/GameItemEquipment.h"
#include "Inventory/GameItemWeapon.h"
#include "Inventory/Items/SinItemFragment_Inventory.h"
#include "SinGlobalStructs.h"
#include "Misc/SinGPTs.h"
#include "Interfaces/I_Inventory.h"
#include "EMSCompSaveInterface.h"
#include "GameplayTagContainer.h"
#include "Inventory.generated.h"

struct FSinInventoryEntry;
struct FSinInventoryContainerState;
class USinInventoryContainerSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSignaltemShiftedSignature, UInventory*, NewInventory, int32, Index, UGameItemBase*, Item, int32, SrcIndex, UInventory*, SrcInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWalletUpdatedSignature, const FCharStats&, NewValue, const FCharStats&, DeltaValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryRefreshedSignature, UInventory*, Inventory);

// NEW INVENTORY SYSTEM
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnSinInventoryContainerChanged,
	UInventory*, Inventory,
	FGuid, ContainerId
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnSinInventoryEntryAdded,
	UInventory*, Inventory,
	const FSinInventoryEntry&, Entry
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnSinInventoryEntryRemoved,
	UInventory*, Inventory,
	const FSinInventoryEntry&, RemovedEntry
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnSinInventoryEntryChanged,
	UInventory*, Inventory,
	const FSinInventoryEntry&, OldEntry,
	const FSinInventoryEntry&, NewEntry
);
// NEW INVENTORY SYSTEM

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
	
	//NEW SYSTEM
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool CreateInventoryEntry(USinItemDefinition* ItemDefinition, FSinInventoryEntry& OutEntry, int32 StackCount = 1) const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool AddItemToInventory(USinItemDefinition* ItemDefinition, int32 StackCount = 1);
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool IsContainerFull(const FGuid& ContainerId) const;
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool FindFirstFreeSlotV2(const FGuid& ContainerId,int32& OutSlotIndex,USinItemDefinition* ItemDefinition = nullptr) const;
	
	UFUNCTION(Category="Inventory|New")
	bool FindBestSlotForEntry(const FGuid& ContainerId, const FSinInventoryEntry& SourceEntry, int32& OutSlotIndex, bool bAllowSwap = false) const;
	
	UFUNCTION(Category="Inventory|New")
	int32 PourEntryIntoContainer(const FGuid& EntryId, const FGuid& TargetContainerId);
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool DoesContainerAcceptItem(const FGuid& ContainerId, USinItemDefinition* ItemDefinition) const;
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool FindBestContainerForItem(USinItemDefinition* ItemDefinition, FGuid& OutContainerId) const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool AddEntryToContainer(const FSinInventoryEntry& Entry, const FGuid& PreferredContainerId);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	int32 FindEntryIndexById(const FGuid& EntryId) const;
	
	FSinInventoryEntry* FindEntryById(const FGuid& EntryId);
	
	const FSinInventoryEntry* FindEntryById(const FGuid& EntryId) const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool TransferEntryToInventory(const FGuid& EntryId,UInventory* TargetInventory,const FGuid& TargetContainerId,int32 TargetSlotIndex = -1);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	void EnsureContainerIds();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Setup")
	TObjectPtr<USinInventoryContainerSet> ContainerSet;
	
	UFUNCTION()
	void InitializeInventoryRuntimeState();
	
	
	const FSinInventoryContainerState* FindContainerStateByTag(FGameplayTag ContainerTag) const;
	
	FSinInventoryContainerState* FindMutableContainerStateByTag(FGameplayTag ContainerTag);
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool FindContainerStateByTagBP(FGameplayTag ContainerTag, FSinInventoryContainerState& OutContainerState) const;
	//NEW SYSTEM

	// DELEGATES

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Inventory)
		FSignaltemShiftedSignature OnSignalItemAdded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = Inventory)
		FSignaltemShiftedSignature OnSignalItemRemoved;
	
	UPROPERTY(BlueprintAssignable, Category="Inventory")
		FInventoryRefreshedSignature OnInventoryRefreshed;
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
		void NotifyInventoryChanged();
	
	// NEW SYSTEM
	UPROPERTY(BlueprintAssignable, Category="Inventory|New")
	FOnSinInventoryContainerChanged OnContainerChanged;

	UPROPERTY(BlueprintAssignable, Category="Inventory|New")
	FOnSinInventoryEntryAdded OnInventoryEntryAdded;

	UPROPERTY(BlueprintAssignable, Category="Inventory|New")
	FOnSinInventoryEntryRemoved OnInventoryEntryRemoved;

	UPROPERTY(BlueprintAssignable, Category="Inventory|New")
	FOnSinInventoryEntryChanged OnInventoryEntryChanged;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool MoveEntryTo(const FGuid& EntryId, const FGuid& TargetContainerId, int32 TargetSlotIndex = -1);
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	int32 FindEntryIndexAtSlot(const FGuid& ContainerId, int32 SlotIndex) const;
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	int32 GetMaxStackSize(USinItemDefinition* ItemDefinition) const;
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool CanStackEntries(const FSinInventoryEntry& A, const FSinInventoryEntry& B) const;
	
	int32 TryStackIntoExistingEntries(USinItemDefinition* ItemDefinition,int32 StackCount,const FGuid& ContainerId);
	
	const FSinInventoryContainerState* FindContainerStateById(const FGuid& ContainerId) const;

	// Blueprint helper
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool GetContainerStateById(const FGuid& ContainerId, FSinInventoryContainerState& OutContainerState) const;
	
	bool PlaceEntryAtSlot(FSinInventoryEntry& IncomingEntry,int32 IncomingSourceIndex,const FGuid& TargetContainerId,int32 TargetSlotIndex,UInventory* SourceInventory);
	
	bool AddEntryDirect(const FSinInventoryEntry& Entry);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool SplitEntryStackToSlot(const FGuid& EntryId,int32 SplitAmount = -1, const FGuid& TargetContainerId = FGuid(), int32 TargetSlotIndex = -1);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool SplitEntryStackToOtherInventory(
	const FGuid& EntryId,
	int32 SplitAmount,
	UInventory* TargetInventory,
	const FGuid& TargetContainerId,
	int32 TargetSlotIndex = -1);
	
	UFUNCTION(BlueprintPure, Category="Inventory|New")
	bool DoesContainerAcceptItemAtSlot(const FGuid& ContainerId, int32 SlotIndex,USinItemDefinition* ItemDefinition) const;
	
	UPROPERTY(BlueprintReadOnly, Category="Inventory|Link")
	TWeakObjectPtr<UInventory> LinkedInventory;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Link")
	void SetLinkedInventory(UInventory* OtherInventory);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Link")
	void ClearLinkedInventory();
	
	UFUNCTION(BlueprintPure, Category="Inventory|Link")
	bool HasLinkedInventory() const { return LinkedInventory.IsValid(); }
	
	UFUNCTION(BlueprintCallable)
	bool TryDoubleClickEntry(const FGuid& EntryId);
	// NEW SYSTEM

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
	
	// NEW INVENTORY SYSTEM
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Containers")
		TArray<FSinInventoryContainerState> Containers;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory|Entries")
		TArray<FSinInventoryEntry> ItemInventory;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool MoveEntryToBestContainer(const FGuid& EntryId);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|New")
	bool EquipEntryToBestSlot(const FGuid& EntryId);
	// NEW INVENTORY SYSTEM
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