// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SinASC.h"

#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Misc/SinGPTs.h"
#include "SinCharacter.h"
#include "GAS/SinAttributeHealth.h"
#include "GameplayCueManager.h"
#include "GAS/Effects/SinGameplayEffect.h"
#include "GAS/Abilities/SinGameplayAbility.h"
#include "Misc/SinLibrary.h"
#include "GAS/SinAttributeRage.h"
#include "GAS/Effects/Sin_GAS_Buff.h"
#include "Misc/SinGPTs.h"
#include "Misc/SinCommonLibrary.h"
#include "Inventory/SinInventoryEntryTypes.h"
#include "Inventory/SinInventoryContainerTypes.h"
#include "Inventory/Items/SinItemFragment_GrantedEffects.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_GrantedPerks.h"
#include "Inventory/Items/SinItemFragment_Stats.h"
//#include "UE5Coro.h"

USinASC::USinASC(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicated(true);
	SetReplicationMode(EGameplayEffectReplicationMode::Full);
	//BuffManager.Initialize(this); // Set the ASC reference
}

void USinASC::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		InitAbilityActorInfo(GetOwner(), GetOwner());
		InitializeAbilitySystem();
		Gear = Cast<UInventory>(
	GetOwner()->FindComponentByTag(
		UInventory::StaticClass(),
		FName("Backpack")
	)
);
		//Gear = Cast<UInventory>(GetOwner()->GetComponentByClass(UEquipment::StaticClass()));
		if (Gear)
		{
			//Gear->OnSignalItemAdded.AddDynamic(this, &USinASC::ItemAdded);
			//Gear->OnSignalItemRemoved.AddDynamic(this, &USinASC::ItemRemoved);
			
			Gear->OnInventoryEntryAdded.AddDynamic(this, &USinASC::HandleInventoryEntryAdded);
			Gear->OnInventoryEntryRemoved.AddDynamic(this, &USinASC::HandleInventoryEntryRemoved);
			Gear->OnInventoryEntryChanged.AddDynamic(this, &USinASC::HandleInventoryEntryChanged);
			Gear->OnContainerChanged.AddDynamic(this, &USinASC::HandleInventoryContainerChanged);
		}
	}
}

TArray<FActiveGameplayEffectHandle> USinASC::SinApplyBuffsOfSource(FGameplayTag Source, TMap<TSubclassOf<USin_GAS_Buff>, int32> Buffs)
{
	TArray<FActiveGameplayEffectHandle> Output;
	for (const auto& Pair : Buffs)
	{
		const TSubclassOf<USin_GAS_Buff>& BuffClass = Pair.Key;
		const int32 StackCount = Pair.Value;
		FActiveGameplayEffectHandle CurrentHandle = SinApplyBuff(Source, BuffClass, StackCount, MakeEffectContext());
		if (CurrentHandle.IsValid())
		{
			Output.Add(CurrentHandle);
		}
	}
	return Output;
}

FActiveGameplayEffectHandle USinASC::SinApplyBuff(FGameplayTag Source, TSubclassOf<UGameplayEffect> Buff, int32 Stack, FGameplayEffectContextHandle Context)
{
	FActiveGameplayEffectHandle OldBuffHandle = BuffManager.GetEffectHandle(Buff); FGameplayTagContainer AssetTags; FGameplayTagContainer DynamicTags;
	FGameplayEffectSpecHandle NewSpecHandle = MakeOutgoingSpec(Buff, 1.0, Context); FGameplayEffectSpec* Spec = NewSpecHandle.Data.Get();
	FActiveGameplayEffectHandle ReturnHandle;
	if (NewSpecHandle.IsValid())
	{
		Spec->SetStackCount(Stack);
		Spec->AddDynamicAssetTag(Source);
		ReturnHandle = ApplyGameplayEffectSpecToTarget(*NewSpecHandle.Data.Get(), this);
		if (ReturnHandle.IsValid())
		{
			const FActiveGameplayEffect* ActiveGE = GetActiveGameplayEffect(ReturnHandle); if (!ActiveGE) return ReturnHandle; const USinGameplayEffect* SinEffect = Cast<USinGameplayEffect>(ActiveGE->Spec.Def);ActiveGE->Spec.GetAllAssetTags(AssetTags);
			DynamicTags = ActiveGE->Spec.GetDynamicAssetTags();
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString::Printf(TEXT("Dynamic Tags: %s"), *DynamicTags.ToString()));
			if (DynamicTags.Num() == 1 && DynamicTags.HasTag(TAG_Source_Gear)) {};
			if (Source == TAG_Source_Gear) {};
			if (SinEffect)
			{
				OnGameplayEffectStackChangeDelegate(ReturnHandle)->AddUObject(this, &USinASC::OnActiveGameplayEffectStackChanged);
				SinEffect->SinGameplayEffectInitialized(this, ReturnHandle);
				if (SinEffect->StackingType == EGameplayEffectStackingType::AggregateBySource) { return ReturnHandle; }
				BuffManager.TrackBuff(Buff,Source, Stack, ReturnHandle);
			}
		}
	}
	return ReturnHandle;
}

void USinASC::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void USinASC::OnRep_LearnedHandles(const TArray<FGameplayAbilitySpecHandle>& OldValue)
{
}

void USinASC::AddNewAttribute(TSubclassOf<UAttributeSet> AttributeClass, bool Add)
{
	if (Add)
	{
		if (!GetAttributeSubobject(AttributeClass))
		{
			UAttributeSet* Attributes = NewObject<UAttributeSet>(GetOwner(), AttributeClass);
			AddSpawnedAttribute(Attributes);
			AddedAttributes.Add(Attributes);
			//GetOrCreateAttributeSubobject(AttributeClass);
			SignalAttributeAdded.Broadcast(AttributeClass, true);
			const USinAttributeRage* Rage = Cast<USinAttributeRage>(GetAttributeSubobjectChecked(AttributeClass));
			if (Rage)
			{
				SignalDamageDone.AddDynamic(Rage, &USinAttributeRage::HandleDamageDone);
			}
			return;
		}
	}
	for (UAttributeSet* AttributeSet : AddedAttributes)
	{
		if (AttributeSet->GetClass() == AttributeClass)
		{
			const USinAttributeRage* Rage = Cast<USinAttributeRage>(GetAttributeSubobjectChecked(AttributeClass));
			if (Rage)
			{
				SignalDamageDone.RemoveDynamic(Rage, &USinAttributeRage::HandleDamageDone);
			}
			RemoveSpawnedAttribute(AttributeSet);
			SignalAttributeAdded.Broadcast(AttributeClass, false);
			return;
		}
	}
}

