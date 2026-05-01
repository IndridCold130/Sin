// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ProjectileBase.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProjectileBase::CollisionFX_Implementation(AActor* Victim, FHitResult Hit)
{
}

void AProjectileBase::AbsoluteWeaponDamageCall_Implementation(ASinCharacter* WeaponOwner, FCharStats& Stats)
{

}

float AProjectileBase::GetKnockbackPower_Implementation()
{
	return 250.0f;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

