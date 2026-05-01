// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Inventory/Equipment.h"
#include "SinASC.generated.h"

class UAttributeSet;
class USinAttributeHealth;
struct FSinCombatInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityGiven, const FGameplayAbilitySpec&, AbilitySpec, UGameplayAbility*, AbilityInstance, bool, bAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityLevelChanged, UGameplayAbility*, Ability, int32, NewLevel, int32, OldLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSignalLooseTagsAdded, const FGameplayTag&, Tag, bool, TagExists);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSignalNewAttributeSetAdded, TSubclassOf<UAttributeSet>, AttributeClass, bool, Added);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSignalDamageDone, float, DamageAmount, const FHitResult&, HitInfo, const FGameplayTagContainer&, DamageTags, AActor*, Victim, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerksCacheChanged, const TArray<FSinClassPerks>&, OldPerks,const TArray<FSinClassPerks>&, NewPerks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSignalDamageReport, USinASC*, DamageDealer, USinASC*, Victim, const FSinCombatInstance&, DamageReport);

/**
 * 
 */
 // STRUCTURAS

USTRUCT(BlueprintType)
struct FBuffMetaData
{
	GENERATED_BODY()

	/** Handle to the applied gameplay effect */
	FActiveGameplayEffectHandle EffectHandle;

	/** Stack count of the buff */
	int32 StackCount;

	//**NewStuff*/
	TMap<FGameplayTag, int32> SourceCache;

	/** Default constructor */
	FBuffMetaData() : StackCount(0) {}

	/** Custom constructor to initialize with parameters */
	FBuffMetaData(FActiveGameplayEffectHandle InHandle, int32 InStackCount)
		: EffectHandle(InHandle), StackCount(InStackCount) {}