void USinASC::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

int32 USinASC::SinRemoveActiveEffectsWithTags(FGameplayTagContainer Tags, int32 Stacks)
{
	int32 Count;
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(Tags);
	TArray<FActiveGameplayEffectHandle> ActiveGameplayEffectHandles = ActiveGameplayEffects.GetActiveEffects(Query);
	Count = ActiveGameplayEffectHandles.Num();
	for (auto It = ActiveGameplayEffectHandles.CreateConstIterator(); It; ++It)
	{
		const FActiveGameplayEffect* ActiveGE = GetActiveGameplayEffect(*It);
		const USinGameplayEffect* SinEffect = Cast<USinGameplayEffect>(ActiveGE->Spec.Def);
		if (SinEffect)
		{
			SinEffect->SinGameplayEffectEnd(this, *It, Stacks);
		}
		else
		{
			RemoveActiveGameplayEffect(*It, Stacks);
		}
	}
	return Count;
}

void USinASC::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	SignalTagUpdated.Broadcast(Tag, TagExists);
}

void USinASC::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec); // Call the base implementation first
	if (AbilitySpec.Ability)
	{
		OnAbilityGiven.Broadcast(AbilitySpec, AbilitySpec.Ability, true);
	}
}

void USinASC::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec); // Call the base implementation first
	if (AbilitySpec.Ability)
	{
		OnAbilityGiven.Broadcast(AbilitySpec, AbilitySpec.Ability, false);
	}
}

void USinASC::OnRep_PerksCache(const TArray<FSinClassPerks>& PerksOld)
{
	OnPerksChanged.Broadcast(PerksOld, PerksCache);
}

void USinASC::PerkRegister(FGameplayTag ClassTag, FSinPerkRow Perk, int32 IncomingRanks, bool bBonus)
{
	TArray<FSinClassPerks> PerksOld = PerksCache;
	bool bClassFound = false; bool bDidUpdate = false; for (FSinClassPerks& ClassPerks : PerksCache)
	{
		if (ClassPerks.Class == ClassTag)
		{
			bClassFound = true; bool bPerkFound = false;
			for (FSinPerk& KnownPerk : ClassPerks.KnownPerks)
			{
				if (KnownPerk.Perk == Perk.PerkTag)
				{
					if (bBonus)
					{
						KnownPerk.BonusRank += IncomingRanks;
					}
					else
					{
						KnownPerk.Rank += IncomingRanks;
					}
					bPerkFound = true; bDidUpdate = true;
					break;
				}
			}
			if (!bPerkFound)
			{
				FSinPerk NewPerk;
				NewPerk.Perk = Perk.PerkTag;
				NewPerk.Rank = bBonus ? 0 : IncomingRanks;
				NewPerk.BonusRank = bBonus ? IncomingRanks : 0;
				NewPerk.MaxRankBase = Perk.MaxRankBase;

				ClassPerks.KnownPerks.Add(NewPerk);
				bDidUpdate = true;
			}
			break;
		}
	}
	if (!bClassFound)
	{
		FSinClassPerks NewClassPerks; NewClassPerks.Class = ClassTag;

		FSinPerk NewPerk; NewPerk.Perk = Perk.PerkTag;
		NewPerk.Rank = bBonus ? 0 : IncomingRanks;  NewPerk.BonusRank = bBonus ? IncomingRanks : 0; NewPerk.MaxRankBase = Perk.MaxRankBase;
		NewClassPerks.KnownPerks.Add(NewPerk); PerksCache.Add(NewClassPerks);
		bDidUpdate = true;
	}
	if (bDidUpdate)
	{
		OnPerksChanged.Broadcast(PerksOld, PerksCache);
	}
}

void USinASC::PerkUnregister(FGameplayTag ClassTag, FGameplayTag PerkTag, int32 RanksToRemove, bool bBonus)
{
	TArray<FSinClassPerks> PerksOld = PerksCache; bool bDidUpdate = false;
	for (FSinClassPerks& ClassPerks : PerksCache)
	{
		if (ClassPerks.Class == ClassTag)
		{
			for (int32 i = 0; i < ClassPerks.KnownPerks.Num(); ++i)
			{
				FSinPerk& KnownPerk = ClassPerks.KnownPerks[i]; if (KnownPerk.Perk == PerkTag)
				{
					if (bBonus)
					{
						KnownPerk.BonusRank = FMath::Max(0, KnownPerk.BonusRank - RanksToRemove);
					}
					else
					{
						KnownPerk.Rank = FMath::Max(0, KnownPerk.Rank - RanksToRemove);
					}
					bDidUpdate = true;
					// Remove perk entirely if both ranks are zero
					if (KnownPerk.Rank == 0 && KnownPerk.BonusRank == 0)
					{
						ClassPerks.KnownPerks.RemoveAt(i);
					}
					break;
				}
			}
			// Optionally: remove empty class entry
			//if (ClassPerks.KnownPerks.Num() == 0&& ClassPerks.Level<1|| ClassPerks.Level < 1)
			if (ClassPerks.Level < 1)
			{
				int32 ClassIndex = PerksCache.IndexOfByPredicate(
					[&](const FSinClassPerks& Entry) { return Entry.Class == ClassTag; }
				);

				if (ClassIndex != INDEX_NONE)
				{
					PerksCache.RemoveAt(ClassIndex);
				}
			}
			break; // Found class, done
		}
	}
	if (bDidUpdate)
	{
		OnPerksChanged.Broadcast(PerksOld, PerksCache);
	}
}

