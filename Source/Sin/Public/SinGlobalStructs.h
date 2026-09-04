// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
//#include "Templates/SubclassOf.h"
//#include "Inventory/GameItemBase.h"
#include "AbilitySystemComponent.h"
#include "GAS/SinStatTypes.h"
#include "Inventory/InventoryTypes.h"
#include "SinGlobalStructs.generated.h"

class UDataAsset;
class UGameItemBase;
class USinBuffMasterComponent;
class ASinCharacter;
class USinGameplayEffect;
class USinGameplayAbility;
class USinPassiveAbility;
class USin_GAS_Buff;


#define LOCTEXT_NAMESPACE "MyNamespace"

UENUM(BlueprintType)
enum class EPerkType : uint8
{
	Passive      UMETA(DisplayName = "Passive"),
	Proc     UMETA(DisplayName = "Proc"),
	UniqueEffect  UMETA(DisplayName = "UniqueEffect"),
	Active      UMETA(DisplayName = "Active"),

	Count         UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EPerkType, EPerkType::Count);

UENUM(BlueprintType)
enum class EAmbientSoundType : uint8
{
	Ambience,
	BackgroundMusic,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAmbientSoundType, EAmbientSoundType::Count);

USTRUCT(BlueprintType)
struct FSinSoundAmbience : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinAmbience")
	TArray<TSoftObjectPtr<USoundBase>> Sounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinAmbience")
	EAmbientSoundType AudioType;

};

UENUM(BlueprintType)
enum class EActionType : uint8
{
	LightAttack,
	HeavyAttack,
	SpecialAction,
	Block,
	Parry,
	Execution,
	Spellcast,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EActionType, EActionType::Count);

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	RightHand,
	LeftHand,
	RightFoot,
	LeftFoot,
	Bite,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttackType, EAttackType::Count);

USTRUCT(BlueprintType)
struct FComboStepNext
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TMap< EActionType, int32> NextStep;
};

USTRUCT(BlueprintType)
struct FAttackDataItem
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* ComboMontage;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float ComboDamageModifier = 1.f;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TMap<FGameplayTag, float> SpecialComboModifiers;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		FComboStepNext NextCombo;
};

USTRUCT(BlueprintType)
struct FAttackDataItemArray
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TArray<FAttackDataItem> ComboItem;
};

USTRUCT(BlueprintType)
struct FAttackData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* DrawWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UAnimInstance> CombatAnimsLayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		FName SheathSocket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		FName SheathSocketB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* ChargeAttackLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* ChargeAttackHeavy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* Block;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* BlockResponse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* Parry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TMap<EActionType, FAttackDataItemArray> Items;
};

USTRUCT(BlueprintType)
struct FMorphSlider : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		bool DualMorph;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		FName NegativeID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		double Min = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		double Max = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		TMap<FName, float> CorrectiveMods;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		FGameplayTagContainer Whitelist;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slider")
		FGameplayTagContainer Blacklist;
};

USTRUCT(BlueprintType)
struct FMorphSliders : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customization")
		TArray<FMorphSlider> Sliders;
};

USTRUCT(BlueprintType)
struct FSinNamedFloat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyScalars")
		FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyScalars")
		FName AdditionalIdentifier = "Body";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyScalars")
		float Value;
};

USTRUCT(BlueprintType)
struct FSinNamedColor : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyScalars")
		FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyScalars")
		FName AdditionalIdentifier = "Body";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyScalars")
		FLinearColor Color;
};

USTRUCT(BlueprintType)
struct FSinNamedSoftAsset: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Named Asset")
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Named Asset")
	TSoftObjectPtr<UDataAsset> Asset;
};

USTRUCT(BlueprintType)
struct FSinCharAppearance : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FSinNamedFloat> Morphs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FSinNamedColor> Colors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FSinNamedSoftAsset> Assets;
};

USTRUCT(BlueprintType)
struct FSinCharAppearancePreset : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FName PresetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FGameplayTag Gender;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FSinCharAppearance Appearance;
};

USTRUCT(BlueprintType)
struct FSinCharAppearancePresets : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FSinCharAppearancePreset> Presets;

};

USTRUCT(BlueprintType)

struct FSinGameplayAbilityClassWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinGameplayAbilities")
	TArray<TSubclassOf<USinGameplayAbility>> Abilities;
};

USTRUCT(BlueprintType)
struct FSinLearnedClassWrapper
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG_Class")
	FActiveGameplayEffectHandle LearnedClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG_Class")
	TArray<FGameplayAbilitySpecHandle> Abilities;
	// Define equality operator
	bool operator==(const FSinLearnedClassWrapper& Other) const
	{
		return LearnedClass == Other.LearnedClass;
	}
};

//USTRUCT(BlueprintType)
//struct FGameplayTagToText : public FTableRowBase
//{
	//FGameplayTag Tag;
	//FText Text;
//};

USTRUCT(BlueprintType)
struct FSinRace : public FTableRowBase
{
	GENERATED_BODY()

	// This goes in the identity gameplaytag container.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	FGameplayTag RaceTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	TSoftObjectPtr <UTexture2D> RaceIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	FText RaceLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	TArray<FText> RacialDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	TArray<FName> StartingEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	TArray<FSinNamedFloat> RacialMorphConstants;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	TArray<FCharStat> RacialPassives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterCreation")
	TArray <TSubclassOf<USinGameplayEffect>> PassiveEffects;

};

