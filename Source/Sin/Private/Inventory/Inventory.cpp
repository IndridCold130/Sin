// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"

#include "Inventory/SinInventoryContainerTypes.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Net/UnrealNetwork.h"
#include "GAS/SinStatTypes.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;

	for (EPrimaryItemType Source : TEnumRange<EPrimaryItemType>())
	{
		switch(Source)
		{
			case EPrimaryItemType::Regular:
			{
				//UDataTable* LocalItemTable = BasicItems.LoadSynchronous();
				//static ConstructorHelpers::FObjectFinder<UDataTable>GameItemDataObject(TEXT("DataTable'/Game/Systems/Inventory/Tables/DT_Items.DT_Items'"));
				if (BasicItems)
				{
					ItemDataTables.Add(Source, BasicItems);
				}
			}
				break;
			
			case EPrimaryItemType::Equipment:
			{
				if(EquipmentDT)
				{
					ItemDataTables.Add(Source, EquipmentDT);
				}
			}
				break;
			case EPrimaryItemType::Weapon:
			{
				if (WeaponsData)
				{
					ItemDataTables.Add(Source, WeaponsData);
				}
			}
				break;
			default:
				break;
		}
	}
}

bool UInventory::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool ReplicationPrompt = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);


	for (auto& item : Container)
		ReplicationPrompt |= Channel->ReplicateSubobject(item, *Bunch, *RepFlags);
	return ReplicationPrompt;
}

void UInventory::FindFirstFreeSlot(bool& Found, int32& FoundIndex, int32 StartingIndex)
{
	Found = false;
	FoundIndex = INDEX_NONE;

	if (InventorySize <= 0)
	{
		return;
	}

	const int32 ClampedStartingIndex = FMath::Clamp(StartingIndex, 0, InventorySize - 1);

	for (int32 Index = ClampedStartingIndex; Index < InventorySize; ++Index)
	{
		if (!Container.IsValidIndex(Index) || !IsValid(Container[Index]))
		{
			Found = true; FoundIndex = Index; return;
		}
	}

	for (int32 Index = 0; Index < ClampedStartingIndex; ++Index)
	{
		if (!Container.IsValidIndex(Index) || !IsValid(Container[Index]))
		{
			Found = true; FoundIndex = Index; return;
		}
	}
}

bool UInventory::IsValidID(FName ID, EPrimaryItemType Type)
{
	if (ID.IsNone())
	{
		return false;
	}

	UDataTable* const* FoundTable = ItemDataTables.Find(Type);
	if (!FoundTable || !*FoundTable)
	{
		return false;
	}

	FItemBase* LocalRow = (*FoundTable)->FindRow<FItemBase>(ID, TEXT("ContextString"), true);
	return LocalRow != nullptr;
}

int32 UInventory::InventoryItemCount()
{
	int32 Count = 0;
	for (auto It = Container.CreateConstIterator(); It; ++It)
	{
		if(*It!=nullptr)
		{
			Count++;
		}
	}
	return Count;
}

UGameItemBase* UInventory::FindFirstItemByTag(FGameplayTag Tag, bool Exact)
{
	UGameItemBase* LocalItem = nullptr;
	for (auto It = Container.CreateIterator(); It; ++It)
	{
		if (*It != nullptr)
		{
			LocalItem = *It;
			if (Exact)
			{
				if (LocalItem->ItemTags.HasTagExact(Tag))
				{
					return LocalItem;
				}
			}
			else
			{
				if (LocalItem->ItemTags.HasTag(Tag))
				{
					return LocalItem;
				}
			}
		}
	}
	return nullptr;
}

TArray<int32> UInventory::GetAllActiveIndexes()
{
	TArray<int32> FinalArray;
	if (InventoryItemCount() > 0)
	{
		for (auto It = Container.CreateConstIterator(); It; ++It)
		{
			if (*It != nullptr)
			{
				FinalArray.Add(Container.Find(*It));
				//FinalArray.Add(It.GetIndex());
			}
		}
		return FinalArray;
	}
	return FinalArray;
}

bool UInventory::HasSpace()
{
	if (InventoryItemCount() < InventorySize)
	{
		return true;
	}
	return false;
}

bool UInventory::HasWallet()
{
	return false;
}

bool UInventory::StoreCurrency(FCharStat Currency)
{
	return false;
}

bool USinInventory::StoreCurrency(FCharStat Currency)
{
	if (!HasWallet())
	{
		return false;
	}

	if (!Currency.Stat.IsValid() || Currency.Value <= 0.f)
	{
		return false;
	}

	const FCharStats OldWallet = SinWallet;

	for (FCharStat& WalletCurrency : SinWallet.Attributes)
	{
		if (WalletCurrency.Stat == Currency.Stat)
		{
			WalletCurrency.Value += Currency.Value;
			OnRep_Wallet(OldWallet);
			return true;
		}
	}

	SinWallet.Attributes.Add(Currency);
	OnRep_Wallet(OldWallet);

	return true;
}

float UInventory::GetInventoryAbsoluteWeight()
{
	float localWeight = 0.0f;
	for (UGameItemBase* Item : Container) // Proper range-based loop
	{
		if (Item)
		{
			localWeight += Item->GetItemFloatProperty(EItemFloatStat::Weight);
		}
	}
	return localWeight;
}

int32 UInventory::TransferAllTo(UInventory* DestinationInventory)
{
	if (!DestinationInventory || DestinationInventory == this) { return 0;} int32 TransferCount = 0;
	TArray<UGameItemBase*> ItemsToMove;
	ItemsToMove.Reserve(Container.Num());
	for (UGameItemBase* Item : Container)
	{
		if (IsValid(Item))
		{
			ItemsToMove.Add(Item);
		}
	}
	for (UGameItemBase* Item : ItemsToMove)
	{
		if (!IsValid(Item) || Item->Owner != this)
		{
			continue;
		}

		const int32 SourceIndex = Item->CurrentIndex;
		UInventory* OldOwner = Item->Owner;

		const bool bCreateItem = Item->OnItemInitOrMove(DestinationInventory);

		if (!bCreateItem)
		{
			if (OldOwner && OldOwner->Container.IsValidIndex(SourceIndex) && OldOwner->Container[SourceIndex] == Item)
			{
				OldOwner->Container[SourceIndex] = nullptr;

				if (OldOwner->GetNetMode() != NM_Client)
				{
					OldOwner->HandleClient(false, SourceIndex, Item, SourceIndex, OldOwner);
				}

				OldOwner->OnRep_Container();
				++TransferCount;
			}

			continue;
		}

		bool bAdded = false;
		DestinationInventory->TryAddItem(Item, -1, bAdded, SourceIndex);

		if (bAdded)
		{
			++TransferCount;
		}
	}
	NotifyInventoryChanged(); // source
	DestinationInventory->NotifyInventoryChanged(); // destination
	return TransferCount;
}

void UInventory::CreateItemPRC_Implementation(FName ID, EPrimaryItemType Type, int32 Quantity, int32 IntendedIndex)
{
	if (Quantity <= 0)
	{
		return;
	}

	bool bCreated = false;
	UGameItemBase* LocalItem = nullptr;

	CreateItem(ID, Type, Quantity, bCreated, LocalItem);

	if (!bCreated || !LocalItem)
	{
		return;
	}

	bool bAdded = false;
	TryAddItem(LocalItem, IntendedIndex, bAdded);
}

bool UInventory::CreateItemPRC_Validate(FName ID, EPrimaryItemType Type, int32 Quantity, int32 IntendedIndex)
{
	return Quantity > 0 && (IntendedIndex == -1 || Container.IsValidIndex(IntendedIndex));
}


bool UInventory::CanAfford(FCharStat BaseCost)
{
	return false;
}

void UInventory::GiveMoney_Implementation(FCharStat Currency)
{
}

bool UInventory::GiveMoney_Validate(FCharStat Currency) { return true; };

void UInventory::AttemptMerchantTransaction_Implementation(UGameItemBase* Item)
{
}

bool UInventory::AttemptMerchantTransaction_Validate(UGameItemBase* Item) { return true; };

