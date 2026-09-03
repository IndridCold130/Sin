// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryPanel.h"

#include "Widgets/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "Misc/SinLibrary.h"
#include "Components/UniformGridSlot.h" // Needed for setting slot padding
#include "SinPlayerController.h"
#include "SinCharacter.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/Widgets/InventoryContainerButton.h"
#include "Inventory/Widgets/SinItemContextMenu.h"
#include "Inventory/Widgets/SinPlayerInventoryView.h"
#include "Blueprint/SlateBlueprintLibrary.h"

UInventorySlot* UInventoryPanel::NavigateCachedSlot(EInventoryNavigationDirection Direction, bool bWrap,
	bool bSkipEmptySlots)
 {
	if (!SlotGrid){return nullptr;}

	const int32 ChildCount = SlotGrid->GetChildrenCount();

	if (ChildCount <= 0)
		{if (CachedSlot)
			{CachedSlot->OnHovered(false);}
		CachedSlot = nullptr;
		return nullptr;
		}

	TArray<UInventorySlot*> NavigableSlots;
	NavigableSlots.Reserve(ChildCount);

	for (int32 ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
	{
		UInventorySlot* SlotWidget = Cast<UInventorySlot>(SlotGrid->GetChildAt(ChildIndex));
		if (!SlotWidget)
		{
			continue;
		}

		bool bIsEmpty = false;
		if (bUseNewInventorySystem)
		{
			bIsEmpty = !SlotWidget->EntryId.IsValid();
		}
		else
		{
			bIsEmpty = (SlotWidget->SlottedItem == nullptr);
		}

		if (bSkipEmptySlots && bIsEmpty)
		{
			continue;
		}

		NavigableSlots.Add(SlotWidget);
	}

	if (NavigableSlots.IsEmpty())
	{
		if (CachedSlot)
		{
			CachedSlot->OnHovered(false);
		}

		CachedSlot = nullptr;
		return nullptr;
	}

	const int32 CurrentIndex = CachedSlot ? NavigableSlots.Find(CachedSlot) : INDEX_NONE;

	if (CachedSlot)
	{
		CachedSlot->OnHovered(false);
	}

	int32 Delta = 0;

	switch (Direction)
	{
	case EInventoryNavigationDirection::Right:
		Delta = 1;
		break;

	case EInventoryNavigationDirection::Left:
		Delta = -1;
		break;

	case EInventoryNavigationDirection::Down:
		Delta = FMath::Max(1, NavigationColumns);
		break;

	case EInventoryNavigationDirection::Up:
		Delta = -FMath::Max(1, NavigationColumns);
		break;

	default:
		Delta = 0;
		break;
	}

	int32 NewIndex = 0;

	if (CurrentIndex != INDEX_NONE)
	{
		NewIndex = CurrentIndex + Delta;

		if (bWrap)
		{
			const int32 Count = NavigableSlots.Num();

			while (NewIndex < 0)
			{
				NewIndex += Count;
			}

			while (NewIndex >= Count)
			{
				NewIndex -= Count;
			}
		}
		else
		{
			NewIndex = FMath::Clamp(NewIndex, 0, NavigableSlots.Num() - 1);
		}
	}

	CachedSlot = NavigableSlots[NewIndex];
	CachedSlot->OnHovered(true);

	return CachedSlot;
}

UInventorySlot* UInventoryPanel::CycleCachedSlot(bool bRight)
{
	return NavigateCachedSlot(
		bRight ? EInventoryNavigationDirection::Right : EInventoryNavigationDirection::Left,
		true,
		true
	);
}

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
	if (!SlotGrid)
	{
		return;
	}

	if (bItemList)
	{
		ManageInventorySlots(DataHolder ? DataHolder->InventorySize : 0);
	}
	else if (UInventorySlot* SlotWidget = FindSlotWidgetByInventoryIndex(Index))
	{
		SlotWidget->RefreshSlot(Item);
	}

	UPlayerInventoryView* InventoryParent = GetTypedOuter<UPlayerInventoryView>();
	if (InventoryParent)
	{
		float Weight = 0.0f;
		float EquipWeight = 0.0f;
		InventoryParent->RecalculateWeight(DataHolder, Weight, EquipWeight);
	}
	BP_OnInventoryRefreshed(NewInventory);
	BP_OnInventoryRefreshed(SrcInventory);
}