void USinASC::PerkAdd(FGameplayTag ClassTag, FSinPerkRow Perk, int32 Ranks, bool bBonus)
{
	// Check if perk can be added safety
	if (!bBonus && !PerkMeetsRequirements(ClassTag, Perk)) return;

	// Register perk
	FActiveGameplayEffectHandle LocalHandle = GetHandlePerk(); if (!LocalHandle.IsValid()) return;
	for (int32 i = 0; i < Ranks; ++i)
	{
		int32 CurrentRank = GetPerkRank(ClassTag, Perk.PerkTag);
		PerkRegister(ClassTag, Perk, 1, bBonus);

		switch (Perk.PerkType)
		{
			case EPerkType::Passive:
				if (Perk.PassivePerks.IsValidIndex(CurrentRank))
				{
					AddByCallerMagnitude(LocalHandle, Perk.PassivePerks[CurrentRank].PassiveEffects);
				}
				break;
			case EPerkType::Proc:
				{
					TMap<TSubclassOf<USinGameplayAbility>, int32> Abilities;
					Abilities.Add(Perk.Proc.ProcAbility, 1);
					GrantPassiveAbilities(Abilities);
					//SinApplyBuffsOfSource(TAG_Source_Perk, Abilities);
					break;
				}
			case EPerkType::UniqueEffect:
				{
					if (Perk.UniqueEffect)
					{
						FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Perk.UniqueEffect, 1.f, MakeEffectContext());
						if (SpecHandle.IsValid())
						{
							ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
						}
					}
					break;
				}
		}
	}
}

void USinASC::PerkRemove(FGameplayTag ClassTag, FSinPerkRow Perk, int32 Ranks, bool bBonus)
{
	FActiveGameplayEffectHandle LocalHandle = GetHandlePerk(); if (!LocalHandle.IsValid()) return;
	for (int32 i = 0; i < Ranks; ++i)
	{
		int32 CurrentRank = GetPerkRank(ClassTag, Perk.PerkTag); int32 RankToRemove = FMath::Max(CurrentRank - 1, 0);
		switch (Perk.PerkType)
		{
		case EPerkType::Passive:
			if (Perk.PassivePerks.IsValidIndex(RankToRemove))
			{
				AddByCallerMagnitude(LocalHandle, Perk.PassivePerks[RankToRemove].PassiveEffects, /*bAdd=*/false);
			}
			break;
		case EPerkType::Proc:
			{
				// Remove the applied ability / buff
				TMap<TSubclassOf<USinGameplayAbility>, int32> Buffs;
				Buffs.Add(Perk.Proc.ProcAbility, 1);
								// Remove any passive abilities this proc perk granted
				if (Buffs.Num() > 0)
				{
					RemovePassiveAbilities(Buffs);
				}
			}
			break;
		}
		PerkUnregister(ClassTag, Perk.PerkTag, 1, bBonus); // Moved to end of loop
	}
}

bool USinASC::PerkMeetsRequirements(const FGameplayTag& ClassTag, const FSinPerkRow& Perk) const
{
	TArray<FSinPerkRequirement> Requirements;
	//int32 Rank = GetPerkRank(ClassTag, Perk.PerkTag);
	int32 BaseRank = GetPerkRankBase(ClassTag, Perk.PerkTag);
	if (BaseRank >= Perk.MaxRankBase) return false;

	switch (Perk.PerkType)
	{
	case EPerkType::Passive:
		if (Perk.PassivePerks.IsValidIndex(BaseRank))
		{
			Requirements.Add(Perk.PassivePerks[BaseRank].Requirements);
			//FSinPerkRequirement Requirement = Perk.PassivePerks[BaseRank].Requirements;
			//Requirements = Perk.PassivePerks[BaseRank].Requirements;
		}
		else
		{
			return true;
		}
		break;

	case EPerkType::Proc:
		if (Perk.Proc.Requirements.IsValidIndex(BaseRank))
		{
			Requirements = Perk.Proc.Requirements;
			//Requirements = Perk.Proc.Ranks[Rank].Requirements;
		}
		else
		{
			return true;
		}
		break;

	case EPerkType::UniqueEffect:
		// No requirements for now
		break;
	case EPerkType::Active:
		// No requirements for now
		break;

	default:
		break;
	}

	//for (const FSinPerkRequirement& Requirement : Requirements)
	for (int32 i = 0; i < FMath::Min(Requirements.Num(), BaseRank+1); ++i)
	{
		const FSinPerkRequirement& Requirement = Requirements[i];

		bool bFound = false;

		for (const FSinClassPerks& ClassPerks : PerksCache)
		{
			// Always validate class level
			if (ClassPerks.Level < Requirement.RequiredClassLevel)
			{
				return false;
			}

			// If no specific perk is required, class level suffices
			if (!Requirement.RequiredPerk.IsValid())
			{
				bFound = true;
				break;
			}

			// Otherwise check for required perk
			for (const FSinPerk& KnownPerk : ClassPerks.KnownPerks)
			{
				if (KnownPerk.Perk == Requirement.RequiredPerk &&
					KnownPerk.GetEffectiveRank() >= Requirement.RequiredRank)
				{
					bFound = true;
					break;
				}
			}

			if (bFound)
				break;
		}

		if (!bFound)
		{
			return false;
		}
	}

	return true;
}

int32 USinASC::GetPerkRank(FGameplayTag ClassTag, FGameplayTag PerkTag) const
{
	for (FSinClassPerks ClassPerks : PerksCache)
	{
		if (ClassPerks.Class == ClassTag)
		{
			for (FSinPerk& KnownPerk : ClassPerks.KnownPerks)
			{
				if (KnownPerk.Perk == PerkTag)
				{
					//return KnownPerk.Rank;
					return KnownPerk.GetEffectiveRank();
				}
			}
		}
	}
	return 0;
}