void UInventory::CreateItem(FName ID, EPrimaryItemType Type, int32 Quantity, bool& Created, UGameItemBase*& CreatedItem)
{
	Created = false;
	CreatedItem = nullptr;

	if (ID.IsNone() || Quantity <= 0)
	{
		return;
	}

	if (Type == EPrimaryItemType::Equipment)
	{
		UDataTable* const* FoundTable = ItemDataTables.Find(EPrimaryItemType::Equipment);
		if (!FoundTable || !*FoundTable)
		{
			return;
		}

		FEquipmentItem* LocalRow = (*FoundTable)->FindRow<FEquipmentItem>(ID, TEXT("ContextString"), true);
		if (!LocalRow)
		{
			return;
		}

		TSubclassOf<UGameItemEquipment> DefaultClass = UGameItemEquipment::StaticClass();
		TSubclassOf<UGameItemEquipment> ItemClass = DefaultClass;

		if (LocalRow->ItemClass && LocalRow->ItemClass->IsChildOf(DefaultClass))
		{
			ItemClass = LocalRow->ItemClass;
		}

		UGameItemEquipment* LocalItem = NewObject<UGameItemEquipment>(this, ItemClass);
		if (!LocalItem)
		{
			return;
		}

		LocalItem->MaxStack = LocalRow->MaxStack;

		if (!LocalItem->InitGameItemVars(ID, LocalRow->ItemTags, *FoundTable, Quantity))
		{
			return;
		}

		LocalItem->EquipmentData = *LocalRow;
		LocalItem->PrimaryType = Type;

		Created = true; CreatedItem = LocalItem; return;
	}

	if (Type == EPrimaryItemType::Weapon)
	{
		UDataTable* const* FoundTable = ItemDataTables.Find(EPrimaryItemType::Weapon);
		if (!FoundTable || !*FoundTable)
		{
			return;
		}

		FSinWeapon* LocalRow = (*FoundTable)->FindRow<FSinWeapon>(ID, TEXT("ContextString"), true);
		if (!LocalRow)
		{
			return;
		}

		UGameItemWeapon* LocalWeapon = NewObject<UGameItemWeapon>(this);
		if (!LocalWeapon)
		{
			return;
		}

		LocalWeapon->MaxStack = LocalRow->MaxStack;

		if (!LocalWeapon->InitGameItemVars(ID, LocalRow->ItemTags, *FoundTable, Quantity))
		{
			return;
		}

		LocalWeapon->Stack = FMath::Clamp(Quantity, 1, LocalRow->MaxStack);
		LocalWeapon->EquipmentData.Price = LocalRow->Price;
		LocalWeapon->PrimaryType = Type;

		Created = true; CreatedItem = LocalWeapon; return;
	}

	UDataTable* const* FoundTable = ItemDataTables.Find(EPrimaryItemType::Regular);
	if (!FoundTable || !*FoundTable)
	{
		return;
	}

	FItemBase* LocalRow = (*FoundTable)->FindRow<FItemBase>(ID, TEXT("ContextString"), true);
	if (!LocalRow)
	{
		return;
	}

	TSubclassOf<UGameItemBase> DefaultClass = UGameItemBase::StaticClass();
	TSubclassOf<UGameItemBase> ItemClass = DefaultClass;

	if (LocalRow->ItemClass && LocalRow->ItemClass->IsChildOf(DefaultClass))
	{
		ItemClass = LocalRow->ItemClass;
	}

	UGameItemBase* LocalItem = NewObject<UGameItemBase>(this, ItemClass);
	if (!LocalItem)
	{
		return;
	}

	LocalItem->MaxStack = LocalRow->MaxStack;

	if (!LocalItem->InitGameItemVars(ID, LocalRow->ItemTags, *FoundTable, Quantity))
	{
		return;
	}

	LocalItem->DefaultData = *LocalRow;
	LocalItem->Stack = FMath::Clamp(Quantity, 1, LocalRow->MaxStack);
	LocalItem->PrimaryType = Type;

	Created = true; CreatedItem = LocalItem; return;
}

void UInventory::TryAddItem(UGameItemBase* Item, int32 TargetIndex, bool& Success, int32 SrcIndex)
{
	if (GetOwnerRole() == ROLE_Authority && IsValid(Item))
	{
		UInventory* PreviousHolder = nullptr;
		if (IsValid(Item->Owner))
		{
			PreviousHolder = Item->Owner;
		}
		if (TargetIndex < 0 || TargetIndex>=InventorySize)
		{
			if (Item->GetItemIntProperty(EItemIntStat::MaxStack) > 1)
			{
				int32 MaxStack = Item->GetItemIntProperty(EItemIntStat::MaxStack);
				if (!Container.IsEmpty())
				{
					for (UGameItemBase* GameItem : Container)
					{
						if (GameItem)
						{
							if (GameItem->ItemID == Item->ItemID && GameItem->Stack != MaxStack && Item->Stack != MaxStack)
							{
								StackItem(Item, GameItem, MaxStack);
								return;
							}
						}
					}
				}
			}
			for (int32 i = 0; i < InventorySize; i++)
			{
				UGameItemBase* LocItem = nullptr;
				if (SlotRules(Item, i))
				{
					if (!Container.IsValidIndex(i) || !Container[i])
					{
						TargetIndex = i;
						if (SrcIndex != -1)
						{
							PreviousHolder->RemoveItem(Item->CurrentIndex);
						}
						AddItem(Item, TargetIndex, PreviousHolder, SrcIndex);
						Success = true;
						return;
					}
				}
			}
			Success = false;
			return;
		}
		if (SlotRules(Item, TargetIndex))
		{
			bool Occupied;
			UGameItemBase* TargetIndexItem;
			HasItemAtIndex(TargetIndex, Occupied, TargetIndexItem);
			if (Occupied)
			{
				if (Item->GetItemIntProperty(EItemIntStat::MaxStack) > 1 &&
					TargetIndexItem->ItemID == Item->ItemID &&
					TargetIndexItem->Stack != TargetIndexItem->GetItemIntProperty(EItemIntStat::MaxStack) &&
					Item->Stack != Item->GetItemIntProperty(EItemIntStat::MaxStack))
				{
					StackItem(Item, TargetIndexItem, TargetIndexItem->GetItemIntProperty(EItemIntStat::MaxStack));
					return;
				}
				if (!PreviousHolder->SlotRules(TargetIndexItem, SrcIndex))
				{
					Success = false;
					return;
				}
				PreviousHolder->RemoveItem(SrcIndex);
				RemoveItem(TargetIndex);
				AddItem(Item, TargetIndex, PreviousHolder, SrcIndex);
				if (PreviousHolder)
				{
					PreviousHolder->AddItem(TargetIndexItem, SrcIndex, this, TargetIndex);
				};
				Success = true;
				return;
			}
			else
			{
				if (PreviousHolder)
				{
					PreviousHolder->RemoveItem(SrcIndex);
				}
				AddItem(Item, TargetIndex, PreviousHolder, SrcIndex);
				Success = true;
				return;
			}
		}
	}
	Success = false;
	return;
}

void UInventory::AddItem(UGameItemBase* Item, int32 TargetIndex, UInventory* SrcInventory, int32 OldIndex)
{
	if (!Item)
	{
		return;
	}
	if (Container.Num() < TargetIndex + 1)
	{
		Container.SetNum(TargetIndex+1);
	}
	int32 SrcIndex = Item->CurrentIndex;
	UInventory* OldInventory = Item->Owner;
	if (Item->Owner)
	{
		Item->PreviousOwner = Item->Owner;
	}
	Item->Owner = this;
	Container[TargetIndex] = Item;
	Item->PreviousIndex = SrcIndex;
	Item->CurrentIndex = TargetIndex;
	if (GetNetMode() != NM_Client)
	{
		HandleClient(true, TargetIndex, Item, SrcIndex, OldInventory);
	}
}

void UInventory::RemoveItem(int32 Index)
{
	bool ItemFound;
	UGameItemBase* LocItem;
	HasItemAtIndex(Index, ItemFound, LocItem);
	if (ItemFound)
	{
		Container[Index] = nullptr;
		if (GetNetMode() != NM_Client)
		{
			HandleClient(false, LocItem->CurrentIndex, LocItem, Index, this);
		}
	}

}

