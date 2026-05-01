// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SinGlobalStructs.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

#include "GameItemBase.generated.h"

class UInventory;
class USinGameplayEffect;
class USinPassiveAbility;
class USin_GAS_Buff;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class SIN_API UGameItemBase : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "GameItem")
		void Initialize();
		virtual void Initialize_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameItem")
		void OnItemInitOrMove(bool& CreateItem);
		virtual void OnItemInitOrMove_Implementation(bool& CreateItem);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameItem")
		void PostMoved();
		virtual void PostMoved_Implementation();

	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadWrite)
		UInventory* PreviousOwner;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadWrite)
		UInventory* Owner;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = true))
		FName ItemID;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Item")
		int32 PreviousIndex;

	UPROPERTY(Replicated, SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 CurrentIndex;

	UPROPERTY(ReplicatedUsing = OnRep_ItemPropertyChanged, SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 Stack = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
		int32 MaxStack = 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Item")
		EPrimaryItemType PrimaryType;

	UFUNCTION()
		void SetStack(int32 NewStack);

	UFUNCTION()
		void OnRep_ItemPropertyChanged();

	UFUNCTION()
		void OnRep_RemoveItem();

	UFUNCTION(BlueprintPure)
		FGameplayTag GetItemTypeTag();

	UFUNCTION(BlueprintPure)
		virtual float GetItemBaseValue();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		TSoftObjectPtr<UTexture2D> GetIcon();
		virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		int32 GetItemIntProperty(EItemIntStat Stat);
		virtual int32 GetItemIntProperty_Implementation(EItemIntStat Stat);
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		float GetItemFloatProperty(EItemFloatStat Stat);
		virtual float GetItemFloatProperty_Implementation(EItemFloatStat Stat);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void PlaySound(EItemSoundType Sound);
		virtual void PlaySound_Implementation(EItemSoundType Sound);
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		TMap<TSubclassOf<USin_GAS_Buff>, int32> GetBuffs();
		virtual TMap<TSubclassOf<USin_GAS_Buff>, int32> GetBuffs_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		TMap<TSubclassOf<USinGameplayAbility>, int32> GetBuffProcs();
		virtual TMap<TSubclassOf<USinGameplayAbility>, int32> GetBuffProcs_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		TArray< FItemGrantedPerk> GetBonusPerks();
		virtual TArray< FItemGrantedPerk> GetBonusPerks_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		float GetItemDurabilityPercentage();
		virtual float GetItemDurabilityPercentage_Implementation();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Float Stats")
		void HandleDurabilityDamage(ASinCharacter* HitCharacter);
		virtual void HandleDurabilityDamage_Implementation(ASinCharacter* HitCharacter);


		UFUNCTION()
		bool InitGameItemVars(FName ID, FGameplayTagContainer ItemType, UDataTable* Table, int32 Quantity = 1);
			//virtual void InitGameItemVars_Implementation(FName ID, FGameplayTagContainer ItemType, UDataTable* Table);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FItemBase DefaultData;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")
		FGameplayTagContainer ItemTags;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")
		UDataTable* DataTable;

		FTimerHandle StartTimer;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

private:
	virtual class UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;
};