int32 USinASC::GetPerkRankBase(FGameplayTag ClassTag, FGameplayTag PerkTag) const
{
	for (FSinClassPerks ClassPerks : PerksCache)
	{
		if (ClassPerks.Class == ClassTag)
		{
			for (FSinPerk& KnownPerk : ClassPerks.KnownPerks)
			{
				if (KnownPerk.Perk == PerkTag)
				{
					//return KnownPerk.Rank;
					return KnownPerk.GetBaseRank();
				}
			}
		}
	}
	return 0;
}

void USinASC::AddClassLevel(FGameplayTag ClassTag, int32 Levels)
{
	for (FSinClassPerks& ClassPerks : PerksCache)
	{
		if (ClassPerks.Class == ClassTag)
		{
			ClassPerks.Level += Levels;
			return;
		}
	}

	// If not found, create a new entry
	FSinClassPerks NewClass;
	NewClass.Class = ClassTag;
	NewClass.Level = Levels;
	PerksCache.Add(NewClass);
}
void USinASC::InitializeAbilitySystem()
{
	if (AttributeClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}
	
	for (TSubclassOf<UAttributeSet> AttributeClass : AttributeClasses)
	{
	GetOrCreateAttributeSubobject(AttributeClass);
	}
	BaseAttributes.Append(DynamicAttributes);
	for (const TSubclassOf<UGameplayEffect>& GameplayEffect : BaseAttributes)
	{
		FGameplayEffectContextHandle EffectContext = MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FGameplayTag AssetTag;
			//FGameplayTagContainer AssetTags = GameplayEffect->GetDefaultObject<UGameplayEffect>()->InheritableGameplayEffectTags.CombinedTags;
			FGameplayTagContainer AssetTags = GameplayEffect->GetDefaultObject<UGameplayEffect>()->GetAssetTags();
			if (!AssetTags.IsEmpty())
			{
				AssetTag = AssetTags.First(); // Or use a specific search if needed
				FActiveGameplayEffectHandle ActiveGEHandle = SinApplyBuff(AssetTag, GameplayEffect, 1, EffectContext);
				if (!DBH.Contains(AssetTag))
				{
					DBH.Add(AssetTag, ActiveGEHandle);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("GameplayEffect %s has no asset tags!"), *GameplayEffect->GetName());
				continue;
			}
			//FActiveGameplayEffectHandle ActiveGEHandle =SinApplyBuff(TAG_Gear, GameplayEffect, 1, EffectContext);
			//DBH.Add(TAG_Gear, ActiveGEHandle);
		}
	}
	for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
	{
		if (!IsValid(StartupAbility))
		{
			continue;
		}
		UGameplayAbility* AbilityCDO = StartupAbility->GetDefaultObject<UGameplayAbility>();
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);
		AbilitySpec.SourceObject = this;
		bool LocFound;
		FGameplayTag MyTag;
		USinLibrary::GetFirstMatchingGPT(AbilityCDO->AbilityTags, TAG_Input, false, LocFound, MyTag);
		if (LocFound)
		{
			AbilitySpec.DynamicAbilityTags.AddTag(MyTag);
		}
		const FGameplayAbilitySpecHandle AbilitySpecHandle = GiveAbility(AbilitySpec);
	}
	bAbilitiesInitialized = true;
}
void USinASC::OnActiveGameplayEffectStackChanged(FActiveGameplayEffectHandle ActiveHandle, int32 NewStackCount, int32 PreviousStackCount)
{
}

//*Special Operations*/

float USinASC::GetAttributeAdditiveBonusFromEffect(FActiveGameplayEffectHandle Handle, FGameplayAttribute Attribute)
{
	const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle);
	float Bonus = 0.0f;

	if (!ActiveEffect)
	{
		return Bonus;
	}
		const FGameplayEffectSpec& Spec = ActiveEffect->Spec;
		const UGameplayEffect* GECDO = Spec.Def; // The actual GE asset (Class Default Object)
		for (int32 i = 0; i < Spec.Modifiers.Num(); ++i)
		{
			const FModifierSpec& ModSpec = Spec.Modifiers[i];
			const FGameplayModifierInfo& ModInfo = GECDO->Modifiers[i]; // Corresponding modifier info

			if (ModInfo.ModifierOp == EGameplayModOp::Additive && ModInfo.Attribute == Attribute)
			{
				Bonus += ModSpec.GetEvaluatedMagnitude();
			}
		}
	return Bonus;
}

void USinASC::AddByCallerMagnitude(FActiveGameplayEffectHandle Handle, TMap<FGameplayTag, float> Values, bool bAdd)
{
	if (!Handle.IsValid()) return;
	const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle);
	FGameplayEffectSpec* Spec = const_cast<FGameplayEffectSpec*>(&ActiveEffect->Spec);
	if (!Spec){ UE_LOG(LogTemp, Warning, TEXT("AddByCallerMagnitude: Spec is null")); return;
	}
	for (const TPair<FGameplayTag, float>& Pair : Values)
	{
		const FGameplayTag& Tag = Pair.Key; const float AdditionalValue = Pair.Value;
		float* ExistingValuePtr = Spec->SetByCallerTagMagnitudes.Find(Tag);
		if (ExistingValuePtr)
		{
			*ExistingValuePtr += bAdd ? AdditionalValue : AdditionalValue*-1.f;
			UpdateActiveGameplayEffectSetByCallerMagnitude(Handle, Tag, *ExistingValuePtr);
		}
		else
		{
			Spec->SetSetByCallerMagnitude(Tag, AdditionalValue);
		}
	}
}

float USinASC::GetSetByCallerMagnitude(FGameplayTag Tag, FGameplayTag TagMult)
{
	float Value = 0.f;
	float Mult = 1.f;

	TArray<FGameplayEffectSpec> Specs;
	ActiveGameplayEffects.GetAllActiveGameplayEffectSpecs(Specs);

	for (const FGameplayEffectSpec& Spec : Specs)
	{
		Value += Spec.GetSetByCallerMagnitude(Tag);
	}
	// Only fetch mult value ONCE, outside the loop
	if (TagMult.IsValid())
	{
		float MultValue = 0.f;

		for (const FGameplayEffectSpec& Spec : Specs)
		{
			MultValue += Spec.GetSetByCallerMagnitude(TagMult);
		}

		if (MultValue > 0.f)
		{
			Mult += MultValue;
		}
	}
	return Value * Mult;
}