UENUM(BlueprintType)
enum class EInputMode : uint8
{
	VE_GameAndUI UMETA(DisplayName = "Game and UI"),
	VE_GameOnly UMETA(DisplayName = "Game Only"),
	VE_UIOnly UMETA(DisplayName = "UI Only"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EInputMode, EInputMode::Count);

UENUM(BlueprintType)
enum class EAttributeSource : uint8
{
	Full UMETA(DisplayName = "Full"),
	Gear UMETA(DisplayName = "Gear"),
	Perk UMETA(DisplayName = "Perk"),
	Buff UMETA(DisplayName = "Buff"),
	Curse UMETA(DisplayName = "Curse"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttributeSource, EAttributeSource::Count);

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	Missile UMETA(DisplayName = "Missile"),
	Targeted UMETA(DisplayName = "Targeted"),
	Buff UMETA(DisplayName = "Buff"),
	SelfBuff UMETA(DisplayName = "Self-Buff"),
	AoE UMETA(DisplayName = "Area of Effect"),
	Nova UMETA(DisplayName = "Nova"),
	Chanelled_AoE UMETA(DisplayName = "Chanelled AoE"),
	Chanelled_Target UMETA(DisplayName = "Chanelled Target"),
	Chanelled_Beam UMETA(DisplayName = "Chanelled Beam"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ESpellType, ESpellType::Count);

UENUM(BlueprintType)
enum class EAIPrimaryState : uint8
{
	Passive,
	Alert,
	Aggressive,
	Search,
	Leashed,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAIPrimaryState, EAIPrimaryState::Count);


USTRUCT(BlueprintType)
struct FSpawnDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
		FText Name = FText::FromString(TEXT("Generic Female"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
		TSoftClassPtr <ASinCharacter> CharacterClass;
};

UENUM()
enum class EAssetLoadedOp : uint8
{
	Completed,
	Failed
};

UENUM(BlueprintType)
enum class EMovementModifier: uint8
{
	Sprint,
	SpellCast,
	WaterVolume,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EMovementModifier, EMovementModifier::Count);

UENUM(BlueprintType)
enum class EInteractionMenu : uint8
{
	Loot UMETA(DisplayName = "Loot"),
	Merchant UMETA(DisplayName = "Merchant"),
	Mirror UMETA(DisplayName = "Mirror"),
	Altar UMETA(DisplayName = "Altar"),
	Bonfire UMETA(DisplayName = "Bonfire"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EInteractionMenu, EInteractionMenu::Count);

UENUM(BlueprintType)
enum class EStatRetrievalMode : uint8
{
	Attribute UMETA(DisplayName = "Attribute"),
	Tag UMETA(DisplayName = "Tag"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EStatRetrievalMode, EStatRetrievalMode::Count);

USTRUCT(BlueprintType)
struct FSinAttributeUIRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute AttributeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DataTag; // For SetByCaller-based stats like resistances

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DataTagMax; // For SetByCaller-based stats like resistances

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatRetrievalMode RetrievalMode = EStatRetrievalMode::Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisplayAsPerCent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CosmeticAddition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Tooltip;
};

// PERK SYSTEM RELATED STRUCTS

USTRUCT(BlueprintType)
struct FSinPerkRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredClassLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag RequiredPerk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredRank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText RankDescription;
};

USTRUCT(BlueprintType)
struct FSinPerkPassive
{
	GENERATED_BODY()

	// each rank might have its own requirements
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSinPerkRequirement Requirements;
	//TArray<FSinPerkRequirement> Requirements;

	// 'plug and play' for the setbycaller  function
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> PassiveEffects;
};

USTRUCT(BlueprintType)
struct FSinPerkProcRank
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSinPerkRequirement> Requirements;

	// Optional: Rank-specific trigger chance or magnitude
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 1.f;
};

USTRUCT(BlueprintType)
struct FSinPerkProc
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSinPerkRequirement> Requirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USinGameplayAbility> ProcAbility;
};

USTRUCT(BlueprintType)
struct FSinPerkRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PerkName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PerkTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPerkType PerkType;

	// in case perk type is passive, we simply apply the bonuses through SetByCaller
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSinPerkPassive> PassivePerks;

	// procs need to hook onto delegates to execute specific temporary effects so we will need an ability. Proc magnitude based on rank handled inside the proc ability. also handles active abilities for now
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSinPerkProc Proc;

	// unique effect in case passives do not quite cut it, for example a unique curve table progression?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> UniqueEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRankBase = 1;
};

USTRUCT(BlueprintType)
struct FSinPerk
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Perk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRankBase = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BonusRank = 0; // Added by gear, buffs, etc.

	int32 GetBaseRank() const
	{
		return Rank;
	}

	int32 GetEffectiveRank() const
	{
		return Rank + BonusRank;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxTotalRank = 30;

	int32 GetClampedEffectiveRank() const
	{
		return FMath::Min(Rank + BonusRank, MaxTotalRank);
	}
};

USTRUCT(BlueprintType)
struct FSinClassPerks // we should have an array of this stored in our ASC that way each class can be a separate member of the array with its own cached perks
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0; // Total levels invested in this class

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSinPerk> KnownPerks;
};

#undef LOCTEXT_NAMESPACE
