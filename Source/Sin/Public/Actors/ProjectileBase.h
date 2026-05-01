// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SinWeaponInterface.h"
#include "Misc/DA_SinSpellBase.h"
#include "ProjectileBase.generated.h"

UCLASS()
class SIN_API AProjectileBase : public AActor, public ISinWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FX")
		void CollisionFX(AActor* Victim, FHitResult Hit);
		virtual void CollisionFX_Implementation(AActor* Victim, FHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void AbsoluteWeaponDamageCall(ASinCharacter* WeaponOwner, FCharStats& Stats);
		virtual void AbsoluteWeaponDamageCall_Implementation(ASinCharacter* WeaponOwner, FCharStats& Stats);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		float GetKnockbackPower();
		virtual float GetKnockbackPower_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		TObjectPtr<UDA_SinSpellBase> SpellData;
		//TSubclassOf<UDA_SinSpellBase> SpellData;

};