float USinASC::GetSetByCallerMagnitudeWithCap(FGameplayTag BaseTag, FGameplayTag MultTag, FGameplayTag CapBonusTag)
{
	const float RawValue = GetSetByCallerMagnitude(BaseTag, MultTag);

	const float DefaultCap = 0.75f;
	float BonusCap = 0.f;

	if (CapBonusTag.IsValid())
	{
		BonusCap = GetSetByCallerMagnitude(CapBonusTag);
	}

	const float FinalCap = DefaultCap + BonusCap;
	return FMath::Min(RawValue, FinalCap);
}


void USinASC::ManageAttributeBonuses(FGameplayTag GE_Tag, bool Apply, TMap<FGameplayTag, float> Mods)
{

	if (!Mods.IsEmpty())
	{
		FActiveGameplayEffectHandle LocalHandle = DBH[GE_Tag];
		if (!LocalHandle.IsValid())
		{
			return;
		}
		for (auto It = Mods.CreateConstIterator(); It; ++It)
		{
			FGameplayTag LocalTag = It.Key();
			if (AttributeBonusMap.Contains(LocalTag))
			{
				FGameplayAttribute* LocalAttribute = AttributeBonusMap.Find(LocalTag);
				float CurrentValue = GetAttributeAdditiveBonusFromEffect(LocalHandle, *LocalAttribute);
				float Bonus = It.Value();
				float FinalValue = Apply ? CurrentValue + Bonus : CurrentValue - Bonus;
				UpdateActiveGameplayEffectSetByCallerMagnitude(LocalHandle, LocalTag, FinalValue);
			}
			else
			{
				FGameplayTag TagMult = ResistMapMults.Contains(LocalTag) ? ResistMapMults[LocalTag] : FGameplayTag();
				FGameplayTag TagCap = ResistMapCaps.Contains(LocalTag) ? ResistMapCaps[LocalTag] : FGameplayTag();

				float RawBaseValue = 0.f;
				TArray<FGameplayEffectSpec> Specs;
				ActiveGameplayEffects.GetAllActiveGameplayEffectSpecs(Specs);

				for (const FGameplayEffectSpec& Spec : Specs)
				{
					RawBaseValue += Spec.GetSetByCallerMagnitude(LocalTag);
				}

				float Bonus = It.Value();
				float FinalValue = Apply ? RawBaseValue + Bonus : RawBaseValue - Bonus;

				UpdateActiveGameplayEffectSetByCallerMagnitude(LocalHandle, LocalTag, FinalValue);
			}
		}
	}
}

void USinASC::SustainedHealthDamage(float TimeLeft)
{
	float ClampedTime = FMath::Clamp(6.0f, 0.0f, TimeLeft);
	if (GetActiveEffectsWithAllTags(FGameplayTagContainer{ TAG_Health_Regen }).IsValidIndex(0))
	{
		FActiveGameplayEffectHandle Handle = (GetActiveEffectsWithAllTags(FGameplayTagContainer{ TAG_Health_Regen })[0]);
		UpdateActiveGameplayEffectSetByCallerMagnitude(Handle, TAG_Health_Damage, ClampedTime);
	}
}

void USinASC::SustainedStaminaDamage(float TimeLeft)
{
	float ClampedTime = FMath::Clamp(3.0f, 0.0f, TimeLeft);
	if (GetActiveEffectsWithAllTags(FGameplayTagContainer{ TAG_Stamina_Regen }).IsValidIndex(0))
	{
		FActiveGameplayEffectHandle Handle = (GetActiveEffectsWithAllTags(FGameplayTagContainer{ TAG_Stamina_Regen })[0]);
		UpdateActiveGameplayEffectSetByCallerMagnitude(Handle, TAG_Stamina_Damage, ClampedTime);
	}
}

void USinASC::SustainedManaDamage(float TimeLeft)
{
	float ClampedTime = FMath::Clamp(4.0f, 0.0f, TimeLeft);
	if (GetActiveEffectsWithAllTags(FGameplayTagContainer{ TAG_Mana_Regen }).IsValidIndex(0))
	{
		FActiveGameplayEffectHandle Handle = (GetActiveEffectsWithAllTags(FGameplayTagContainer{ TAG_Mana_Regen })[0]);
		UpdateActiveGameplayEffectSetByCallerMagnitude(Handle, TAG_Mana_Damage, ClampedTime);
	}
}

void USinASC::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (HasMatchingGameplayTag(TAG_Input_Ability_Block))
	{
		ClearAbilityInput();
		return;
	}
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
			}
		}
	}
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}
	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void USinASC::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

FActiveGameplayEffectHandle USinASC::GetFirstGameEffectHandleBasedOnTag(FGameplayTag GE_Tag, bool& Found)
{
	{
		FGameplayTagContainer InTags{ GE_Tag };
		TArray<FActiveGameplayEffectHandle> AllEffects = GetActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InTags));
		if (!AllEffects.IsEmpty())
		{
			Found = true;
			return AllEffects[0];
		}
		Found = false;
		return FActiveGameplayEffectHandle();
	}
}

FActiveGameplayEffectHandle USinASC::GetHandleProgression()
{
	bool Found;
	return GetFirstGameEffectHandleBasedOnTag(TAG_Attribute_Progression, Found);
}

FActiveGameplayEffectHandle USinASC::GetHandleGear()
{
	bool Found;
	return GetFirstGameEffectHandleBasedOnTag(TAG_Gear, Found);
}

FActiveGameplayEffectHandle USinASC::GetHandlePerk()
{
	bool Found;
	return GetFirstGameEffectHandleBasedOnTag(TAG_Perk, Found);
}

FActiveGameplayEffectHandle USinASC::GetHandleWeapon(bool OffHand)
{
	bool Found;
	return  OffHand ? 
	GetFirstGameEffectHandleBasedOnTag(TAG_Item_Equipment_Weapon_OffHand, Found) :
	GetFirstGameEffectHandleBasedOnTag(TAG_Item_Equipment_Weapon_MainHand, Found);
}

