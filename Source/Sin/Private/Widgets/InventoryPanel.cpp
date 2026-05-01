// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryPanel.h"
#include "Widgets/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "Misc/SinLibrary.h"
#include "Components/UniformGridSlot.h" // Needed for setting slot padding
#include "SinPlayerController.h"
#include "SinCharacter.h"

bool UInventoryPanel::HasSpace(FGameplayTag InventoryTag)
{
	return DataHolder->HasSpace();
}

bool UInventoryPanel::HasItemInInventorySlot(int32 InvSlot, FGameplayTag SlotType)
{
	if (!DataHolder) { return false; }
	if (DataHolder->Container.IsValidIndex(InvSlot))
	{
		if (DataHolder->Container[InvSlot] != nullptr)
		{
			return true;
		}
	}
	return false;
}

void UInventoryPanel::CacheLastHoveredSlot(UInventorySlot* HoveredSlot)
{
	CachedSlot = HoveredSlot;
}

bool UInventoryPanel::ToggleToolTip(int32 SlotIndex, FGameplayTag SlotType)
{
	if (HasItemInInventorySlot(SlotIndex, SlotType))
	{
		return true;
	}
	return false;
}

void UInventoryPanel::MoveItemClient(UInventoryPanel* Panel, int32 OldIndex, int32 DstIndex, FGameplayTag IncomingSlotType, FGameplayTag LocalSlotType)
{
	bool bHasItem;
	UGameItemBase* LocItem;
	Panel->DataHolder->HasItemAtIndex(OldIndex, bHasItem, LocItem);
	if (bHasItem)
	{
		ASinPlayerController* Controller = Cast<ASinPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			Controller->MoveItem(DataHolder, LocItem, DstIndex);
			return;
		}
	}
}

void UInventoryPanel::ItemAdded(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
		Cast<UInventorySlot>(SlotGrid->GetChildAt(Index))->RefreshSlot(Item);
		UPlayerInventoryView* InventoryParent = GetTypedOuter<UPlayerInventoryView>();
		if (InventoryParent)
		{
			float Weight=0.0f;
			float EquipWeight=0.0f;
			InventoryParent->RecalculateWeight(DataHolder, Weight, EquipWeight);
		}
}

void UInventoryPanel::ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
	Cast<UInventorySlot>(SlotGrid->GetChildAt(Index))->RefreshSlot(nullptr);
	UPlayerInventoryView* InventoryParent = GetTypedOuter<UPlayerInventoryView>();
	if (InventoryParent)
	{
		float Weight = 0.0f;
		float EquipWeight = 0.0f;
		InventoryParent->RecalculateWeight(DataHolder, Weight, EquipWeight);
	}
}

bool UInventoryPanel::OnDoubleClickedSlot(FGameplayTag SlotType, int32 SlotIndex)
{
	ASinPlayerController* LocalController = Cast<ASinPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!LocalController||!HasItemInInventorySlot(SlotIndex, SlotType)){return false;}
	ASinCharacter* PlayerPawn = Cast<ASinCharacter>(LocalController->GetPawn());
	if (!PlayerPawn){ return false;}
	ASinCharacter* PlayerCharacter = Cast<ASinCharacter>(PlayerPawn);
	if (!PlayerCharacter) { return false; }
	UGameItemBase* LocalItem = DataHolder->Container[SlotIndex];
	FGameplayTag ItemTag = LocalItem->GetItemTypeTag();
	if (DataHolder->GetOwner() == PlayerCharacter)
	{
		if (DataHolder->InventoryType.MatchesTag(TAG_Item_Equipment))
		{
			return MoveItemOnDoubleClick(LocalController, LocalItem, PlayerCharacter->GetInventoryByTag(TAG_Item), SlotIndex);
		}
		else
		{
			if (ItemTag.MatchesTag(TAG_Item_Equipment_Weapon_OffHand))
			{
				return MoveItemOnDoubleClick(LocalController, LocalItem, PlayerCharacter->GetInventoryByTag(TAG_Item_Equipment_Weapon_OffHand), SlotIndex);
			}
			if (ItemTag.MatchesTag(TAG_Item_Equipment_Weapon))
			{
				return MoveItemOnDoubleClick(LocalController, LocalItem, PlayerCharacter->GetInventoryByTag(TAG_Item_Equipment_Weapon), SlotIndex);
			}
			if(ItemTag.MatchesTag(TAG_Item_Equipment))
			{
				return MoveItemOnDoubleClick(LocalController, LocalItem, PlayerCharacter->GetInventoryByTag(TAG_Item_Equipment), SlotIndex);
			}
			if (ItemTag.MatchesTag(TAG_Item_Equipment_Utility))
			{
				return MoveItemOnDoubleClick(LocalController, LocalItem, PlayerCharacter->GetInventoryByTag(TAG_Item_Equipment_Utility), SlotIndex);
			}
		}
	}
	else
	{
		return MoveItemOnDoubleClick(LocalController, LocalItem, PlayerCharacter->GetInventoryByTag(TAG_Item), SlotIndex);
	}
	return false;
}