void UInventory::StackItem(UGameItemBase* IncomingItem, UGameItemBase* TargetItem, int32 MaxStack)
{
	int32 ExistingStack = TargetItem->Stack;
	int32 IncomingStack = IncomingItem->Stack;
	int32 Remainder = 0;
	int32 Sum = ExistingStack + IncomingStack;
	if (Sum > MaxStack)
	{
		TargetItem->SetStack(MaxStack);
		Remainder = Sum % MaxStack;
		bool Created;
		UGameItemBase* CreatedItem;
		CreateItem(IncomingItem->ItemID, IncomingItem->PrimaryType, Remainder, Created, CreatedItem);
		if (Created)
		{
			if (IncomingItem->Owner == TargetItem->Owner && Container.Contains(IncomingItem))
			{
					IncomingItem->SetStack(Remainder);
					return;
			}
			bool Added;
			TryAddItem(CreatedItem, -1, Added);
			if (Added)
			{
				if (IncomingItem->Owner && IncomingItem->Owner->Container.Contains(IncomingItem))
				{
						IncomingItem->Owner->RemoveItem(IncomingItem->CurrentIndex);
				}
			}
			else
			{
				IncomingItem->SetStack(Remainder);
			}
		}
		return;
	}
	else
	{
		TargetItem->SetStack(Sum);
		if(IncomingItem->Owner)
		{
			if (IncomingItem->Owner->Container.Contains(IncomingItem))
			{
				IncomingItem->Owner->RemoveItem(IncomingItem->CurrentIndex);
			}
		}
		return;
	}
}

void UInventory::MoveItemDeux_Implementation(UGameItemBase* Item, int32 DstIndex)
{
	if (!Item)
	{
		return;
	}

	const int32 OldIndex = Item->CurrentIndex;
	UInventory* OldOwner = Item->Owner;

	const bool bCreateItem = Item->OnItemInitOrMove(this);

	if (!bCreateItem)
	{
		if (OldOwner && OldOwner->Container.IsValidIndex(OldIndex) && OldOwner->Container[OldIndex] == Item)
		{
			OldOwner->Container[OldIndex] = nullptr;

			if (OldOwner->GetNetMode() != NM_Client)
			{
				OldOwner->HandleClient(false, OldIndex, Item, OldIndex, OldOwner);
			}

			OldOwner->OnRep_Container();
		}

		return;
	}

	bool bMoved = false;
	TryAddItem(Item, DstIndex, bMoved, OldIndex);
}

bool UInventory::MoveItemDeux_Validate(UGameItemBase* Item, int32 DstIndex) { return true; }

bool UInventory::SlotRules(UGameItemBase* Item, int32 Slot)
{
	if (Item->ItemTags.HasTag(InventoryType))
	{
		return true;
	}
	return false;
}

void UInventory::HasItemAtIndex(int32 Index, bool& HasItem, UGameItemBase*& Item)
{
	if (Container.IsValidIndex(Index))
	{
		if (Container[Index] != nullptr)
		{
			//if (IsValidID(Container[Index]->ItemID, EPrimaryItemType::Regular))
			//{
				HasItem = true;
				Item = Container[Index];
				return;
			//}

		}
	}
	HasItem = false;
	Item = nullptr;
	return;
}

bool UInventory::CreateInventoryEntry(USinItemDefinition* ItemDefinition, FSinInventoryEntry& OutEntry, int32 StackCount) const
{
	if (!ItemDefinition || StackCount <= 0)
	{
		return false;
	}

	StackCount = FMath::Max(1, StackCount);
	
	OutEntry = FSinInventoryEntry();
	OutEntry.EntryId = FGuid::NewGuid();
	OutEntry.ItemDefinition = ItemDefinition;
	OutEntry.StackCount = StackCount;
	OutEntry.ContainerTag = FGameplayTag();
	OutEntry.SlotIndex = INDEX_NONE;

	return true;
}

bool UInventory::AddItemToInventory(USinItemDefinition* ItemDefinition, int32 StackCount)
{
	if (!ItemDefinition || StackCount <= 0)
	{
		return false;
	}

	FGuid BestContainerId;
	if (!FindBestContainerForItem(ItemDefinition, BestContainerId))
	{
		return false;
	}

	// First, try to merge into existing stacks in this exact container.
	StackCount = TryStackIntoExistingEntries(ItemDefinition, StackCount, BestContainerId);

	if (StackCount <= 0)
	{
		return true;
	}

	const int32 MaxStackSize = GetMaxStackSize(ItemDefinition);

	bool bAddedAnything = false;

	while (StackCount > 0)
	{
		const int32 AmountForThisEntry = FMath::Min(StackCount, MaxStackSize);

		FSinInventoryEntry NewEntry;
		if (!CreateInventoryEntry(ItemDefinition, NewEntry, AmountForThisEntry))
		{
			break;
		}

		if (!AddEntryToContainer(NewEntry, BestContainerId))
		{
			break;
		}

		StackCount -= AmountForThisEntry;
		bAddedAnything = true;
	}

	return bAddedAnything;
}

bool UInventory::DoesContainerAcceptItem(const FGuid& ContainerId, USinItemDefinition* ItemDefinition) const
{
	if (!ItemDefinition)
	{
		return false;
	}

	const FSinInventoryContainerState* Subinventory =
		FindContainerStateById(ContainerId);

	if (!Subinventory)
	{
		return false;
	}

	return Subinventory->AcceptsItemTags(ItemDefinition->ItemTags);
}

bool UInventory::IsContainerFull(const FGuid& ContainerId) const
{
	const FSinInventoryContainerState* Subinventory =
		FindContainerStateById(ContainerId);

	if (!Subinventory)
	{
		return true; // treat invalid container as full
	}

	if (Subinventory->bBottomless)
	{
		return false;
	}

	int32 ItemCount = 0;

	for (const FSinInventoryEntry& Entry : ItemInventory)
	{
		if (Entry.ContainerId == ContainerId)
		{
			ItemCount++;
		}
	}

	return ItemCount >= Subinventory->SlotCount;
}

bool UInventory::FindFirstFreeSlotV2(
	const FGuid& ContainerId,
	int32& OutSlotIndex,
	USinItemDefinition* ItemDefinition) const
{
	OutSlotIndex = INDEX_NONE;

	const FSinInventoryContainerState* Subinventory =
		FindContainerStateById(ContainerId);

	if (!Subinventory)
	{
		return false;
	}

	if (Subinventory->bBottomless)
	{
		int32 MaxUsedIndex = INDEX_NONE;

		for (const FSinInventoryEntry& Entry : ItemInventory)
		{
			if (Entry.ContainerId == ContainerId)
			{
				MaxUsedIndex = FMath::Max(MaxUsedIndex, Entry.SlotIndex);
			}
		}

		OutSlotIndex = MaxUsedIndex + 1;
		return true;
	}

	for (int32 SlotIndex = 0; SlotIndex < Subinventory->SlotCount; ++SlotIndex)
	{
		if (FindEntryIndexAtSlot(ContainerId, SlotIndex) != INDEX_NONE)
		{
			continue;
		}

		if (ItemDefinition)
		{
			if (!DoesContainerAcceptItemAtSlot(ContainerId, SlotIndex, ItemDefinition))
			{
				continue;
			}
		}

		OutSlotIndex = SlotIndex;
		return true;
	}

	return false;
}

bool UInventory::FindBestSlotForEntry(
	const FGuid& ContainerId,
	const FSinInventoryEntry& SourceEntry,
	int32& OutSlotIndex,
	bool bAllowSwap) const
{
	OutSlotIndex = INDEX_NONE;

	const FSinInventoryContainerState* LocContainer = FindContainerStateById(ContainerId);
	if (!LocContainer || !SourceEntry.ItemDefinition)
	{
		return false;
	}

	if (!DoesContainerAcceptItem(ContainerId, SourceEntry.ItemDefinition))
	{
		return false;
	}

	const int32 MaxStackSize = GetMaxStackSize(SourceEntry.ItemDefinition);
	const bool bCanStack = MaxStackSize > 1;

	int32 EmptySlot = INDEX_NONE;
	int32 SwapSlot = INDEX_NONE;

	for (int32 SlotIndex = 0; SlotIndex < LocContainer->SlotCount; ++SlotIndex)
	{
		if (!DoesContainerAcceptItemAtSlot(ContainerId, SlotIndex, SourceEntry.ItemDefinition))
		{
			continue;
		}

		const int32 ExistingIndex = FindEntryIndexAtSlot(ContainerId, SlotIndex);

		if (ExistingIndex == INDEX_NONE)
		{
			if (EmptySlot == INDEX_NONE)
			{
				EmptySlot = SlotIndex;
			}
			continue;
		}

		const FSinInventoryEntry& Existing = ItemInventory[ExistingIndex];

		// Prefer merge
		if (bCanStack
			&& CanStackEntries(SourceEntry, Existing)
			&& Existing.StackCount < MaxStackSize)
		{
			OutSlotIndex = SlotIndex;
			return true;
		}

		if (bAllowSwap && SwapSlot == INDEX_NONE)
		{
			SwapSlot = SlotIndex;
		}
	}

	if (EmptySlot != INDEX_NONE)
	{
		OutSlotIndex = EmptySlot;
		return true;
	}

	if (bAllowSwap && SwapSlot != INDEX_NONE)
	{
		OutSlotIndex = SwapSlot;
		return true;
	}

	return false;
}

