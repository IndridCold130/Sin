// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"
#include "Net/UnrealNetwork.h"
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

void UInventory::CreateItemPRC_Implementation(FName ID, EPrimaryItemType Type,  int32 Quantity, int32 IntendedIndex)
{
	bool Created;
	UGameItemBase* LocalItem;
	CreateItem(ID, Type, Quantity, Created, LocalItem);
		if (Created)
		{
			TryAddItem(LocalItem, IntendedIndex, Created);
		}
}

bool UInventory::CreateItemPRC_Validate(FName ID, EPrimaryItemType Type, int32 Quantity, int32 IntendedIndex) { return true; }


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
		if (!Item) { return; }
		bool CreateItem;
		Item->OnItemInitOrMove_Implementation(CreateItem);
		if (!CreateItem) { return; }
		int32 OldIndex = Item->CurrentIndex;
		bool Moved;
		TryAddItem(Item, DstIndex, Moved, OldIndex);
		return;
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
}

// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();
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
