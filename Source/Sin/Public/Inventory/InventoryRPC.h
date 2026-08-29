#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"                  // ← FGameplayTag
#include "Abilities/GameplayAbilityTypes.h"        // ← FGameplayEventData
#include "InventoryRPC.generated.h"

class UInventory;
class UAbilitySystemComponent;
struct FGameplayEventData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIN_API USinInventoryRPC : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USinInventoryRPC();
	
	UFUNCTION(BlueprintCallable, Category="Inventory|RPC")
	void RequestTransferAll(UInventory* SourceInventory, UInventory* DestinationInventory);
	
	// ----- Quickslot -----
	UPROPERTY(BlueprintReadOnly, Category="Inventory|Quickslot")
	int32 SelectedQuickslotIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Quickslot")
	int32 NumQuickslots = 3;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/** Caches the ASC and (re)binds the quickslot gameplay events. Safe to call on repossess. */
	void CacheAbilitySystemAndBindEvents();
	void ClearAbilitySystemBindings();
	
	UFUNCTION(Server, Reliable)
	void Server_TransferAll(UInventory* SourceInventory, UInventory* DestinationInventory);
	void Server_TransferAll_Implementation(UInventory* SourceInventory, UInventory* DestinationInventory);
	
	// Event handlers
	void OnQuickSlotUseEvent(const FGameplayEventData* Payload);
	void OnQuickSlotCycleEvent(const FGameplayEventData* Payload);
	
	// Actual logic
	void TryUseSelectedQuickslot();
	void CycleQuickslot(int32 Direction = 1);
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;
};