void UInventoryPanel::ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
	if (!SlotGrid)
	{
		return;
	}

	if (bItemList)
	{
		ManageInventorySlots(DataHolder ? DataHolder->InventorySize : 0);
	}
	else if (UInventorySlot* SlotWidget = FindSlotWidgetByInventoryIndex(Index))
	{
		SlotWidget->RefreshSlot(nullptr);
	}

	UPlayerInventoryView* InventoryParent = GetTypedOuter<UPlayerInventoryView>();
	if (InventoryParent)
	{
		float Weight = 0.0f;
		float EquipWeight = 0.0f;
		InventoryParent->RecalculateWeight(DataHolder, Weight, EquipWeight);
	}
	BP_OnInventoryRefreshed(NewInventory);
	BP_OnInventoryRefreshed(SrcInventory);
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
		DataHolder->OnInventoryRefreshed.RemoveDynamic(this, &UInventoryPanel::HandleInventoryRefreshed);
		DataHolder->OnContainerChanged.RemoveDynamic(this, &UInventoryPanel::HandleContainerChanged);
	}
}

void UInventoryPanel::SetInventoryData_Implementation(AActor* Owner)
{
	if (!Owner){return;}

	TArray<UInventory*> Inventories; Owner->GetComponents<UInventory>(Inventories);

	if (Inventories.IsEmpty()){return;}

	for (UInventory* LocalInventory : Inventories)
	{
		if (!LocalInventory || LocalInventory->InventoryType != InventoryType)
		{
			continue;
		}

		DataHolder = LocalInventory;

		if (bUseNewInventorySystem)
		{
			DataHolder->EnsureContainerIds();

			if (!DisplayedContainerId.IsValid())
			{
				for (const FSinInventoryContainerState& Container : DataHolder->Containers)
				{
					if (Container.ContainerId.IsValid())
					{
						DisplayedContainerId = Container.ContainerId;
						break;
					}
				}
			}

			RebuildContainerButtons();

			if (DisplayedContainerId.IsValid())
			{
				ManageInventorySlotsV2(DisplayedContainerId);
			}

			UpdateContainerButtonStates();

			if (!DataHolder->OnContainerChanged.IsAlreadyBound(this, &UInventoryPanel::HandleContainerChanged))
			{
				DataHolder->OnContainerChanged.AddDynamic(this, &UInventoryPanel::HandleContainerChanged);
			}
		}
		else
		{
			ManageInventorySlots(DataHolder->InventorySize);

			if (!DataHolder->OnSignalItemAdded.IsAlreadyBound(this, &UInventoryPanel::ItemAdded))
			{
				DataHolder->OnSignalItemAdded.AddDynamic(this, &UInventoryPanel::ItemAdded);
			}

			if (!DataHolder->OnSignalItemRemoved.IsAlreadyBound(this, &UInventoryPanel::ItemRemoved))
			{
				DataHolder->OnSignalItemRemoved.AddDynamic(this, &UInventoryPanel::ItemRemoved);
			}

			if (!DataHolder->OnInventoryRefreshed.IsAlreadyBound(this, &UInventoryPanel::HandleInventoryRefreshed))
			{
				DataHolder->OnInventoryRefreshed.AddDynamic(this, &UInventoryPanel::HandleInventoryRefreshed);
			}
		}

		if (Cast<ASinCharacter>(Owner))
		{
			if (UPlayerInventoryView* InventoryParent = GetTypedOuter<UPlayerInventoryView>())
			{
				float Weight = 0.0f;
				float EquipWeight = 0.0f;
				InventoryParent->RecalculateWeight(DataHolder, Weight, EquipWeight);
			}
		}
		return;
	}
}

void UInventoryPanel::HandleInventoryRefreshed(UInventory* RefreshedInventory)
{
	if (RefreshedInventory != DataHolder)
	{
		return;
	}

	ManageInventorySlots(DataHolder ? DataHolder->InventorySize : 0);
	BP_OnInventoryRefreshed(RefreshedInventory);
}