int32 UInventory::PourEntryIntoContainer(const FGuid& EntryId, const FGuid& TargetContainerId)
{
if (!EntryId.IsValid() || !TargetContainerId.IsValid())
	{
		return INDEX_NONE;
	}

	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}

	FSinInventoryEntry& SourceEntry = ItemInventory[SourceIndex];
	if (!SourceEntry.ItemDefinition || SourceEntry.StackCount <= 0)
	{
		return INDEX_NONE;
	}

	const FSinInventoryContainerState* TargetContainer = FindContainerStateById(TargetContainerId);
	if (!TargetContainer)
	{
		return SourceEntry.StackCount;
	}

	if (!DoesContainerAcceptItem(TargetContainerId, SourceEntry.ItemDefinition))
	{
		return SourceEntry.StackCount;
	}

	const int32 MaxStackSize = GetMaxStackSize(SourceEntry.ItemDefinition);
	if (MaxStackSize <= 1)
	{
		return SourceEntry.StackCount;
	}

	const FGuid SourceContainerId = SourceEntry.ContainerId;
	bool bAnyChange = false;

	const int32 SlotCount = TargetContainer->bBottomless
		? FMath::Max(TargetContainer->SlotCount, 256)
		: TargetContainer->SlotCount;

	for (int32 SlotIndex = 0; SlotIndex < SlotCount && SourceEntry.StackCount > 0; ++SlotIndex)
	{
		const int32 TargetIndex = FindEntryIndexAtSlot(TargetContainerId, SlotIndex);
		if (TargetIndex == INDEX_NONE || TargetIndex == SourceIndex)
		{
			continue;
		}

		FSinInventoryEntry& TargetEntry = ItemInventory[TargetIndex];

		if (!CanStackEntries(SourceEntry, TargetEntry))
		{
			continue;
		}

		if (TargetEntry.StackCount >= MaxStackSize)
		{
			continue;
		}

		if (!DoesContainerAcceptItemAtSlot(TargetContainerId, SlotIndex, SourceEntry.ItemDefinition))
		{
			continue;
		}

		const int32 SpaceAvailable = MaxStackSize - TargetEntry.StackCount;
		const int32 AmountToMove = FMath::Min(SpaceAvailable, SourceEntry.StackCount);
		if (AmountToMove <= 0)
		{
			continue;
		}

		const FSinInventoryEntry OldSourceEntry = SourceEntry;
		const FSinInventoryEntry OldTargetEntry = TargetEntry;

		TargetEntry.StackCount += AmountToMove;
		SourceEntry.StackCount -= AmountToMove;
		bAnyChange = true;

		OnInventoryEntryChanged.Broadcast(this, OldTargetEntry, TargetEntry);
		OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
	}

	if (!bAnyChange)
	{
		return SourceEntry.StackCount;
	}

	OnContainerChanged.Broadcast(this, TargetContainerId);
	if (SourceContainerId != TargetContainerId)
	{
		OnContainerChanged.Broadcast(this, SourceContainerId);
	}

	if (SourceEntry.StackCount <= 0)
	{
		const FSinInventoryEntry RemovedEntry = SourceEntry;
		ItemInventory.RemoveAt(SourceIndex);
		OnInventoryEntryRemoved.Broadcast(this, RemovedEntry);
		OnContainerChanged.Broadcast(this, SourceContainerId);
		return 0;
	}
	return SourceEntry.StackCount;
}

bool UInventory::FindBestContainerForItem(USinItemDefinition* ItemDefinition, FGuid& OutContainerId) const
{
	OutContainerId.Invalidate();

	if (!ItemDefinition)
	{
		return false;
	}

	const FSinInventoryContainerState* BestContainer = nullptr;

	for (const FSinInventoryContainerState& Subinventory : Containers)
	{
		
		if (!Subinventory.ContainerId.IsValid())
		{
			continue;
		}
		
		if (!Subinventory.bAllowAutoAdd)
		{
			continue;
		}

		if (!DoesContainerAcceptItem(Subinventory.ContainerId, ItemDefinition))
		{
			continue;
		}

		if (IsContainerFull(Subinventory.ContainerId))
		{
			continue;
		}

		if (!BestContainer || Subinventory.SortPriority < BestContainer->SortPriority)
		{
			BestContainer = &Subinventory;
		}
	}

	if (!BestContainer)
	{
		return false;
	}

	OutContainerId = BestContainer->ContainerId;
	return true;
}

const FSinInventoryContainerState* UInventory::FindContainerStateById(const FGuid& ContainerId) const
{
	for (const FSinInventoryContainerState& Subinventory : Containers)
	{
		if (Subinventory.ContainerId == ContainerId)
		{
			return &Subinventory;
		}
	}

	return nullptr;
}

bool UInventory::AddEntryToContainer(const FSinInventoryEntry& Entry, const FGuid& PreferredContainerId)
{
	if (!Entry.ItemDefinition)
	{
		return false;
	}

	const FSinInventoryContainerState* ContainerState =
		FindContainerStateById(PreferredContainerId);

	if (!ContainerState)
	{
		return false;
	}

	// Rules still use the container's gameplay tag.
	if (!DoesContainerAcceptItem(ContainerState->ContainerId, Entry.ItemDefinition))
	{
		return false;
	}

	int32 FreeSlot = INDEX_NONE;

	if (!FindFirstFreeSlotV2(PreferredContainerId, FreeSlot, Entry.ItemDefinition))
	{
		return false;
	}

	FSinInventoryEntry NewEntry = Entry;

	NewEntry.ContainerId = PreferredContainerId;
	NewEntry.ContainerTag = ContainerState->ContainerTag;
	NewEntry.SlotIndex = FreeSlot;

	if (!NewEntry.EntryId.IsValid())
	{
		NewEntry.EntryId = FGuid::NewGuid();
	}

	ItemInventory.Add(NewEntry);

	OnInventoryEntryAdded.Broadcast(this, NewEntry);
	OnContainerChanged.Broadcast(this, PreferredContainerId);

	return true;
}

