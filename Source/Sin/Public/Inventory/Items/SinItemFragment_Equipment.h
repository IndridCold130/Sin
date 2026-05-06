#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "GameplayTagContainer.h"
#include "SinItemFragment_Equipment.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_Equipment : public USinItemFragment
{
	GENERATED_BODY()

public:

	// Example tags:
	// Equipment.Slot.Head
	// Equipment.Slot.Chest
	// Equipment.Slot.MainHand
	// Equipment.Slot.OffHand
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	FGameplayTag EquipSlot;

	// Optional broader type:
	// Equipment.Type.Armor
	// Equipment.Type.Weapon
	// Equipment.Type.Ring
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	FGameplayTag EquipmentType;

	// Optional: requirements such as Class.Fighter, Armor.Heavy, Weapon.Sword, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	FGameplayTagContainer RequiredTags;

	// Optional: tags granted while equipped.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	FGameplayTagContainer GrantedTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment|Durability", meta=(ClampMin="0.0"))
	float MaxDurability = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment|Durability")
	bool bUsesDurability = true;

	// Mesh used for male/default body.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment|Visual")
	TSoftObjectPtr<UObject> Mesh;

	// Optional alternate mesh.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment|Visual")
	TSoftObjectPtr<UObject> MeshFemale;
};