#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryRPC.generated.h"

class UInventory;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIN_API USinInventoryRPC : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USinInventoryRPC();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|RPC")
	void RequestTransferAll(UInventory* SourceInventory, UInventory* DestinationInventory);
	
protected:
	UFUNCTION(Server, Reliable)
	void Server_TransferAll(UInventory* SourceInventory, UInventory* DestinationInventory);

	void Server_TransferAll_Implementation(UInventory* SourceInventory, UInventory* DestinationInventory);
};