bool UInventory::TransferEntryToInventory(
	const FGuid& EntryId,
	UInventory* TargetInventory,
	const FGuid& TargetContainerId,
	int32 TargetSlotIndex)
{
	if (!TargetInventory || !EntryId.IsValid() || !TargetContainerId.IsValid())
	{
		return false;
	}

	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE)
	{
		return false;
	}

	FSinInventoryEntry& SourceEntry = ItemInventory[SourceIndex];

	if (!SourceEntry.ItemDefinition)
	{
		return false;
	}

	const FGuid SourceContainerId = SourceEntry.ContainerId;
	const int32 SourceSlotIndex = SourceEntry.SlotIndex;
	const FSinInventoryEntry OldSourceEntry = SourceEntry;

	if (!SourceContainerId.IsValid())
	{
		return false;
	}

	const FSinInventoryContainerState* TargetContainer =
		TargetInventory->FindContainerStateById(TargetContainerId);

	if (!TargetContainer)
	{
		return false;
	}

	// Auto-slot transfer.
	if (TargetSlotIndex == INDEX_NONE)
	{
		FSinInventoryEntry EntryCopy = SourceEntry;

		if (!TargetInventory->AddEntryToContainer(EntryCopy, TargetContainerId))
		{
			return false;
		}

		const FSinInventoryEntry RemovedEntry = SourceEntry;
		
		ItemInventory.RemoveAt(SourceIndex);

		OnInventoryEntryRemoved.Broadcast(this, RemovedEntry);
		OnContainerChanged.Broadcast(this, SourceContainerId);

		return true;
	}

	if (!TargetContainer->bBottomless)
	{
		if (TargetSlotIndex < 0 || TargetSlotIndex >= TargetContainer->SlotCount)
		{
			return false;
		}
	}

	if (!TargetInventory->DoesContainerAcceptItemAtSlot(
		TargetContainerId,
		TargetSlotIndex,
		SourceEntry.ItemDefinition))
	{
		return false;
	}

	const int32 TargetEntryIndex =
		TargetInventory->FindEntryIndexAtSlot(TargetContainerId, TargetSlotIndex);

	// Empty exact target slot.
	if (TargetEntryIndex == INDEX_NONE)
	{
		FSinInventoryEntry NewEntry = SourceEntry;
		NewEntry.ContainerId = TargetContainerId;
		NewEntry.ContainerTag = TargetContainer->ContainerTag;
		NewEntry.SlotIndex = TargetSlotIndex;

		TargetInventory->AddEntryDirect(NewEntry);
		const FSinInventoryEntry RemovedEntry = SourceEntry;
		ItemInventory.RemoveAt(SourceIndex);

		OnInventoryEntryRemoved.Broadcast(this, RemovedEntry);
		OnContainerChanged.Broadcast(this, SourceContainerId);

		return true;
	}

	FSinInventoryEntry& TargetEntry =
		TargetInventory->ItemInventory[TargetEntryIndex];

	// Stack merge.
	if (TargetInventory->CanStackEntries(SourceEntry, TargetEntry))
	{
		const int32 MaxStack =
			TargetInventory->GetMaxStackSize(SourceEntry.ItemDefinition);

		const int32 SpaceAvailable =
			MaxStack - TargetEntry.StackCount;

		if (SpaceAvailable > 0)
		{
			const FSinInventoryEntry OldTargetEntry = TargetEntry;

			const int32 AmountToMove =
				FMath::Min(SpaceAvailable, SourceEntry.StackCount);

			TargetEntry.StackCount += AmountToMove;
			SourceEntry.StackCount -= AmountToMove;

			TargetInventory->OnInventoryEntryChanged.Broadcast(
				TargetInventory,
				OldTargetEntry,
				TargetEntry
			);

			TargetInventory->OnContainerChanged.Broadcast(
				TargetInventory,
				TargetContainerId
			);

			if (SourceEntry.StackCount <= 0)
			{
				const FSinInventoryEntry RemovedEntry = SourceEntry;
				ItemInventory.RemoveAt(SourceIndex);
				
				OnInventoryEntryRemoved.Broadcast(this, RemovedEntry);
				OnContainerChanged.Broadcast(this, SourceContainerId);
			}
			else
			{
				OnInventoryEntryChanged.Broadcast(
					this,
					OldSourceEntry,
					SourceEntry
				);

				OnContainerChanged.Broadcast(this, SourceContainerId);
			}

			return true;
		}
	}

	// Cross-inventory swap.
	const FSinInventoryContainerState* SourceContainer =
		FindContainerStateById(SourceContainerId);

	if (!SourceContainer)
	{
		return false;
	}

	if (!DoesContainerAcceptItemAtSlot(
		SourceContainerId,
		SourceSlotIndex,
		TargetEntry.ItemDefinition))
	{
		return false;
	}

	const FSinInventoryEntry OldTargetEntry = TargetEntry;

	FSinInventoryEntry SourceCopy = SourceEntry;
	FSinInventoryEntry TargetCopy = TargetEntry;

	SourceCopy.ContainerId = TargetContainerId;
	SourceCopy.ContainerTag = TargetContainer->ContainerTag;
	SourceCopy.SlotIndex = TargetSlotIndex;

	TargetCopy.ContainerId = SourceContainerId;
	TargetCopy.ContainerTag = SourceContainer->ContainerTag;
	TargetCopy.SlotIndex = SourceSlotIndex;

	TargetInventory->ItemInventory[TargetEntryIndex] = SourceCopy;
	ItemInventory[SourceIndex] = TargetCopy;

	TargetInventory->OnInventoryEntryChanged.Broadcast(
		TargetInventory,
		OldTargetEntry,
		SourceCopy
	);

	TargetInventory->OnContainerChanged.Broadcast(
		TargetInventory,
		TargetContainerId
	);

	OnInventoryEntryChanged.Broadcast(
		this,
		OldSourceEntry,
		TargetCopy
	);

	OnContainerChanged.Broadcast(this, SourceContainerId);

	return true;
}

void UInventory::EnsureContainerIds()
{
	for (FSinInventoryContainerState& ContainerState : Containers)
	{
		if (!ContainerState.ContainerId.IsValid())
		{
			ContainerState.ContainerId = FGuid::NewGuid();
		}
	}
}

void UInventory::InitializeInventoryRuntimeState()
{
	if (!ContainerSet)
	{
		return;
	}

	Containers = ContainerSet->Containers;
	EnsureContainerIds();
}

const FSinInventoryContainerState* UInventory::FindContainerStateByTag(FGameplayTag ContainerTag) const
{
	if (!ContainerTag.IsValid()){return nullptr;}

	for (const FSinInventoryContainerState& SinContainer : Containers)
	{
		if (SinContainer.ContainerTag == ContainerTag)
		{
			return &SinContainer;
		}
	}
	return nullptr;
}

FSinInventoryContainerState* UInventory::FindMutableContainerStateByTag(FGameplayTag ContainerTag)
{
	if (!ContainerTag.IsValid()){return nullptr;}

	for (FSinInventoryContainerState& SinContainer : Containers)
	{
		if (SinContainer.ContainerTag == ContainerTag)
		{
			return &SinContainer;
		}
	}
	return nullptr;
}

bool UInventory::FindContainerStateByTagBP(FGameplayTag ContainerTag,
	FSinInventoryContainerState& OutContainerState) const
{
	if (const FSinInventoryContainerState* Found = FindContainerStateByTag(ContainerTag))
	{
		OutContainerState = *Found;
		return true;
	}
	return false;
}

bool UInventory::AddEntryDirect(const FSinInventoryEntry& Entry)
{
	ItemInventory.Add(Entry);

	OnInventoryEntryAdded.Broadcast(this, Entry);
	OnContainerChanged.Broadcast(this, Entry.ContainerId);

	return true;
}

bool UInventory::SplitEntryStackToSlot(
	const FGuid& EntryId,
	int32 SplitAmount,
	const FGuid& TargetContainerId,
	int32 TargetSlotIndex)
{
	if (!EntryId.IsValid() || !TargetContainerId.IsValid())
	{
		return false;
	}

	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE)
	{
		return false;
	}

	FSinInventoryEntry& SourceEntry = ItemInventory[SourceIndex];

	if (!SourceEntry.ItemDefinition || SourceEntry.StackCount <= 1)
	{
		return false;
	}

	const FSinInventoryContainerState* TargetContainer =
		FindContainerStateById(TargetContainerId);

	if (!TargetContainer)
	{
		return false;
	}

	if (TargetSlotIndex == -1)
	{
		if (!FindFirstFreeSlotV2(TargetContainerId, TargetSlotIndex, SourceEntry.ItemDefinition))
		{
			return false;
		}
	}

	if (!DoesContainerAcceptItemAtSlot(TargetContainerId, TargetSlotIndex, SourceEntry.ItemDefinition))
	{
		return false;
	}

	if (!TargetContainer->bBottomless)
	{
		if (TargetSlotIndex < 0 || TargetSlotIndex >= TargetContainer->SlotCount)
		{
			return false;
		}
	}

	// Clamp split amount
	if (SplitAmount == -1)
	{
		SplitAmount = SourceEntry.StackCount / 2;
	}
	SplitAmount = FMath::Clamp(SplitAmount, 1, SourceEntry.StackCount - 1);

	const int32 TargetEntryIndex = FindEntryIndexAtSlot(TargetContainerId, TargetSlotIndex);

	// -------------------------------------------------
	// Case A: target slot empty → create new entry
	// -------------------------------------------------
	if (TargetEntryIndex == INDEX_NONE)
	{
		const FSinInventoryEntry OldSourceEntry = SourceEntry;

		FSinInventoryEntry NewEntry = SourceEntry;
		NewEntry.EntryId = FGuid::NewGuid();
		NewEntry.ContainerId = TargetContainerId;
		NewEntry.ContainerTag = TargetContainer->ContainerTag;
		NewEntry.SlotIndex = TargetSlotIndex;
		NewEntry.StackCount = SplitAmount;

		SourceEntry.StackCount -= SplitAmount;

		ItemInventory.Add(NewEntry);

		OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
		OnInventoryEntryAdded.Broadcast(this, NewEntry);

		OnContainerChanged.Broadcast(this, SourceEntry.ContainerId);
		if (SourceEntry.ContainerId != TargetContainerId)
		{
			OnContainerChanged.Broadcast(this, TargetContainerId);
		}

		return true;
	}

	// -------------------------------------------------
	// Case B: target slot occupied → try stack merge
	// -------------------------------------------------
	if (TargetEntryIndex == SourceIndex)
	{
		return false;	// can't split onto yourself
	}

	FSinInventoryEntry& TargetEntry = ItemInventory[TargetEntryIndex];

	if (!CanStackEntries(SourceEntry, TargetEntry))
	{
		return false;
	}

	const int32 MaxStackSize = GetMaxStackSize(SourceEntry.ItemDefinition);
	const int32 SpaceAvailable = MaxStackSize - TargetEntry.StackCount;

	if (SpaceAvailable <= 0)
	{
		return false;
	}

	const int32 AmountToMove = FMath::Min(SplitAmount, SpaceAvailable);

	const FSinInventoryEntry OldSourceEntry = SourceEntry;
	const FSinInventoryEntry OldTargetEntry = TargetEntry;

	TargetEntry.StackCount += AmountToMove;
	SourceEntry.StackCount -= AmountToMove;

	OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
	OnInventoryEntryChanged.Broadcast(this, OldTargetEntry, TargetEntry);

	OnContainerChanged.Broadcast(this, SourceEntry.ContainerId);
	if (SourceEntry.ContainerId != TargetContainerId)
	{
		OnContainerChanged.Broadcast(this, TargetContainerId);
	}

	return true;
}