	/** Check if the buff is valid */
	bool IsValid() const
	{
		return EffectHandle.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FActiveBuffCollection
{
	GENERATED_BODY()

private:
	/** Map of active buffs */
	TMap<TSubclassOf<UGameplayEffect>, FBuffMetaData> ActiveBuffs;

public:
	/** Constructor */
	FActiveBuffCollection() {}
	/** Get the effect handle of an active buff */
	FActiveGameplayEffectHandle GetEffectHandle(TSubclassOf<UGameplayEffect> BuffClass) const
	{
		if (ActiveBuffs.Contains(BuffClass))
		{
			return ActiveBuffs[BuffClass].EffectHandle;
		}
		return FActiveGameplayEffectHandle(); // Return invalid handle if not found
	}

	/** Determines if a buff should be applied (returns true if new buff is needed) */
	void ShouldApplyBuff(TSubclassOf<UGameplayEffect> BuffClass, FGameplayTag Source, bool& ShouldCache, bool& ShouldApply)
	{
		if (!BuffClass) { ShouldCache = false; ShouldApply = false; return; }
		const UGameplayEffect* GameplayEffect = BuffClass->GetDefaultObject<UGameplayEffect>();
		if (ActiveBuffs.Contains(BuffClass))
		{
			if (ActiveBuffs[BuffClass].SourceCache.Contains(Source))
			{
				if (Source == TAG_Source_Gear)
				{
					ShouldCache = true;
				}
			}
			ShouldCache = true;
			ShouldApply = true;
			return;
			//if(GameplayEffect->StackingType==EGameplayEffectStackingType::None){ return false; }
			//return true; // Buff is already applied, no need to reapply
		}
	}

	int32 ShouldRemoveBuff(TSubclassOf<UGameplayEffect> BuffClass, FGameplayTag Source, int32 Stacks = 1)
	{
		if (!BuffClass || !ActiveBuffs.Contains(BuffClass)) return false;
		ActiveBuffs[BuffClass].StackCount = ActiveBuffs[BuffClass].StackCount - Stacks;
		if (ActiveBuffs[BuffClass].StackCount < 0) { ActiveBuffs.Remove(BuffClass); return Stacks; }
		if(Stacks)
		if (ActiveBuffs[BuffClass].SourceCache.Contains(Source))
		{
			ActiveBuffs[BuffClass].SourceCache[Source] = ActiveBuffs[BuffClass].SourceCache[Source] - Stacks;
			if (ActiveBuffs[BuffClass].SourceCache[Source] <= 0)
				ActiveBuffs[BuffClass].SourceCache.Remove(Source);
		}
		return Stacks;
	}

	void TrackBuff(TSubclassOf<UGameplayEffect> BuffClass, FGameplayTag Source, float Level, FActiveGameplayEffectHandle Handle)
	{
		if (!BuffClass) return;
		if (ActiveBuffs.Contains(BuffClass))
		{
			ActiveBuffs[BuffClass].StackCount++;
			if (!ActiveBuffs[BuffClass].SourceCache.Contains(Source))
			{
				ActiveBuffs[BuffClass].SourceCache.Add(Source, Level);
			}
			else { ActiveBuffs[BuffClass].SourceCache[Source] = ActiveBuffs[BuffClass].SourceCache[Source] + Level; }
		}
		else
		{
			ActiveBuffs.Add(BuffClass, { Handle, 1 });
		}
	}

	int32 GetStackCountTotal(TSubclassOf<UGameplayEffect>BuffClass)
	{
		if (!BuffClass || !ActiveBuffs.Contains(BuffClass))return 0;
		return ActiveBuffs[BuffClass].StackCount;
	}

	int32 GetStackCountFromSource(TSubclassOf<UGameplayEffect>BuffClass, FGameplayTag Source)
	{
		if (!BuffClass)return 0;
		if (ActiveBuffs[BuffClass].SourceCache.Contains(Source))
			return ActiveBuffs[BuffClass].SourceCache[Source];
		else return 0;
	}

	/** Removes a buff from tracking (should be called when ASC removes a buff) */
	void UntrackBuff(TSubclassOf<UGameplayEffect> BuffClass)
	{
		if (!BuffClass) return;
		ActiveBuffs.Remove(BuffClass);
	}
};
// STRUCTURAS

UCLASS()
class SIN_API USinASC : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	USinASC(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	TMap<FGameplayTag, FGameplayAttribute> AttributeBonusMap;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	TMap<FGameplayTag, FGameplayTag> ResistMapMults;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	TMap<FGameplayTag, FGameplayTag> ResistMapCaps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UAttributeSet>> AttributeClasses;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
	UPROPERTY()
	uint8 bAbilitiesInitialized : 1;
	UFUNCTION()
	void SustainedHealthDamage(float TimeLeft);
	UFUNCTION()
	void SustainedStaminaDamage(float TimeLeft);
	UFUNCTION()
	void SustainedManaDamage(float TimeLeft);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();
	UFUNCTION(BlueprintCallable, Category = "SinASC")
	FActiveGameplayEffectHandle GetFirstGameEffectHandleBasedOnTag(FGameplayTag GE_Tag, bool& Found);
	UFUNCTION(BlueprintPure, Category = "SinASC")
	FActiveGameplayEffectHandle GetHandleProgression();
	UFUNCTION(BlueprintPure, Category = "SinASC")
	FActiveGameplayEffectHandle GetHandleGear();
	UFUNCTION(BlueprintPure, Category = "SinASC")
	FActiveGameplayEffectHandle GetHandlePerk();
	UFUNCTION(BlueprintPure, Category = "SinASC")
	FActiveGameplayEffectHandle GetHandleWeapon(bool OffHand);
	UFUNCTION(BlueprintCallable)
	FGameplayEffectSpec GetGameEffectSpecBasedOnTag(FGameplayTag GE_Tag);
	UFUNCTION(BlueprintCallable, Category = "GAS_Class_Management")
	TArray<FActiveGameplayEffectHandle> GetAllJobHandles();
	UFUNCTION(BlueprintCallable, Category = "GAS_Class_Management")
	FSinLearnedClassWrapper& GetCachedClass(bool& Found, int32& Index, FActiveGameplayEffectHandle& ClassHandle);
	UFUNCTION(BlueprintCallable, Category = "GAS_Class_Management")
	TArray<FGameplayAbilitySpecHandle> GetClassLearnedAbilities(FActiveGameplayEffectHandle ClassHandle);
	UFUNCTION(BlueprintCallable, Category = "GAS_Class_Management")
	void GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> Abilities, const FActiveGameplayEffectHandle& ClassHandle);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FAsyncCoroutine GrantBonusPerks(TArray< FItemGrantedPerk> Perks, bool bGrant = true);
	UFUNCTION(BlueprintCallable, Category = "Passive_Abilities")
	void GrantPassiveAbilities(const TMap<TSubclassOf<USinGameplayAbility>, int32>& PassiveAbilities);
	UFUNCTION(BlueprintCallable, Category = "Passive_Abilities")
	void GrantPassiveAbility(TSubclassOf<USinGameplayAbility> Ability, int32 Level);
	UFUNCTION(BlueprintCallable, Category = "Passive_Abilities")
	void RemovePassiveAbilities(const TMap<TSubclassOf<USinGameplayAbility>, int32>& PassiveAbilities);
	UFUNCTION(BlueprintCallable, Category = "GAS_Class_Management")
	void CacheClassAbility(FActiveGameplayEffectHandle ClassHandle, FGameplayAbilitySpecHandle NewAbility);
	UFUNCTION(BlueprintCallable, Category = "GAS_Class_Management")
	bool RemoveCachedClass(FActiveGameplayEffectHandle ClassHandle);
	UFUNCTION(BlueprintPure, Category = "GameplayEffects")
	bool IsGameplayEffectHandleValid(FActiveGameplayEffectHandle Handle) const;
	UPROPERTY(BlueprintReadOnly, Category = "Gear")
	TObjectPtr<UEquipment> Gear;
	UFUNCTION(Category = "Gear")
	void ItemAdded(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory);

	UFUNCTION(Category = "Gear")
	void ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory);

