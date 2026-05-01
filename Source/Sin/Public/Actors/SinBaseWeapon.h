// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SinGlobalStructs.h"
#include "Interfaces/SinWeaponInterface.h"
#include "SinBaseWeapon.generated.h"

class ASinCharacter;

UCLASS()
class SIN_API ASinBaseWeapon : public AActor, public ISinWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASinBaseWeapon();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ExposeOnSpawn = "true"))
		FSinWeapon WeaponData;

	UFUNCTION(BlueprintPure, Category = "Combat")
		void RawDamage(TMap<FGameplayTag, float>& WeaponDamage, TMap<FGameplayTag, float>& Modifiers);

	/** Deprecation appreciation */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Combat")
		void GetMoveset(FName& MovesetID);
		virtual void GetMoveset_Implementation(FName& MovesetID);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void SetDamage();
		virtual void SetDamage_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FX")
		void CollisionFX(AActor* Victim, FHitResult Hit);
		virtual void CollisionFX_Implementation(AActor* Victim, FHitResult Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		float GetKnockbackPower();
		virtual float GetKnockbackPower_Implementation();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;  // Ensure override keyword

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Default", meta = (ExposeOnSpawn = "true"))
	bool OffHand;


};
