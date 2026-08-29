#include "Inventory/InventoryRPC.h"
#include "Inventory/Inventory.h"

USinInventoryRPC::USinInventoryRPC()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void USinInventoryRPC::BeginPlay()
{
	Super::BeginPlay();
	CacheAbilitySystemAndBindEvents();
}

void USinInventoryRPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearAbilitySystemBindings();
	Super::EndPlay(EndPlayReason);
}

void USinInventoryRPC::RequestTransferAll(UInventory* SourceInventory, UInventory* DestinationInventory)
{
	if (!SourceInventory || !DestinationInventory || SourceInventory == DestinationInventory)
	{
		return;
	}

	// If already authority, execute directly.
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Server_TransferAll(SourceInventory, DestinationInventory);
		return;
	}

	Server_TransferAll(SourceInventory, DestinationInventory);
}

void USinInventoryRPC::CacheAbilitySystemAndBindEvents()
{
	// Always clear old bindings first
	ClearAbilitySystemBindings();
	AActor* Owner = GetOwner(); if (!Owner){return;}
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		if (APawn* Pawn = PC->GetPawn()){CachedASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();}
	}
	else
	{
		CachedASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
	}
	if (!CachedASC){return;}
	CachedASC->GenericGameplayEventCallbacks.FindOrAdd(TAG_Input_QuickSlotUse)
		.AddUObject(this, &USinInventoryRPC::OnQuickSlotUseEvent);
	CachedASC->GenericGameplayEventCallbacks.FindOrAdd(TAG_Input_QuickSlotCycle)
		.AddUObject(this, &USinInventoryRPC::OnQuickSlotCycleEvent);
}

void USinInventoryRPC::ClearAbilitySystemBindings()
{
	if (CachedASC)
	{
		CachedASC->GenericGameplayEventCallbacks.FindOrAdd(TAG_Input_QuickSlotUse).RemoveAll(this);
		CachedASC->GenericGameplayEventCallbacks.FindOrAdd(TAG_Input_QuickSlotCycle).RemoveAll(this);
	}

	CachedASC = nullptr;
}

void USinInventoryRPC::OnQuickSlotUseEvent(const FGameplayEventData* Payload)
{
	TryUseSelectedQuickslot();
}

void USinInventoryRPC::OnQuickSlotCycleEvent(const FGameplayEventData* Payload)
{
	CycleQuickslot(+1);
}

void USinInventoryRPC::TryUseSelectedQuickslot()
{
}

void USinInventoryRPC::CycleQuickslot(int32 Direction)
{
}

void USinInventoryRPC::Server_TransferAll_Implementation(
	UInventory* SourceInventory,
	UInventory* DestinationInventory)
{
	if (!SourceInventory || !DestinationInventory || SourceInventory == DestinationInventory)
	{
		return;
	}

	SourceInventory->TransferAllTo(DestinationInventory);
}