	///** Lyra Systems //
	//UFUNCTION(BlueprintCallable, Category = "GAS Input")
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	//UFUNCTION(BlueprintCallable, Category = "GAS Input")
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable, Category = SinGameplayEffects)
	TArray<FActiveGameplayEffectHandle> SinApplyBuffsOfSource(FGameplayTag Source, TMap<TSubclassOf<USin_GAS_Buff>, int32> Buffs);
	UFUNCTION(BlueprintCallable, Category = SinGameplayEffects, meta = (DisplayName = "SinApplyGameplayEffectSpecToSelf", ScriptName = "SinApplyGameplayEffectSpecToSelf"))
	FActiveGameplayEffectHandle SinApplyBuff(FGameplayTag Source, TSubclassOf<UGameplayEffect> Buff, int32 Stack, FGameplayEffectContextHandle Context);
	UFUNCTION(BlueprintCallable, Category = SinGameplayEffects)
	int32 SinRemoveActiveEffectsWithTags(FGameplayTagContainer Tags, int32 Stacks=-1);
	UFUNCTION(BlueprintCallable)
	void AddNewAttribute(TSubclassOf<UAttributeSet> AttributeClass, bool Add);
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FSignalDamageDone SignalDamageDone;
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FSignalDamageReport SignalDamageReport;
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnPerksCacheChanged OnPerksChanged;
	UPROPERTY(BlueprintReadOnly)
	FActiveBuffCollection BuffManager;
	// Delegate that broadcasts when an ability is added or removed
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnAbilityGiven OnAbilityGiven;
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnAbilityLevelChanged OnAbilityLevelChanged;
	UFUNCTION(BlueprintCallable, Category = "SinASC")
	void AddByCallerMagnitude(FActiveGameplayEffectHandle Handle, TMap<FGameplayTag, float> Values, bool bAdd = true);
	UFUNCTION(BlueprintPure, Category = "SinASC")
	float GetSetByCallerMagnitude(FGameplayTag Tag, FGameplayTag TagMult = FGameplayTag());
	UFUNCTION(BlueprintPure, Category = "SinASC")
	float GetSetByCallerMagnitudeWithCap(FGameplayTag BaseTag, FGameplayTag MultTag, FGameplayTag CapBonusTag);
	UFUNCTION()
	float GetAttributeScalingCeiling(EPrimaryAttribute Attribute);