bool UInventory::SplitEntryStackToOtherInventory(
	const FGuid& EntryId,
	int32 SplitAmount,
	UInventory* TargetInventory,
	const FGuid& TargetContainerId,
	int32 TargetSlotIndex)
{
	if (!EntryId.IsValid() || !TargetInventory || !TargetContainerId.IsValid())
	{
		return false;
	}

	// Same inventory → use the existing path
	if (TargetInventory == this)
	{
		return SplitEntryStackToSlot(EntryId, SplitAmount, TargetContainerId, TargetSlotIndex);
	}

	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE)
	{
		return false;
	}

	FSinInventoryEntry& SourceEntry = ItemInventory[SourceIndex];

	if (!SourceEntry.ItemDefinition || SourceEntry.StackCount <= 1)
	{
		return false;
	}

	const FSinInventoryContainerState* TargetContainer =
		TargetInventory->FindContainerStateById(TargetContainerId);

	if (!TargetContainer)
	{
		return false;
	}

	// Resolve free slot if caller didn’t specify one
	if (TargetSlotIndex == -1)
	{
		if (!TargetInventory->FindFirstFreeSlotV2(
			TargetContainerId,
			TargetSlotIndex,
			SourceEntry.ItemDefinition))
		{
			return false;
		}
	}

	if (!TargetInventory->DoesContainerAcceptItemAtSlot(
		TargetContainerId,
		TargetSlotIndex,
		SourceEntry.ItemDefinition))
	{
		return false;
	}

	if (!TargetContainer->bBottomless)
	{
		if (TargetSlotIndex < 0 || TargetSlotIndex >= TargetContainer->SlotCount)
		{
			return false;
		}
	}

	// Clamp split amount
	if (SplitAmount == -1)
	{
		SplitAmount = SourceEntry.StackCount / 2;
	}
	SplitAmount = FMath::Clamp(SplitAmount, 1, SourceEntry.StackCount - 1);

	const int32 TargetEntryIndex =
		TargetInventory->FindEntryIndexAtSlot(TargetContainerId, TargetSlotIndex);

	// -------------------------------------------------
	// Case A: target slot is empty → create new entry
	// -------------------------------------------------
	if (TargetEntryIndex == INDEX_NONE)
	{
		const FSinInventoryEntry OldSourceEntry = SourceEntry;
		SourceEntry.StackCount -= SplitAmount;

		OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
		OnContainerChanged.Broadcast(this, SourceEntry.ContainerId);

		FSinInventoryEntry NewEntry = OldSourceEntry;
		NewEntry.EntryId = FGuid::NewGuid();
		NewEntry.ContainerId = TargetContainerId;
		NewEntry.ContainerTag = TargetContainer->ContainerTag;
		NewEntry.SlotIndex = TargetSlotIndex;
		NewEntry.StackCount = SplitAmount;

		TargetInventory->ItemInventory.Add(NewEntry);

		TargetInventory->OnInventoryEntryAdded.Broadcast(TargetInventory, NewEntry);
		TargetInventory->OnContainerChanged.Broadcast(TargetInventory, TargetContainerId);

		return true;
	}

	// -------------------------------------------------
	// Case B: target slot occupied → try stack merge
	// -------------------------------------------------
	FSinInventoryEntry& TargetEntry = TargetInventory->ItemInventory[TargetEntryIndex];

	if (!TargetInventory->CanStackEntries(SourceEntry, TargetEntry))
	{
		return false;	// different item or non-stackable
	}

	const int32 MaxStackSize = TargetInventory->GetMaxStackSize(SourceEntry.ItemDefinition);
	const int32 SpaceAvailable = MaxStackSize - TargetEntry.StackCount;

	if (SpaceAvailable <= 0)
	{
		return false;	// target stack is already full
	}

	const int32 AmountToMove = FMath::Min(SplitAmount, SpaceAvailable);

	const FSinInventoryEntry OldSourceEntry = SourceEntry;
	const FSinInventoryEntry OldTargetEntry = TargetEntry;

	TargetEntry.StackCount += AmountToMove;
	SourceEntry.StackCount -= AmountToMove;

	// Source side
	OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
	OnContainerChanged.Broadcast(this, SourceEntry.ContainerId);

	// Target side
	TargetInventory->OnInventoryEntryChanged.Broadcast(
		TargetInventory,
		OldTargetEntry,
		TargetEntry);
	TargetInventory->OnContainerChanged.Broadcast(TargetInventory, TargetContainerId);

	return true;
}

bool UInventory::DoesContainerAcceptItemAtSlot(const FGuid& ContainerId, int32 SlotIndex,
                                               USinItemDefinition* ItemDefinition) const
{
	if (!ItemDefinition){return false;}
	const FSinInventoryContainerState* ContainerState = FindContainerStateById(ContainerId);
	if (!ContainerState){return false;}
	return ContainerState->AcceptsItemTagsAtSlot(ItemDefinition->ItemTags,SlotIndex);
}

void UInventory::SetLinkedInventory(UInventory* OtherInventory)
{
	LinkedInventory=OtherInventory;
}

void UInventory::ClearLinkedInventory()
{
	LinkedInventory = nullptr;
}

bool UInventory::TryDoubleClickEntry(const FGuid& EntryId)
{
	if (!EntryId.IsValid()){return false;}

	const int32 SourceIndex = FindEntryIndexById(EntryId); if (SourceIndex == INDEX_NONE){return false;}

	const FSinInventoryEntry& Entry = ItemInventory[SourceIndex]; if (!Entry.ItemDefinition){return false;}

	// -------------------------------------------------
	// 1. External bridge (chest / other bag open)
	// -------------------------------------------------
	if (LinkedInventory.IsValid())
	{
		FGuid TargetContainerId;
		if (!LinkedInventory->FindBestContainerForItem(Entry.ItemDefinition, TargetContainerId))
		{
			return false;
		}

		int32 TargetSlot = INDEX_NONE;
		if (!LinkedInventory->FindBestSlotForEntry(TargetContainerId, Entry, TargetSlot, false))
		{
			return false;
		}

		return TransferEntryToInventory(
			EntryId,
			LinkedInventory.Get(),
			TargetContainerId,
			TargetSlot);
	}

	// -------------------------------------------------
	// 2. Internal move (no external inventory)
	// -------------------------------------------------
	const FSinInventoryContainerState* CurrentContainer =
		FindContainerStateById(Entry.ContainerId);

	if (!CurrentContainer){return false;}

	// In base backpack (auto-add allowed) → try specialized container
	if (CurrentContainer->bAllowAutoAdd)
	{
		return EquipEntryToBestSlot(EntryId);
	}

	// Already in specialized container (equipment / utility / weapon)
	// → send back to best base container
	return MoveEntryToBestContainer(EntryId);
}

