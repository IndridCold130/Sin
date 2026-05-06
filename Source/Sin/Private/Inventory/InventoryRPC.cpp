#include "Inventory/InventoryRPC.h"
#include "Inventory/Inventory.h"

USinInventoryRPC::USinInventoryRPC()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
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
