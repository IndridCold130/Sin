#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SinStatTypes.generated.h"

UENUM(BlueprintType)
enum class EPrimaryAttribute : uint8
{
	Strength      UMETA(DisplayName = "Strength"),
	Dexterity     UMETA(DisplayName = "Dexterity"),
	Constitution  UMETA(DisplayName = "Constitution"),
	Sorcery        UMETA(DisplayName = "Sorcery"),
	Faith          UMETA(DisplayName = "Faith"),
	Charisma      UMETA(DisplayName = "Charisma"),

	Count         UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EPrimaryAttribute, EPrimaryAttribute::Count);

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Slash UMETA(DisplayName = "Slash"),
	Strike UMETA(DisplayName = "Strike"),
	Pierce UMETA(DisplayName = "Pierce"),

	Fire UMETA(DisplayName = "Fire"),
	Cold UMETA(DisplayName = "Cold"),
	Lightning UMETA(DisplayName = "Lightning"),

	Arcane UMETA(DisplayName = "Arcane"),
	Dark UMETA(DisplayName = "Dark"),
	Holy UMETA(DisplayName = "Holy"),

	Bleed UMETA(DisplayName = "Bleed"),
	Disease UMETA(DisplayName = "Disease"),
	Poison UMETA(DisplayName = "Poison"),

	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EDamageType, EDamageType::Count);

USTRUCT(BlueprintType)
struct FCharStat
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	FGameplayTag Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	float Value = 0.f;
};

USTRUCT(BlueprintType)
struct FCharStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	TArray<FCharStat> Attributes;
};

USTRUCT(BlueprintType)
struct FStatScalingInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EPrimaryAttribute Attribute = EPrimaryAttribute::Strength;

	UPROPERTY(BlueprintReadOnly)
	float ScalingValue = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ScalingTag;

	FStatScalingInfo()
		: Attribute(EPrimaryAttribute::Strength)
		, ScalingValue(0.f)
		, ScalingTag()
	{
	}

	FStatScalingInfo(EPrimaryAttribute InAttr, float InVal, FGameplayTag InTag)
		: Attribute(InAttr)
		, ScalingValue(InVal)
		, ScalingTag(InTag)
	{
	}
};