void UInventory::NotifyInventoryChanged()
{
	OnInventoryRefreshed.Broadcast(this);
}

bool UInventory::MoveEntryTo(const FGuid& EntryId, const FGuid& TargetContainerId, int32 TargetSlotIndex)
{
	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE || !TargetContainerId.IsValid())
	{
		return false;
	}

	FSinInventoryEntry& SourceEntry = ItemInventory[SourceIndex];
	
	const int32 SourceSlotIndex = SourceEntry.SlotIndex;
	const FGuid SourceContainerId = SourceEntry.ContainerId;

	const FSinInventoryContainerState* TargetContainer = FindContainerStateById(TargetContainerId);
	if (!TargetContainer)
	{
		return false;
	}

	if (TargetSlotIndex == INDEX_NONE)
	{
		if (!FindFirstFreeSlotV2(TargetContainerId, TargetSlotIndex, SourceEntry.ItemDefinition))
		{
			return false;
		}
	}

	if (!DoesContainerAcceptItemAtSlot(TargetContainerId, TargetSlotIndex, SourceEntry.ItemDefinition))
	{
		return false;
	}

	if (!TargetContainer->bBottomless)
	{
		if (TargetSlotIndex < 0 || TargetSlotIndex >= TargetContainer->SlotCount)
		{
			return false;
		}
	}

	const int32 TargetEntryIndex = FindEntryIndexAtSlot(TargetContainerId, TargetSlotIndex);

	// Empty target slot
	if (TargetEntryIndex == INDEX_NONE)
	{
		const FSinInventoryEntry OldSourceEntry = SourceEntry;

		SourceEntry.ContainerId = TargetContainerId;
		SourceEntry.ContainerTag = TargetContainer->ContainerTag;
		SourceEntry.SlotIndex = TargetSlotIndex;

		OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
		OnContainerChanged.Broadcast(this, SourceContainerId);

		if (SourceContainerId != TargetContainerId)
		{
			OnContainerChanged.Broadcast(this, TargetContainerId);
		}

		return true;
	}

	if (TargetEntryIndex == SourceIndex)
	{
		return true;
	}

	FSinInventoryEntry& TargetEntry = ItemInventory[TargetEntryIndex];

	// Stack merge
	if (CanStackEntries(SourceEntry, TargetEntry))
	{
		const int32 MaxStackSize = GetMaxStackSize(SourceEntry.ItemDefinition);
		const int32 SpaceAvailable = MaxStackSize - TargetEntry.StackCount;

		if (SpaceAvailable > 0)
		{
			const FSinInventoryEntry OldSourceEntry = SourceEntry;
			const FSinInventoryEntry OldTargetEntry = TargetEntry;

			const int32 AmountToMove = FMath::Min(SpaceAvailable, SourceEntry.StackCount);

			TargetEntry.StackCount += AmountToMove;
			SourceEntry.StackCount -= AmountToMove;

			OnInventoryEntryChanged.Broadcast(this, OldTargetEntry, TargetEntry);

			if (SourceEntry.StackCount <= 0)
			{
				
				const FSinInventoryEntry RemovedEntry = SourceEntry;
				ItemInventory.RemoveAt(SourceIndex);

				OnInventoryEntryRemoved.Broadcast(this, RemovedEntry);
				OnContainerChanged.Broadcast(this, SourceContainerId);
			}
			else
			{
				OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
			}

			OnContainerChanged.Broadcast(this, SourceContainerId);

			if (SourceContainerId != TargetContainerId)
			{
				OnContainerChanged.Broadcast(this, TargetContainerId);
			}

			return true;
		}
	}

	// Swap validation
	if (!DoesContainerAcceptItemAtSlot(SourceContainerId, SourceSlotIndex, TargetEntry.ItemDefinition))
	{
		return false;
	}

	const FSinInventoryContainerState* SourceContainer = FindContainerStateById(SourceContainerId);
	if (!SourceContainer)
	{
		return false;
	}

	const FSinInventoryEntry OldSourceEntry = SourceEntry;
	const FSinInventoryEntry OldTargetEntry = TargetEntry;

	TargetEntry.ContainerId = SourceContainerId;
	TargetEntry.ContainerTag = SourceContainer->ContainerTag;
	TargetEntry.SlotIndex = SourceSlotIndex;

	SourceEntry.ContainerId = TargetContainerId;
	SourceEntry.ContainerTag = TargetContainer->ContainerTag;
	SourceEntry.SlotIndex = TargetSlotIndex;

	OnInventoryEntryChanged.Broadcast(this, OldSourceEntry, SourceEntry);
	OnInventoryEntryChanged.Broadcast(this, OldTargetEntry, TargetEntry);

	OnContainerChanged.Broadcast(this, SourceContainerId);

	if (SourceContainerId != TargetContainerId)
	{
		OnContainerChanged.Broadcast(this, TargetContainerId);
	}

	return true;
}

