// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SinBaseWeapon.h"

#include "SinCharacter.h"
#include "Misc/SinLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASinBaseWeapon::ASinBaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASinBaseWeapon::RawDamage(TMap<FGameplayTag, float>& WeaponDamage, TMap<FGameplayTag, float>& Modifiers)
{
	// Clear the output maps to ensure they start empty
	WeaponDamage.Empty();
	Modifiers.Empty();
	//
	WeaponDamage = USinLibrary::ConvertDamageMap(0, WeaponData.Damage);
	Modifiers = USinLibrary::ConvertAttributeMap(0, WeaponData.Scaling);
}

void ASinBaseWeapon::GetMoveset_Implementation(FName& MovesetID)
{
	MovesetID = WeaponData.MovesetID;
}

void ASinBaseWeapon::SetDamage_Implementation()
{
	USinASC* LocalASC = GetOwner()->GetComponentByClass<USinASC>();
	if (IsValid(LocalASC))
	{
		FActiveGameplayEffectHandle LocalHandle = LocalASC->GetHandleWeapon(OffHand);
		if (const FActiveGameplayEffect* ActiveEffect = LocalASC->GetActiveGameplayEffect(LocalHandle))
		{
			// Directly access the Spec from ActiveEffect and cast to mutable
			FGameplayEffectSpec* MutableSpec = const_cast<FGameplayEffectSpec*>(&ActiveEffect->Spec);
			if (MutableSpec)
			{
				MutableSpec->SetByCallerTagMagnitudes.Empty();
			}
		}
		// SET RAW WEAPON DAMAGE
		LocalASC->UpdateActiveGameplayEffectSetByCallerMagnitudes(LocalHandle, USinLibrary::ConvertDamageMap(0, WeaponData.Damage));
		// SET SCALING BASED ON ATTRIBUTE TAGS
		LocalASC->UpdateActiveGameplayEffectSetByCallerMagnitudes(LocalHandle, USinLibrary::ConvertAttributeMap(0, WeaponData.Scaling));
		// set raw blocking stats, scaling will be handled more universally rather than indvidually for each weapon like damage, no need to make it too complicated
		LocalASC->UpdateActiveGameplayEffectSetByCallerMagnitudes(LocalHandle, USinLibrary::ConvertDamageMap(1, WeaponData.BlockPower));
	}
}

void ASinBaseWeapon::CollisionFX_Implementation(AActor* Victim, FHitResult Hit)
{
}

float ASinBaseWeapon::GetKnockbackPower_Implementation()
{
	return 300.0f;
}

// Called when the game starts or when spawned
void ASinBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASinBaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASinBaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION(ASinBaseWeapon, WeaponData, COND_InitialOnly);
	DOREPLIFETIME(ASinBaseWeapon, OffHand);
}

