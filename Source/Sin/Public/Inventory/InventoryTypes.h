#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GAS/SinStatTypes.h"
#include "InventoryTypes.generated.h"

class UGameItemBase;
class USinGameplayAbility;
class USin_GAS_Buff;

#define LOCTEXT_NAMESPACE "InventoryTypes"

UENUM(BlueprintType)
enum class EPrimaryItemType : uint8
{
	Regular,
	Crafting,
	Equipment,
	Weapon,
	Consumable,
	Container,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EPrimaryItemType, EPrimaryItemType::Count);

UENUM(BlueprintType)
enum class EItemIntStat : uint8
{
	Stack,
	MaxStack,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemIntStat, EItemIntStat::Count);

UENUM(BlueprintType)
enum class EItemFloatStat : uint8
{
	Durability,
	MaxDurability,
	Weight,
	Value,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemFloatStat, EItemFloatStat::Count);

UENUM(BlueprintType)
enum class EItemSoundType : uint8
{
	PickUp,
	Receive,
	Drop,
	Equip,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemSoundType, EItemSoundType::Count);

UENUM(BlueprintType)
enum class EWeaponSize : uint8
{
	One_Hand UMETA(DisplayName = "One-Handed"),
	Two_Hand UMETA(DisplayName = "Two-Handed"),
	Main_Hand UMETA(DisplayName = "Main Hand"),
	Off_Hand UMETA(DisplayName = "Off-Hand"),
	Two_Hand_Only UMETA(DisplayName = "Two-Handed Only"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EWeaponSize, EWeaponSize::Count);

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Axe,
	Greataxe,
	Dagger,
	Shortsword,
	Longword,
	Scimitar,
	Rapier,
	Bastard_Sword UMETA(DisplayName = "Bastard Sword"),
	Greatsword,
	Colossal_Sword UMETA(DisplayName = "Colossal Sword"),
	Katana,
	Mace,
	Warhammer,
	Spear,
	Halberd,
	Scythe,
	Shortbow,
	Longbow,
	Crossbow,
	Shield,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EWeaponType, EWeaponType::Count);

USTRUCT(BlueprintType)
struct FItemContainer : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	int32 Index = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	UGameItemBase* Item = nullptr;

};

USTRUCT(BlueprintType)
struct FItemBase : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	TSubclassOf<UGameItemBase> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	FText Label = LOCTEXT("GameItem", "Game Item");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	TSoftObjectPtr <UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	FGameplayTagContainer ItemTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	FText ShortDescription = LOCTEXT("GameItem", "Game Item");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	FText LongDescription = LOCTEXT("GameItem", "Game Item");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	TSoftObjectPtr<USoundBase> ItemPickedUpSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	int32 MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameItem")
	float Price = 21.f;
};

USTRUCT(BlueprintType)
struct FItemGrantedPerk
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> PerkDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PerkID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BonusRanks = 1;
};

USTRUCT(BlueprintType)
struct FItemPersistence
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float Quality = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 Upgrades = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float Durability = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float MaxDurability = 100.0f;
};

USTRUCT(BlueprintType)
struct FEquipmentItem : public FItemBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Persistence"))
	FItemPersistence PersistentValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	float Durability = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	float MaxDurability = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	float ArmorRating = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TMap<FGameplayTag, float> PassiveAdds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TMap<FGameplayTag, float> PassiveMults;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TMap<TSubclassOf<USin_GAS_Buff>, int32> Buffs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TMap<TSubclassOf<USinGameplayAbility>, int32> BuffProcs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TArray< FItemGrantedPerk> BonusPerks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TSoftObjectPtr<UObject> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (DisplayAfter = "Price"))
	TSoftObjectPtr<UObject> MeshFemale;
};

USTRUCT(BlueprintType)
struct FSinWeapon : public FEquipmentItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	EWeaponType WeaponType = EWeaponType::Dagger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	EWeaponSize WeaponSize = EWeaponSize::One_Hand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	TMap<EPrimaryAttribute, float> Scaling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	TMap<EDamageType, float> Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	TMap<EDamageType, float> BlockPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	float BaseKnockbackPower = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	TSoftObjectPtr<UObject> Sheath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	FName MovesetID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	FName MovesetIDTwoHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	FName MH_Suffix_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	FName OH_Suffix_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	TSoftObjectPtr<USoundBase> SoundDrawWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (DisplayAfter = "MeshFemale"))
	TSoftObjectPtr<USoundBase> SoundSheatheWeapon;
};

UENUM(BlueprintType)
enum class EInventoryNavigationDirection : uint8
{
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down")
};

#undef LOCTEXT_NAMESPACE