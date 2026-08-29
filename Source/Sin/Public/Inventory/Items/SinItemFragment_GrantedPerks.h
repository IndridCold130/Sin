#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "GameplayTagContainer.h"
#include "SinGlobalStructs.h"
#include "SinItemFragment_GrantedPerks.generated.h"

USTRUCT(BlueprintType)
struct FSinGrantedPerkModifier
{
	GENERATED_BODY()

public:
	// Examples:
	// Perk.SwordMastery
	// Skill.Fireball
	// Mastery.Divine
	// Class.Cleric.AllSkills
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Perk")
	FGameplayTag PerkTag;

	// Example:
	// +1 Fireball
	// +2 Sword Mastery
	// +1 Divine Mastery
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Perk")
	int32 RankBonus = 1;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_GrantedPerks : public USinItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Application")
	ESinItemEffectApplicationPolicy ApplicationPolicy = ESinItemEffectApplicationPolicy::WhileEquipped;
	// Static perk/skill/mastery rank bonuses while equipped.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Perks")
	TArray<FSinGrantedPerkModifier> GrantedPerks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Perks")
	TArray<FItemGrantedPerk> BonusPerks;
};