int32 UInventory::FindEntryIndexById(const FGuid& EntryId) const
{
	for (int32 i = 0; i < ItemInventory.Num(); ++i)
	{
		if (ItemInventory[i].EntryId == EntryId)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

FSinInventoryEntry* UInventory::FindEntryById(const FGuid& EntryId)
{
	const int32 Index = FindEntryIndexById(EntryId);

	if (Index == INDEX_NONE)
	{
		return nullptr;
	}

	return &ItemInventory[Index];
}

const FSinInventoryEntry* UInventory::FindEntryById(const FGuid& EntryId) const
{
	const int32 Index = FindEntryIndexById(EntryId);

	if (Index == INDEX_NONE)
	{
		return nullptr;
	}

	return &ItemInventory[Index];
}

int32 UInventory::FindEntryIndexAtSlot(const FGuid& ContainerId, int32 SlotIndex) const
{
	for (int32 i = 0; i < ItemInventory.Num(); ++i)
	{
		const FSinInventoryEntry& Entry = ItemInventory[i];

		if (Entry.ContainerId == ContainerId && Entry.SlotIndex == SlotIndex)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int32 UInventory::GetMaxStackSize(USinItemDefinition* ItemDefinition) const
{
	if (!ItemDefinition)
	{
		return 1;
	}

	const USinItemFragment_Inventory* InventoryFragment =
		ItemDefinition->FindFragmentByClass<USinItemFragment_Inventory>();

	if (!InventoryFragment)
	{
		return 1;
	}

	return FMath::Max(1, InventoryFragment->MaxStackSize);
}

bool UInventory::CanStackEntries(const FSinInventoryEntry& A, const FSinInventoryEntry& B) const
{
	if (!A.ItemDefinition || !B.ItemDefinition)
	{
		return false;
	}

	if (A.ItemDefinition != B.ItemDefinition)
	{
		return false;
	}

	return GetMaxStackSize(A.ItemDefinition) > 1;
}

int32 UInventory::TryStackIntoExistingEntries(
	USinItemDefinition* ItemDefinition,
	int32 StackCount,
	const FGuid& ContainerId)
{
	if (!ItemDefinition || StackCount <= 0)
	{
		return StackCount;
	}

	const int32 MaxStackSize = GetMaxStackSize(ItemDefinition);

	if (MaxStackSize <= 1)
	{
		return StackCount;
	}

	for (FSinInventoryEntry& Entry : ItemInventory)
	{
		if (StackCount <= 0)
		{
			break;
		}

		if (Entry.ContainerId != ContainerId)
		{
			continue;
		}

		if (Entry.ItemDefinition != ItemDefinition)
		{
			continue;
		}

		if (Entry.StackCount >= MaxStackSize)
		{
			continue;
		}

		const int32 SpaceAvailable = MaxStackSize - Entry.StackCount;
		const int32 AmountToAdd = FMath::Min(SpaceAvailable, StackCount);

		FSinInventoryEntry OldEntry = Entry;

		Entry.StackCount += AmountToAdd;
		StackCount -= AmountToAdd;

		OnInventoryEntryChanged.Broadcast(this, OldEntry, Entry);
		OnContainerChanged.Broadcast(this, ContainerId);
	}
	return StackCount;
}

bool UInventory::GetContainerStateById(const FGuid& ContainerId, FSinInventoryContainerState& OutContainerState) const
{
	if (const FSinInventoryContainerState* Found = FindContainerStateById(ContainerId))
	{
		OutContainerState = *Found;
		return true;
	}
	return false;
}

bool UInventory::PlaceEntryAtSlot(FSinInventoryEntry& IncomingEntry, int32 IncomingSourceIndex,
	const FGuid& TargetContainerId, int32 TargetSlotIndex, UInventory* SourceInventory)
{
	return false;
}

void UInventory::HandleClient_Implementation(bool Added, int32 NewIndex, UGameItemBase* Item, int32 OldIndex, UInventory* SrcInventory)
{
	if (Added)
	{

			this->OnSignalItemAdded.Broadcast(this, NewIndex, Item, OldIndex, SrcInventory);
			return;
	}
	else
	{
		this->OnSignalItemRemoved.Broadcast(this, NewIndex, Item, OldIndex, SrcInventory);
		return;
	}
}


void UInventory::OnRep_Container()
{
	OnInventoryRefreshed.Broadcast(this);
}

bool UInventory::MoveEntryToBestContainer(const FGuid& EntryId)
{
	if (!EntryId.IsValid())
	{
		return false;
	}

	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE)
	{
		return false;
	}

	const FSinInventoryEntry SourceSnapshot = ItemInventory[SourceIndex];
	if (!SourceSnapshot.ItemDefinition)
	{
		return false;
	}

	const FGuid SourceContainerId = SourceSnapshot.ContainerId;
	const int32 StartingStack = SourceSnapshot.StackCount;
	bool bMovedAnything = false;

	TArray<const FSinInventoryContainerState*> Candidates;
	for (const FSinInventoryContainerState& SinContainer : Containers)
	{
		if (!SinContainer.ContainerId.IsValid() || !SinContainer.bAllowAutoAdd)
		{
			continue;
		}

		if (SinContainer.ContainerId == SourceContainerId)
		{
			continue;
		}

		if (!DoesContainerAcceptItem(SinContainer.ContainerId, SourceSnapshot.ItemDefinition))
		{
			continue;
		}

		Candidates.Add(&SinContainer);
	}

	Candidates.Sort([](const FSinInventoryContainerState& A, const FSinInventoryContainerState& B)
	{
		return A.SortPriority < B.SortPriority;
	});

	for (const FSinInventoryContainerState* Candidate : Candidates)
	{
		if (FindEntryIndexById(EntryId) == INDEX_NONE)
		{
			return true;
		}

		const int32 Leftover = PourEntryIntoContainer(EntryId, Candidate->ContainerId);
		if (Leftover != StartingStack && Leftover != INDEX_NONE)
		{
			bMovedAnything = true;
		}

		if (Leftover == 0)
		{
			return true;
		}

		int32 EmptySlot = INDEX_NONE;
		if (FindFirstFreeSlotV2(Candidate->ContainerId, EmptySlot, SourceSnapshot.ItemDefinition))
		{
			if (MoveEntryTo(EntryId, Candidate->ContainerId, EmptySlot))
			{
				return true;
			}
		}
	}
	return bMovedAnything;
}

bool UInventory::EquipEntryToBestSlot(const FGuid& EntryId)
{
	if (!EntryId.IsValid())
	{
		return false;
	}

	const int32 SourceIndex = FindEntryIndexById(EntryId);
	if (SourceIndex == INDEX_NONE)
	{
		return false;
	}

	const FSinInventoryEntry SourceSnapshot = ItemInventory[SourceIndex];
	if (!SourceSnapshot.ItemDefinition)
	{
		return false;
	}

	const FGuid SourceContainerId = SourceSnapshot.ContainerId;
	const int32 StartingStack = SourceSnapshot.StackCount;
	bool bMovedAnything = false;

	TArray<const FSinInventoryContainerState*> Candidates;
	for (const FSinInventoryContainerState& SinContainer : Containers)
	{
		if (!SinContainer.ContainerId.IsValid())
		{
			continue;
		}

		if (SinContainer.ContainerId == SourceContainerId)
		{
			continue;
		}

		if (SinContainer.bAllowAutoAdd)
		{
			continue;
		}

		if (!DoesContainerAcceptItem(SinContainer.ContainerId, SourceSnapshot.ItemDefinition))
		{
			continue;
		}

		Candidates.Add(&SinContainer);
	}

	Candidates.Sort([](const FSinInventoryContainerState& A, const FSinInventoryContainerState& B)
	{
		return A.SortPriority < B.SortPriority;
	});

	for (const FSinInventoryContainerState* Candidate : Candidates)
	{
		if (FindEntryIndexById(EntryId) == INDEX_NONE)
		{
			return true;
		}

		const int32 Leftover = PourEntryIntoContainer(EntryId, Candidate->ContainerId);
		if (Leftover != StartingStack && Leftover != INDEX_NONE)
		{
			bMovedAnything = true;
		}

		if (Leftover == 0)
		{
			return true;
		}

		int32 EmptySlot = INDEX_NONE;
		if (FindFirstFreeSlotV2(Candidate->ContainerId, EmptySlot, SourceSnapshot.ItemDefinition))
		{
			if (MoveEntryTo(EntryId, Candidate->ContainerId, EmptySlot))
			{
				return true;
			}
		}
	}

	if (FindEntryIndexById(EntryId) != INDEX_NONE)
	{
		for (const FSinInventoryContainerState* Candidate : Candidates)
		{
			for (int32 SlotIndex = 0; SlotIndex < Candidate->SlotCount; ++SlotIndex)
			{
				if (!DoesContainerAcceptItemAtSlot(
					Candidate->ContainerId,
					SlotIndex,
					SourceSnapshot.ItemDefinition))
				{
					continue;
				}

				if (FindEntryIndexAtSlot(Candidate->ContainerId, SlotIndex) == INDEX_NONE)
				{
					continue;
				}

				if (MoveEntryTo(EntryId, Candidate->ContainerId, SlotIndex))
				{
					return true;
				}
			}
		}
	}
	return bMovedAnything;
}

// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay(); InitializeInventoryRuntimeState();
	for (EPrimaryItemType Source : TEnumRange<EPrimaryItemType>())
	{
		switch (Source)
		{
		case EPrimaryItemType::Regular:
		{
			if (BasicItems)
			{
				ItemDataTables.Add(Source, BasicItems);
			}
		}
		break;

		case EPrimaryItemType::Equipment:
		{
			if (EquipmentDT)
			{
				ItemDataTables.Add(Source, EquipmentDT);
			}
		}
		break;
		case EPrimaryItemType::Weapon:
		{
			if (WeaponsData)
			{
				ItemDataTables.Add(Source, WeaponsData);
			}
		}
		break;
		default:
			break;
		}
	}

	// ...
	
}

bool UInventory::CanModifyInventoryV2()
{
	if (!GetOwner()) { return false; } if (!GetOwner()->HasAuthority()) { return false; }
	return true;
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventory, Container);
}

USinInventory::USinInventory()
{
}

bool USinInventory::HasWallet()
{
	return true;
}

bool USinInventory::CanAfford(FCharStat BaseCost)
{
	for (auto It = SinWallet.Attributes.CreateConstIterator(); It; ++It)
	{
		if (It->Stat == BaseCost.Stat)
		{
			if (It->Value >= BaseCost.Value)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

void USinInventory::GiveMoney_Implementation(FCharStat Currency)
{
	FCharStats DeltaWallet = SinWallet;
	for (auto It = SinWallet.Attributes.CreateConstIterator(); It; ++It)
	{
		if (It->Stat == Currency.Stat)
		{
			SinWallet.Attributes[It.GetIndex()].Value += Currency.Value;
			OnRep_Wallet(DeltaWallet);
			return;
		}
	}
	SinWallet.Attributes.Add(Currency);
	OnRep_Wallet(DeltaWallet);
	return;
}

void USinInventory::AttemptMerchantTransaction_Implementation(UGameItemBase* Item)
{
	FCharStat BaseCost = { TAG_Item_Currency, Item->GetItemBaseValue()};
	if (!CanAfford(BaseCost))
	{
		return;
	}
	bool Added;
	TryAddItem(Item, -1, Added, Item->Owner->Container.Find(Item));
	if(Added)
	{
		FCharStats DeltaWallet = SinWallet;
		for (auto It = SinWallet.Attributes.CreateConstIterator(); It; ++It)
		{
			if (It->Stat == BaseCost.Stat)
			{
				SinWallet.Attributes[It.GetIndex()].Value += BaseCost.Value*-1.0f;
				OnRep_Wallet(DeltaWallet);
				return;
			}
		}
	}
}

void USinInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USinInventory, SinWallet);
}