FGameplayEffectSpec USinASC::GetGameEffectSpecBasedOnTag(FGameplayTag GE_Tag)
{
	TArray<FGameplayEffectSpec> LocArray;
	ActiveGameplayEffects.GetAllActiveGameplayEffectSpecs(LocArray);
	for (FGameplayEffectSpec LocSpec : LocArray)
{
	FGameplayTagContainer LocalContainer;
	LocSpec.GetAllAssetTags(LocalContainer);
	if (LocalContainer.HasTag(GE_Tag))
	{
		return LocSpec;
	}
}
	return FGameplayEffectSpec();
}

TArray<FActiveGameplayEffectHandle> USinASC::GetAllJobHandles()
{
	TArray<FActiveGameplayEffectHandle> LocalHandles;
	for (auto It = LearnedClassAbilities.CreateIterator(); It; ++It)
	{
		LocalHandles.Add(It->LearnedClass);
	}
	return LocalHandles;
}

FSinLearnedClassWrapper& USinASC::GetCachedClass(bool& Found, int32& Index, FActiveGameplayEffectHandle& ClassHandle)
{
	Found = false;  // Ensure we start with Found as false

	// Iterate through the array to find a matching ClassHandle
	for (int32 i = 0; i < LearnedClassAbilities.Num(); ++i)
	{
		if (LearnedClassAbilities[i].LearnedClass == ClassHandle)
		{
			Index = i;
			Found = true;
			return LearnedClassAbilities[i]; // Return the existing entry
		}
	}

	// If not found, create a new entry
	FSinLearnedClassWrapper NewEntry;
	NewEntry.LearnedClass = ClassHandle;

	// Add the new entry and get the correct reference
	Index = LearnedClassAbilities.Add(NewEntry);
	Found = true;
	return LearnedClassAbilities[Index];  // Return the new entry correctly
}

TArray<FGameplayAbilitySpecHandle> USinASC::GetClassLearnedAbilities(FActiveGameplayEffectHandle ClassHandle)
{
	TArray<FGameplayAbilitySpecHandle> ClassAbilities;
	for (auto It = LearnedClassAbilities.CreateIterator(); It; ++It)
	{
		if (It->LearnedClass == ClassHandle)
		{
			ClassAbilities = LearnedClassAbilities[It.GetIndex()].Abilities;
			return ClassAbilities;
		}
	}
	return ClassAbilities;
}

void USinASC::GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> Abilities, const FActiveGameplayEffectHandle& ClassHandle)
{
	if (Abilities.IsEmpty()) { return; }
	for (TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		FGameplayAbilitySpecHandle NewSpec = K2_GiveAbility(AbilityClass, 1, -1);
		if (NewSpec.IsValid()) 
		{
			CacheClassAbility(ClassHandle, NewSpec);
		}
	}
}

FAsyncCoroutine USinASC::GrantBonusPerks(TArray<FItemGrantedPerk> Perks, bool bGrant)
{
	for (const FItemGrantedPerk& Perk : Perks)
	{
		UDataTable* LoadedTable = nullptr; EAssetLoadedOp LoadStatus; FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this; LatentInfo.UUID = __LINE__; LatentInfo.Linkage = 0; LatentInfo.ExecutionFunction = NAME_None;
		co_await USinCommonLibrary::SinLoadDataTable(LatentInfo, Perk.PerkDataTable, LoadStatus, LoadedTable);

		if (LoadStatus == EAssetLoadedOp::Completed && LoadedTable)
		{
			FSinPerkRow* FoundRow = LoadedTable->FindRow<FSinPerkRow>(Perk.PerkID, TEXT("GrantBonusPerks Lookup"));
			if (FoundRow)
			{
				if (bGrant)
				{
					PerkAdd(Perk.ClassTag, *FoundRow, Perk.BonusRanks, /*bBonus=*/true);
				}
				else
				{
					PerkRemove(Perk.ClassTag, *FoundRow, Perk.BonusRanks, /*bBonus=*/true);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Perk row not found: %s"), *Perk.PerkID.ToString());
			}
		}
	}
}

void USinASC::GrantPassiveAbilities(const TMap<TSubclassOf<USinGameplayAbility>, int32>& PassiveAbilities)
{
	for (const auto& Pair : PassiveAbilities)
	{
		TSubclassOf<USinGameplayAbility> AbilityClass = Pair.Key; int32 AbilityLevel = Pair.Value; if (AbilityClass)
		{
			GrantPassiveAbility(AbilityClass, AbilityLevel);
		}
	}
}

void USinASC::GrantPassiveAbility(TSubclassOf<USinGameplayAbility> AbilityClass, int32 Level)
{
	if (!FindAbilitySpecFromClass(AbilityClass))
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, Level);

		// Set max level from CDO
		if (USinPassiveAbility* AbilityCDO = Cast<USinPassiveAbility>(AbilityClass->GetDefaultObject()))
		{
			int32 MaxLev = AbilityCDO->MaxLevel;
			if (MaxLev > 0)
			{
				AbilitySpec.Level = FMath::Min(Level, MaxLev);
			}
		}

		FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

		// Wait until we have a live instance
		if (FGameplayAbilitySpec* NewSpec = FindAbilitySpecFromHandle(Handle))
		{
			if (USinPassiveAbility* AbilityInstance = Cast<USinPassiveAbility>(NewSpec->GetPrimaryInstance()))
			{
				//AbilityInstance->StoredLevel = Level; AbilityInstance->UpdateAbilityLevel(Level);
				OnAbilityLevelChanged.Broadcast(NewSpec->Ability, NewSpec->Level, NewSpec->Level);
				UE_LOG(LogTemp, Log, TEXT("Granted Passive Ability: %s at Level %d"), *AbilityClass->GetName(), Level);
			}
		}
	}
	else
	{
		// Already granted - increment
		if (FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromClass(AbilityClass))
		{
			if (USinPassiveAbility* AbilityInstance = Cast<USinPassiveAbility>(ExistingSpec->GetPrimaryInstance()))
			{
				int32 PreviousLevel = ExistingSpec->Level; int32 NewLevel = PreviousLevel + Level;
				AbilityInstance->StoredLevel += Level; AbilityInstance->UpdateAbilityLevel(NewLevel);
				OnAbilityLevelChanged.Broadcast(ExistingSpec->Ability, NewLevel, PreviousLevel);
				UE_LOG(LogTemp, Log, TEXT("Updated Passive Ability Level: %s to %d"), *AbilityClass->GetName(), NewLevel);
			}
		}
	}
}