void UInventoryPanel::ManageInventorySlots(int32 Slots, bool bPreview)
{
	if (!SlotGrid)
	{
		return;
	}

	if (bItemList && !bPreview)
	{
		SlotGrid->ClearChildren();
	}

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
			LocalSlot->ApplyVisualSettings(SlotSize, IconSize);
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

UInventorySlot* UInventoryPanel::FindSlotWidgetByInventoryIndex(int32 InventoryIndex) const
{
	if (!SlotGrid)
	{
		return nullptr;
	}

	const int32 ChildCount = SlotGrid->GetChildrenCount();

	for (int32 ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
	{
		UInventorySlot* SlotWidget = Cast<UInventorySlot>(SlotGrid->GetChildAt(ChildIndex));
		if (SlotWidget && SlotWidget->SlotIndex == InventoryIndex)
		{
			return SlotWidget;
		}
	}

	return nullptr;
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

void UInventoryPanel::ManageInventorySlotsV2(const FGuid& ContainerId)
{
	if (!SlotGrid || !DataHolder || !InventorySlotClass)
	{
		return;
	}

	SlotGrid->ClearChildren();
	CachedSlot = nullptr;

	const FSinInventoryContainerState* ContainerState =
		DataHolder->FindContainerStateById(ContainerId);

	if (!ContainerState)
	{
		return;
	}

	auto CreateSlotWidget = [this, ContainerId, ContainerState](int32 SlotIndex, const FSinInventoryEntry* Entry) -> UInventorySlot*
	{
		UInventorySlot* LocalSlot = CreateWidget<UInventorySlot>(this, InventorySlotClass);
		if (!LocalSlot)
		{
			return nullptr;
		}

		LocalSlot->SlotIndex = SlotIndex;
		LocalSlot->MasterPanel = this;
		LocalSlot->ContainerId = ContainerId;
		LocalSlot->SlotType = ContainerState->ContainerTag;
		LocalSlot->ApplyVisualSettings(SlotSize, IconSize);
		LocalSlot->SetPadding(SlotPadding);

		if (Entry)
		{
			LocalSlot->RefreshSlotV2(*Entry);
		}
		else
		{
			LocalSlot->ClearSlotV2();
		}

		return LocalSlot;
	};

	auto AddToGrid = [this](UInventorySlot* LocalSlot, int32 GridIndex)
	{
		if (UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(
			LocalSlot,
			GridIndex / InventoryDivider,
			GridIndex % InventoryDivider))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	};

	if (bItemList)
	{
		TArray<const FSinInventoryEntry*> Occupied;
		for (const FSinInventoryEntry& Entry : DataHolder->ItemInventory)
		{
			if (Entry.ContainerId == ContainerId && Entry.ItemDefinition)
			{
				Occupied.Add(&Entry);
			}
		}

		Occupied.Sort([](const FSinInventoryEntry& A, const FSinInventoryEntry& B)
		{
			return A.SlotIndex < B.SlotIndex;
		});

		int32 VisibleIndex = 0;

		for (const FSinInventoryEntry* Entry : Occupied)
		{
			if (UInventorySlot* LocalSlot = CreateSlotWidget(Entry->SlotIndex, Entry))
			{
				AddToGrid(LocalSlot, VisibleIndex);

				if (CachedSlot == nullptr)
				{
					CachedSlot = LocalSlot;
					CachedSlot->OnHovered(true);
				}

				++VisibleIndex;
			}
		}

		while (VisibleIndex < MinVisibleSlots)
		{
			if (UInventorySlot* EmptySlot = CreateSlotWidget(INDEX_NONE, nullptr))
			{
				AddToGrid(EmptySlot, VisibleIndex);
				++VisibleIndex;
			}
			else
			{
				break;
			}
		}

		return;
	}

	int32 SlotsToCreate = ContainerState->SlotCount;

	if (ContainerState->bBottomless)
	{
		int32 HighestUsedSlot = INDEX_NONE;
		for (const FSinInventoryEntry& Entry : DataHolder->ItemInventory)
		{
			if (Entry.ContainerId == ContainerId)
			{
				HighestUsedSlot = FMath::Max(HighestUsedSlot, Entry.SlotIndex);
			}
		}
		SlotsToCreate = FMath::Max(ContainerState->MinVisibleSlots, HighestUsedSlot + 1);
	}

	for (int32 i = 0; i < SlotsToCreate; ++i)
	{
		const FSinInventoryEntry* MatchingEntry = nullptr;
		for (const FSinInventoryEntry& Entry : DataHolder->ItemInventory)
		{
			if (Entry.ContainerId == ContainerId && Entry.SlotIndex == i)
			{
				MatchingEntry = &Entry;
				break;
			}
		}

		if (UInventorySlot* LocalSlot = CreateSlotWidget(i, MatchingEntry))
		{
			AddToGrid(LocalSlot, i);
		}
	}
}

void UInventoryPanel::HandleContainerChanged(UInventory* Inventory, FGuid ContainerId)
{
	if (Inventory != DataHolder)
	{
		return;
	}

	if (ContainerId != DisplayedContainerId)
	{
		return;
	}

	ManageInventorySlotsV2(DisplayedContainerId);
}

void UInventoryPanel::SetDisplayedContainer(const FGuid& NewContainerId)
{
	if (!DataHolder || !NewContainerId.IsValid()){return;}

	if (DisplayedContainerId == NewContainerId)
	{
		return;
	}

	const FSinInventoryContainerState* ContainerState =
		DataHolder->FindContainerStateById(NewContainerId);

	if (!ContainerState)
	{
		return;
	}

	DisplayedContainerId = NewContainerId;

	ManageInventorySlotsV2(DisplayedContainerId);
	UpdateContainerButtonStates();
}

void UInventoryPanel::RebuildContainerButtons()
{
	if (!ContainerButtonBox || !DataHolder || !ContainerButtonClass)
	{
		return;
	}

	ContainerButtonBox->ClearChildren();

	for (const FSinInventoryContainerState& Container : DataHolder->Containers)
	{
		if (!Container.ContainerId.IsValid() || !Container.ContainerTag.IsValid())
		{
			continue;
		}

		if (!Container.bShowInContainerTabs)
		{
			continue;
		}

		UInventoryContainerButton* Button =
			CreateWidget<UInventoryContainerButton>(this, ContainerButtonClass);

		if (!Button)
		{
			continue;
		}

		Button->InitContainerButton(
			this,
			Container.ContainerId,
			Container.ContainerTag,
			Container.DisplayName
		);

		ContainerButtonBox->AddChild(Button);
	}
}

void UInventoryPanel::UpdateContainerButtonStates()
{
	if (!ContainerButtonBox)
	{
		return;
	}

	for (UWidget* Child : ContainerButtonBox->GetAllChildren())
	{
		if (UInventoryContainerButton* Button = Cast<UInventoryContainerButton>(Child))
		{
			Button->SetSelected(Button->ContainerId == DisplayedContainerId);
		}
	}
}

bool UInventoryPanel::TryShowItemContextMenuFromSlot(UInventorySlot* SourceSlot)
{
	if (!GetOwningPlayer() || !SourceSlot || !ContextMenuClass || !DataHolder || !SourceSlot->EntryId.IsValid())
	{
		return false;
	}
	HideItemContextMenu(); APlayerController* PC = GetOwningPlayer();
	ActiveContextMenu = CreateWidget<USinItemContextMenu>(PC, ContextMenuClass); if (!ActiveContextMenu) {return false;}
	FVector2D MousePosition; PC->GetMousePosition(MousePosition.X, MousePosition.Y); MousePosition += FVector2D(12.f, 8.f);
	ActiveContextMenu->InitContextMenu(this, SourceSlot);
	ActiveContextMenu->AddToViewport(69);
	return true;
}

void UInventoryPanel::ShowItemContextMenu(UInventorySlot* SourceSlot)
{
	if (!SourceSlot || !ContextMenuClass)
	{
		return;
	}
	TryShowItemContextMenuFromSlot(SourceSlot);
}

void UInventoryPanel::HideItemContextMenu()
{
	if (ActiveContextMenu)
	{
		ActiveContextMenu->RemoveFromParent();
		ActiveContextMenu = nullptr;
	}
}
