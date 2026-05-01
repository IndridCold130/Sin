// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GameItemBase.h"

#include "Misc/SinLibrary.h"
#include "GAS/Abilities/SinGameplayAbility.h"
#include "Interfaces/I_CombatInterface.h"
#include "Inventory/Inventory.h"
#include "GAS/Effects/Sin_GAS_Buff.h"

void UGameItemBase::Initialize_Implementation()
{
	if (DataTable && Owner->Container.Contains(this))
	{
		bool CreateItem;
		OnItemInitOrMove_Implementation(CreateItem);
		if(CreateItem)
		{
			FItemBase* LocDefaultData = DataTable->FindRow<FItemBase>(ItemID, TEXT("ContextString"), true);
			if (LocDefaultData)
			{
				DefaultData = *LocDefaultData;
				ItemTags = DefaultData.ItemTags;
			}
			Owner->OnSignalItemAdded.Broadcast(Owner, CurrentIndex, this, PreviousIndex, PreviousOwner);
		}
	}
}

void UGameItemBase::OnItemInitOrMove_Implementation(bool& CreateItem)
{
	if(ItemTags.HasTag(TAG_Item_Currency))
	{
		if (Owner->HasWallet())
		{
			FCharStat LocalCurrency;
			FGameplayTag LocalCurrencyTag;
			bool Found;
			USinLibrary::GetFirstMatchingGPT(ItemTags, TAG_Item_Currency, false, Found, LocalCurrencyTag);
			LocalCurrency.Stat = LocalCurrencyTag;
			LocalCurrency.Value += Stack;
			Owner->StoreCurrency(LocalCurrency);
			CreateItem = false;
		}
	}
	CreateItem = true;
}

void UGameItemBase::PostMoved_Implementation()
{
}

void UGameItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UGameItemBase, ItemID);
	DOREPLIFETIME(UGameItemBase, ItemTags);
	DOREPLIFETIME(UGameItemBase, DataTable);
	DOREPLIFETIME(UGameItemBase, PrimaryType);
	//DOREPLIFETIME_CONDITION(UGameItemBase, ItemTags, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(UGameItemBase, DataTable, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(UGameItemBase, PrimaryType, COND_OwnerOnly);
	DOREPLIFETIME(UGameItemBase, Owner);
	DOREPLIFETIME(UGameItemBase, PreviousOwner);
    DOREPLIFETIME(UGameItemBase, CurrentIndex);
	DOREPLIFETIME(UGameItemBase, PreviousIndex);
    DOREPLIFETIME(UGameItemBase, Stack);
}

UWorld* UGameItemBase::GetWorld() const
{
	// Return pointer to World from object owner, if we don’t work in editor
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr;
	}
	else if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
    return nullptr;
}

void UGameItemBase::PostInitProperties()
{
	Super::PostInitProperties();
	if (GetWorld())
	{
		Owner = Cast<UInventory>(GetOuter());
		const float Delay = 0.006f;
		GetWorld()->GetTimerManager().SetTimer(StartTimer, this, &UGameItemBase::Initialize, Delay, false);
	}
}

void UGameItemBase::SetStack(int32 NewStack)
{
	Stack = NewStack;
	if (Owner && GetWorld()->GetNetMode() == NM_Standalone)
	{
		Owner->HandleClient(true, CurrentIndex, this, PreviousIndex, PreviousOwner);
	}
}

void UGameItemBase::OnRep_ItemPropertyChanged()
{
	if (Owner)
	{	
		Owner->OnSignalItemAdded.Broadcast(Owner, CurrentIndex, this, PreviousIndex, PreviousOwner);
	}
}

void UGameItemBase::OnRep_RemoveItem()
{
	bool HasItem;
	UGameItemBase* LocItem;
	Owner->HasItemAtIndex(CurrentIndex, HasItem, LocItem);
	if (!HasItem)
	{
		//PreviousOwner->OnSignalItemRemoved.Broadcast(CurrentIndex, this, PreviousIndex, PreviousOwner);
	}

}

FGameplayTag UGameItemBase::GetItemTypeTag()
{
	bool LocFound;
	FGameplayTag MyTag;
	USinLibrary::GetFirstMatchingGPT(ItemTags, TAG_Item, false, LocFound, MyTag);
	return MyTag;
}

float UGameItemBase::GetItemBaseValue()
{
	return DefaultData.Price;
}

TSoftObjectPtr<UTexture2D> UGameItemBase::GetIcon_Implementation()
{
	return DefaultData.Icon;
}

int32 UGameItemBase::GetItemIntProperty_Implementation(EItemIntStat Stat)
{
	for (EItemIntStat Source : TEnumRange<EItemIntStat>())
	{
		switch (Stat)
		{
		case EItemIntStat::Stack:
			return Stack;
			break;
		case EItemIntStat::MaxStack:
			return DefaultData.MaxStack;
			break;
		default:
			return 0;
			break;
		}

	}
	return 0;
}

float UGameItemBase::GetItemFloatProperty_Implementation(EItemFloatStat Stat)
{
	switch (Stat)
	{
	case EItemFloatStat::Weight:
		return DefaultData.Weight; // Assuming you have a property for weight

	default:
		UE_LOG(LogTemp, Warning, TEXT("UGameItemBase::GetItemFloatProperty - Unsupported float stat requested: %d"), static_cast<int32>(Stat));
		return 0.0f; // Default value for unsupported properties
	}
}

void UGameItemBase::PlaySound_Implementation(EItemSoundType Sound)
{

}

TMap<TSubclassOf<USin_GAS_Buff>, int32> UGameItemBase::GetBuffs_Implementation()
{
	return TMap<TSubclassOf<USin_GAS_Buff>, int32>();
}

TMap<TSubclassOf<USinGameplayAbility>, int32> UGameItemBase::GetBuffProcs_Implementation()
{
	return TMap<TSubclassOf<USinGameplayAbility>, int32>();
}

TArray<FItemGrantedPerk> UGameItemBase::GetBonusPerks_Implementation()
{
	return TArray<FItemGrantedPerk>();
}

float UGameItemBase::GetItemDurabilityPercentage_Implementation()
{
	return 0.0f;
}

void UGameItemBase::HandleDurabilityDamage_Implementation(ASinCharacter* HitCharacter)
{
}

bool UGameItemBase::InitGameItemVars(FName ID, FGameplayTagContainer ItemType, UDataTable* Table, int32 Quantity)
{
	ItemTags = ItemType;
	Stack = FMath::Clamp(Quantity, 1, MaxStack);
	bool CreateItem;
	OnItemInitOrMove(CreateItem);
	if(CreateItem)
	{
		ItemID = ID;
		DataTable = Table;
		return true;
	}
	return false;
}