// USinPassiveAbility
void USinASC::RemovePassiveAbilities(const TMap<TSubclassOf<USinGameplayAbility>, int32>& PassiveAbilities)
{
	for (const auto& Pair : PassiveAbilities)
	{
		TSubclassOf<USinGameplayAbility> AbilityClass = Pair.Key;
		int32 LevelValue = Pair.Value;

		if (AbilityClass)
		{
			FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromClass(AbilityClass);
			if (ExistingSpec)
			{
				int32 NewLevel = FMath::Max(0, ExistingSpec->Level - LevelValue);

				if (NewLevel <= 0)
				{
					ClearAbility(ExistingSpec->Handle);
					UE_LOG(LogTemp, Log, TEXT("Removed Passive Ability: %s"), *AbilityClass->GetName());
				}
				else
				{
					USinPassiveAbility* AbilityInstance = Cast<USinPassiveAbility>(ExistingSpec->GetPrimaryInstance());
					if (AbilityInstance)
					{
						int32 PreviousLevel = ExistingSpec->Level;
						AbilityInstance->UpdateAbilityLevel(NewLevel);
						OnAbilityLevelChanged.Broadcast(ExistingSpec->Ability, NewLevel, PreviousLevel);
					}
				}
			}
		}
	}
}

void USinASC::CacheClassAbility(FActiveGameplayEffectHandle ClassHandle, FGameplayAbilitySpecHandle NewAbility)
{
	bool Found;
	int32 ClassIndex;
	FSinLearnedClassWrapper& CachedClass = GetCachedClass(Found, ClassIndex, ClassHandle);
	if (Found)
	{
		CachedClass.Abilities.AddUnique(NewAbility);
	}
}

bool USinASC::RemoveCachedClass(FActiveGameplayEffectHandle ClassHandle)
{
	bool Found;
	int32 ClassIndex;
	GetCachedClass(Found, ClassIndex, ClassHandle);
	if (!Found)
	{
		return false;
	}
	LearnedClassAbilities.RemoveAt(ClassIndex);
	return true;
}

bool USinASC::IsGameplayEffectHandleValid(FActiveGameplayEffectHandle Handle) const
{
	return Handle.IsValid() && GetActiveGameplayEffect(Handle) != nullptr;
}

bool USinASC::ShouldApplyItemFragment(const FSinInventoryEntry& Entry, ESinItemEffectApplicationPolicy Policy) const
{
	if (!Gear){return false;}
	switch (Policy)
	{
	case ESinItemEffectApplicationPolicy::Never:
		return false;

	case ESinItemEffectApplicationPolicy::Always:
		return true;

	case ESinItemEffectApplicationPolicy::WhileInInventory:
		return true;

	case ESinItemEffectApplicationPolicy::WhileEquipped:
		{
			const FSinInventoryContainerState* Container =
				Gear->FindContainerStateById(Entry.ContainerId);

			if (!Container)
			{
				return false;
			}

			return Container->ContainerTag.MatchesTag(
				TAG_Item_Equipment);
		}
	}

	return false;
}

void USinASC::RefreshItemGrantedEffects()
{
	if (!Gear){return;}
}

void USinASC::HandleInventoryEntryAdded(UInventory* Inventory, const FSinInventoryEntry& Entry)
{
	if (const USinItemFragment_Stats* BonusStats =
Entry.ItemDefinition->GetStatsFragment())
	{
		if (ShouldApplyItemFragment(Entry, BonusStats->ApplicationPolicy))
		{
			ManageAttributeBonuses(TAG_Gear, true, BonusStats->Additives);
			ManageAttributeBonuses(TAG_Gear_Mult, true, BonusStats->Multiplicatives);
		}
	}
	if (const USinItemFragment_GrantedEffects* Effects =
Entry.ItemDefinition->GetGrantedEffectsFragment())
	{
		if (ShouldApplyItemFragment(Entry, Effects->ApplicationPolicy))
		{
			SinApplyBuffsOfSource(TAG_Source_Gear, Effects->Passives); GrantPassiveAbilities(Effects->Procs);
		}
	}
	if (const USinItemFragment_GrantedPerks* Perks =
Entry.ItemDefinition->GetPerksFragment())
	{
		if (ShouldApplyItemFragment(Entry, Perks->ApplicationPolicy))
		{
			GrantBonusPerks(Perks->BonusPerks, true);
		}
	}
}

void USinASC::HandleInventoryEntryRemoved(UInventory* Inventory, const FSinInventoryEntry& RemovedEntry)
{
	RefreshItemGrantedEffects();
}