bool UInventoryPanel::MoveItemOnDoubleClick(ASinPlayerController* Controller, UGameItemBase* Item, UInventory* NewInventory, int32 SlotIndex)
{
	if (NewInventory->HasSpace())
	{
		if (NewInventory->InventoryType.MatchesTag(TAG_Item_Equipment))
		{
			for (int32 i = 0; i < NewInventory->InventorySize; i++)
			{
				if (NewInventory->SlotRules(Item, i))
				{
					bool Occupied;
					UGameItemBase* TargetIndexItem;
					NewInventory->HasItemAtIndex(i, Occupied, TargetIndexItem);
					if(!Occupied)
					{
						Controller->TryAddItemPRC(NewInventory, Item, i, SlotIndex);
						return true;
					}
				}
			}
		}
		else 
		{
			Controller->TryAddItemPRC(NewInventory, Item, -1, SlotIndex);
			return true;
		}
	}
	return false;
}

void UInventoryPanel::AttemptSplitStack(FGameplayTag SlotType, int32 SlotIndex, int32 Amount)
{
	UGameItemBase* LocalItem = DataHolder->Container[SlotIndex];
		if (LocalItem)
		{
			if (LocalItem->GetItemIntProperty(EItemIntStat::Stack) > 1)
			{
				bool Found;
				int32 FoundIndex;
				DataHolder->FindFirstFreeSlot(Found, FoundIndex, SlotIndex);
				if(Found)
				{
					int32 InitialStack = LocalItem->GetItemIntProperty(EItemIntStat::Stack);
					int32 Quantity;
					if (Amount < 0)
					{
						Quantity = InitialStack / 2;
					}
					else
					{
						Quantity = Amount;
					}
					DataHolder->CreateItemPRC(LocalItem->ItemID, LocalItem->PrimaryType, Quantity, FoundIndex);
					ASinPlayerController* LocalController = Cast<ASinPlayerController>(GetWorld()->GetFirstPlayerController());
					LocalController->SetStackPRC(LocalItem, InitialStack - Quantity);
					return;
				}
			}
			return;
		}
}

void UInventoryPanel::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryPanel::NativeDestruct()
{
	Super::NativeDestruct();
	if (DataHolder) 
	{
		DataHolder->OnSignalItemAdded.RemoveDynamic(this, &UInventoryPanel::ItemAdded);
		DataHolder->OnSignalItemRemoved.RemoveDynamic(this, &UInventoryPanel::ItemRemoved);
	}
}

void UInventoryPanel::SetInventoryData(AActor* Owner)
{
	if(!Owner)
	{
		return;
	}
	TArray<UInventory*> Inventories;
	Owner->GetComponents<UInventory>(Inventories);
	if(Inventories.IsEmpty())
	{
		return;
	}
	for(UInventory* LocalInventory:Inventories)
	{
		if (LocalInventory->InventoryType == InventoryType) 
		{ 
			DataHolder = LocalInventory; 
			ManageInventorySlots(DataHolder->InventorySize);
			if (!DataHolder->OnSignalItemAdded.IsAlreadyBound(this, &UInventoryPanel::ItemAdded))
			{
				DataHolder->OnSignalItemAdded.AddDynamic(this, &UInventoryPanel::ItemAdded);
			}
			if (!DataHolder->OnSignalItemRemoved.IsAlreadyBound(this, &UInventoryPanel::ItemRemoved))
			{
				DataHolder->OnSignalItemRemoved.AddDynamic(this, &UInventoryPanel::ItemRemoved);
			}
			ASinCharacter* Character = Cast<ASinCharacter>(Owner);
			if (Character)
			{
				UPlayerInventoryView* InventoryParent = GetTypedOuter<UPlayerInventoryView>();
				if (InventoryParent)
				{
					float Weight = 0.0f;
					float EquipWeight = 0.0f;
					InventoryParent->RecalculateWeight(DataHolder, Weight, EquipWeight);
				}
			}
			return;
		};
	}	
}