protected:
	virtual void BeginPlay() override;
	void InitializeAbilitySystem();
	void OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount);
	UFUNCTION(BlueprintCallable)
	void ManageAttributeBonuses(FGameplayTag GE_Tag, bool Apply, TMap<FGameplayTag, float> Mods);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefaultAbilities")
	TArray<TSubclassOf<UGameplayEffect>> BaseAttributes;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DefaultAbilities")
	TArray <TSubclassOf<UGameplayEffect>> DynamicAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DefaultAbilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LearnedAbilities", ReplicatedUsing = OnRep_LearnedHandles)
	TArray<FGameplayAbilitySpecHandle> LearnedHandles;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "LearnedAbilities", ReplicatedUsing = OnRep_LearnedClassAbilities)
	TArray < FSinLearnedClassWrapper> LearnedClassAbilities;
	UPROPERTY() // DYNAMIC BONUS HANDLES
	TMap<FGameplayTag, FActiveGameplayEffectHandle> DBH;
	UFUNCTION()
	float GetAttributeAdditiveBonusFromEffect(FActiveGameplayEffectHandle Handle, FGameplayAttribute Attribute);
	UFUNCTION()
	virtual void OnRep_LearnedHandles(const TArray<FGameplayAbilitySpecHandle>& OldValue);
	UFUNCTION()
	virtual void OnRep_LearnedClassAbilities(const TArray < FSinLearnedClassWrapper>& OldValue);
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	// TAG DELEGATE
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FSignalLooseTagsAdded SignalTagUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FSignalNewAttributeSetAdded SignalAttributeAdded;
	UPROPERTY(transient)
	TArray<TObjectPtr<UAttributeSet>> AddedAttributes;
	// Override OnGiveAbility to add custom behavior when an ability is granted
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	// LYRA HELPERS
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	//PERK SYSTEM

	UPROPERTY(ReplicatedUsing = OnRep_PerksCache, BlueprintReadOnly)
	TArray<FSinClassPerks> PerksCache;
	UFUNCTION()
	virtual void OnRep_PerksCache(const TArray<FSinClassPerks>& PreviousValue);
	UFUNCTION(BlueprintCallable)
	void PerkRegister(FGameplayTag ClassTag, FSinPerkRow Perk, int32 IncomingRanks, bool bBonus = false);
	UFUNCTION(BlueprintCallable)
	void PerkUnregister(FGameplayTag ClassTag, FGameplayTag PerkTag, int32 RanksToRemove = 1, bool bBonus = false);
	UFUNCTION(BlueprintCallable)
	void PerkAdd(FGameplayTag ClassTag, FSinPerkRow Perk, int32 Ranks =1, bool bBonus = false);
	UFUNCTION(BlueprintCallable)
	void PerkRemove(FGameplayTag ClassTag, FSinPerkRow Perk, int32 Ranks =1, bool bBonus = false);
	UFUNCTION(BlueprintPure)
	bool PerkMeetsRequirements(const FGameplayTag& ClassTag, const FSinPerkRow& Perk) const;
	//bool PerkMeetsRequirements(const TArray<FSinPerkRequirement>& Requirements) const;
	UFUNCTION(BlueprintCallable)
	int32 GetPerkRank(FGameplayTag ClassTag, FGameplayTag PerkTag) const;
	UFUNCTION(BlueprintCallable)
	int32 GetPerkRankBase(FGameplayTag ClassTag, FGameplayTag PerkTag) const;
	UFUNCTION(BlueprintCallable)
	void AddClassLevel(FGameplayTag ClassTag, int32 Levels = 1);

};