void USinASC::HandleInventoryEntryChanged(
	UInventory* Inventory,
	const FSinInventoryEntry& OldEntry,
	const FSinInventoryEntry& Entry)
{
	if (!Entry.ItemDefinition)
	{
		return;
	}

	if (const USinItemFragment_Stats* BonusStats =
		Entry.ItemDefinition->GetStatsFragment())
	{
		const bool bWasApplied =
			ShouldApplyItemFragment(OldEntry, BonusStats->ApplicationPolicy);

		const bool bShouldApply =
			ShouldApplyItemFragment(Entry, BonusStats->ApplicationPolicy);

		if (bWasApplied && !bShouldApply)
		{
			ManageAttributeBonuses(TAG_Gear, false, BonusStats->Additives);
			ManageAttributeBonuses(TAG_Gear_Mult, false, BonusStats->Multiplicatives);
		}
		else if (!bWasApplied && bShouldApply)
		{
			ManageAttributeBonuses(TAG_Gear, true, BonusStats->Additives);
			ManageAttributeBonuses(TAG_Gear_Mult, true, BonusStats->Multiplicatives);
		}
	}

	if (const USinItemFragment_GrantedEffects* Effects =
		Entry.ItemDefinition->GetGrantedEffectsFragment())
	{
		const bool bWasApplied =
			ShouldApplyItemFragment(OldEntry, Effects->ApplicationPolicy);

		const bool bShouldApply =
			ShouldApplyItemFragment(Entry, Effects->ApplicationPolicy);

		if (bWasApplied && !bShouldApply)
		{
			// remove gear-sourced effects here
			RemoveGrantedEffectsFragment(Effects);
		}
		else if (!bWasApplied && bShouldApply)
		{
			SinApplyBuffsOfSource(TAG_Source_Gear, Effects->Passives);
			GrantPassiveAbilities(Effects->Procs);
		}
	}

	if (const USinItemFragment_GrantedPerks* Perks =
		Entry.ItemDefinition->GetPerksFragment())
	{
		const bool bWasApplied =
			ShouldApplyItemFragment(OldEntry, Perks->ApplicationPolicy);

		const bool bShouldApply =
			ShouldApplyItemFragment(Entry, Perks->ApplicationPolicy);

		if (bWasApplied && !bShouldApply)
		{
			GrantBonusPerks(Perks->BonusPerks, false);
		}
		else if (!bWasApplied && bShouldApply)
		{
			GrantBonusPerks(Perks->BonusPerks, true);
		}
	}
}

void USinASC::HandleInventoryContainerChanged(UInventory* Inventory, FGuid ContainerId)
{
	RefreshItemGrantedEffects();
}

void USinASC::ItemAdded(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
	UGameItemEquipment* LocalEquipment = Cast<UGameItemEquipment>(Item);
	if (LocalEquipment)
	{
		ManageAttributeBonuses(TAG_Gear, true, LocalEquipment->EquipmentData.PassiveAdds);
		FGameplayTagContainer TagContainer;
		//TagContainer.AddTag(TAG_Gear);
		//TagContainer.AddTag(TAG_Mult);
		ManageAttributeBonuses(TAG_Gear_Mult, true, LocalEquipment->EquipmentData.PassiveMults);
		SinApplyBuffsOfSource(TAG_Source_Gear, Item->GetBuffs());
		GrantPassiveAbilities(Item->GetBuffProcs());
		GrantBonusPerks(Item->GetBonusPerks());
	}
}


void USinASC::ItemRemoved(UInventory* NewInventory, int32 Index, UGameItemBase* Item, int32 SrcIndex, UInventory* SrcInventory)
{
	UGameItemEquipment* LocalEquipment = Cast<UGameItemEquipment>(Item);
	if (LocalEquipment)
	{
		ManageAttributeBonuses(TAG_Gear, false, LocalEquipment->EquipmentData.PassiveAdds);
		FGameplayTagContainer TagContainer;
		ManageAttributeBonuses(TAG_Gear_Mult, false, LocalEquipment->EquipmentData.PassiveMults);
		for (const auto& Pair : Item->GetBuffs())
		{
			const TSubclassOf<USin_GAS_Buff>& BuffClass = Pair.Key;
			const int32 StackCount = Pair.Value;
			if (BuffClass)
			{
				FActiveGameplayEffectHandle Handle = BuffManager.GetEffectHandle(BuffClass);
				int32 StacksToRemove = BuffManager.ShouldRemoveBuff(BuffClass, TAG_Source_Gear, StackCount);
				if (StacksToRemove >=0)
				{
					RemoveActiveGameplayEffect(Handle, StacksToRemove);
				}
			}
		}
		RemovePassiveAbilities(Item->GetBuffProcs());
		GrantBonusPerks(Item->GetBonusPerks(), false);
		//PerkRemove(FGameplayTag ClassTag, FSinPerkRow Perk, int32 Ranks, bool bBonus)
	}
}

void USinASC::RemoveGrantedEffectsFragment(const USinItemFragment_GrantedEffects* Effects)
{
	if (!Effects){return;}

	for (const auto& Pair : Effects->Passives)
	{
		const TSubclassOf<USin_GAS_Buff>& BuffClass = Pair.Key;
		const int32 StackCount = Pair.Value;

		if (!BuffClass)
		{
			continue;
		}

		FActiveGameplayEffectHandle Handle =
			BuffManager.GetEffectHandle(BuffClass);

		const int32 StacksToRemove =
			BuffManager.ShouldRemoveBuff(
				BuffClass,
				TAG_Source_Gear,
				StackCount
			);

		if (StacksToRemove >= 0)
		{
			RemoveActiveGameplayEffect(Handle, StacksToRemove);
		}
	}

	RemovePassiveAbilities(Effects->Procs);
}

void USinASC::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			//if (AbilitySpec.Ability)
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				ProcessAbilityInput(0.f, false);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void USinASC::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())	
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
				FGameplayAbilitySpec AbilitySpecLo = AbilitySpec;
				AbilitySpecInputReleased(AbilitySpecLo);
			}
		}
	}
}

float USinASC::GetAttributeScalingCeiling(EPrimaryAttribute Attribute)
{
	// default base value is 0.5f. We can pump it up to 1.0 with perks, skills, class levels
	// In the future: check for perks, passives, class levels, etc.
	switch (Attribute)
	{
	case EPrimaryAttribute::Strength:
	case EPrimaryAttribute::Dexterity:
	case EPrimaryAttribute::Sorcery:
	case EPrimaryAttribute::Faith:
	case EPrimaryAttribute::Charisma:
		return 0.5f; // Default ceiling for now
	default:
		return 0.0f;
	}
}

void USinASC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USinASC, LearnedHandles);
	DOREPLIFETIME(USinASC, LearnedClassAbilities);
}
void USinASC::OnRep_LearnedClassAbilities(const TArray < FSinLearnedClassWrapper>& OldValue)
{

}