void UInventoryPanel::ManageInventorySlots(int32 Slots, bool bPreview)
{
	//SlotGrid->ClearChildren();
	TArray<UInventorySlot*> LocalSlots;
	GetWidgetsOfClassUnderParent(UInventorySlot::StaticClass(), LocalSlots);
	int32 Skippabili = 0;
	if (LocalSlots.IsEmpty()) {
		for (int32 i = 0; i <Slots; i++)
		{
			if (bItemList&&!HasItemInInventorySlot(i, InventoryType))
			{
				Skippabili += 1;
				continue;
			}
			UInventorySlot* LocalSlot;
			LocalSlot = CreateWidget<UInventorySlot>(this, InventorySlotClass);
			LocalSlot->SlotIndex = i;
			LocalSlot->MasterPanel = this;
			LocalSlot->SlotType = InventoryType;
			if (!DefaultPreviewIcon.IsNull()&& LocalSlot->PreviewIconOverride.IsNull())
			{
				LocalSlot->PreviewIconOverride = DefaultPreviewIcon;
				LocalSlot->SetPreviewIcon(true);
			}
			int32 SlotPosition = FMath::Max(i - Skippabili, 0);
			SlotGrid->AddChildToUniformGrid(LocalSlot, SlotPosition / InventoryDivider, SlotPosition % InventoryDivider);
			LocalSlot->SetPadding(SlotPadding);
			if (!bPreview)
			{
				if ((DataHolder->Container.IsValidIndex(i)))
				{
					if ((DataHolder->Container[i]))
					{
						if ((DataHolder->Container[i] != nullptr))
						{
							LocalSlot->RefreshSlot((DataHolder->Container[i]));
						}
					}
				}
			}
		}
	}
	if (bPreview) { return; }
	else {
		for (UInventorySlot* LocalSlot : LocalSlots)
		{
			LocalSlot->SetPadding(SlotPadding);
			if (DataHolder->Container.IsValidIndex(LocalSlot->SlotIndex)) 
			{
				LocalSlot->RefreshSlot((DataHolder->Container[LocalSlot->SlotIndex]));
			}
			LocalSlot->MasterPanel = this;
		}
	}
}

void UInventoryPanel::GetWidgetsOfClassUnderParent(TSubclassOf<UInventorySlot> WidgetClass, TArray<UInventorySlot*>& FoundWidgets)
{
	TArray<UWidget*> LocalWidgets = SlotGrid->GetAllChildren();
	Algo::Transform(LocalWidgets, FoundWidgets, [](UWidget* Widget) -> UInventorySlot*
		{
			return Cast<UInventorySlot>(Widget);
		});
	FoundWidgets.RemoveAll([](UInventorySlot* ConvertSlot) { return ConvertSlot == nullptr; });
	if (FoundWidgets.IsEmpty())
	{
		for (int32 i = 0; i < SlotGrid->GetChildrenCount(); i++)
		{
			UWidget* Child = SlotGrid->GetChildAt(i);
			if (!Child) continue;
			// If the child is of the desired class, add it to the array
			UInventorySlot* UserWidget = Cast<UInventorySlot>(Child);
			if (UserWidget && UserWidget->IsA(WidgetClass))
			{
				FoundWidgets.Add(UserWidget);
			}
			// Recursively search inside the child in case it's a container
			GetWidgetsOfClassUnderParent(WidgetClass, FoundWidgets);
		}
	}
}

void UPlayerInventoryView::RecalculateWeight_Implementation(UInventory* Holder, float& BackpackWeight, float& EquipLoad)
{
	if (!Holder) { BackpackWeight = 0.0f; EquipLoad = 0.0f; return; }
	ASinCharacter* Character = Cast<ASinCharacter>(Holder->GetOwner());
	if (Character)
	{
		BackpackWeight = Character->GetBackpackWeight();
		EquipLoad = Character->GetEquipLoad();
	}
}
