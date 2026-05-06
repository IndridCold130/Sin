#pragma once

#include "CoreMinimal.h"
#include "SinItemFragment.h"
#include "GameplayEffect.h"
#include "Sound/SoundBase.h"
#include "SinItemFragment_GrantedEffects.generated.h"

class UNiagaraSystem;
class USoundBase;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FSinItemGrantedEffect
{
	GENERATED_BODY()

public:

	// Optional label for editor clarity.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	FText DisplayName;

	// Effect applied while equipped, if you use GAS.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> GameplayEffect;

	// Optional trigger tag:
	// Event.OnHit, Event.OnKill, Event.OnBlock, Event.OnEquip, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	FGameplayTag TriggerTag;

	// Chance from 0.0 to 1.0. Example: 0.15 = 15%.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ProcChance = 1.0f;

	// Optional visual effect for this proc/aura.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Visual")
	TSoftObjectPtr<UNiagaraSystem> VisualEffect;

	// Optional sound effect.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Audio")
	TSoftObjectPtr<USoundBase> SoundEffect;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SIN_API USinItemFragment_GrantedEffects : public USinItemFragment
{
	GENERATED_BODY()

public:

	// Passive effects applied while equipped.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TArray<FSinItemGrantedEffect> PassiveEffects;

	// Conditional effects triggered by gameplay events.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TArray<FSinItemGrantedEffect> ProcEffects;
};