// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SinCMC.h"

#include "GameFramework/Character.h"
#include "GAS/SinAttributeSecondary.h"

USinCMC::USinCMC()
{
}

void USinCMC::SetMovementModeFlag(bool Add, EMovementModifier Flag)
{
	uint8 LocFlag = static_cast<uint8>(Flag);
	switch (Flag)
	{
	case EMovementModifier::Sprint:
	{
		FLAG_Sprint = Add;
	}
		break;
	case EMovementModifier::SpellCast:
	{
		FLAG_Cast = Add;
	}
		break;
	default:
		break;
	}
}

void USinCMC::BeginPlay()
{
	GameplayAbilityComponent = GetOwner()->FindComponentByClass<USinASC>();
	Super::BeginPlay();
}

void USinCMC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	//MaxWalkSpeed = BaseWalkSpeed * SpeedMultiplier;
}

float USinCMC::MovementSpeedDirectionMod() const
{
	FVector ForwardVector = GetOwner()->GetActorForwardVector();
	FVector MovementDirection = Velocity.GetSafeNormal();
	// Calculate the alignment (dot product)
	float Alignment = FVector::DotProduct(ForwardVector, MovementDirection);
	// Determine movement speed multiplier based on alignment
	float SpeedMultiplier = 1.0f; // Default speed for forward movement
	if (Alignment < -0.1f) // Moving backwards
	{
		return 0.6f;
		//SpeedMultiplier = 0.7f; // 50% slower when moving backwards
	}
	else if (Alignment < 0.5f) // Slightly misaligned
	{
		SpeedMultiplier = FMath::Lerp(0.6f, 1.0f, Alignment * 2.0f); // Scale between 50% and 100%
	}
	return SpeedMultiplier;
}

void USinCMC::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	FLAG_Sprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	FLAG_Cast = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* USinCMC::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		if (USinCMC* TempComponent = const_cast<USinCMC*>(this))
		{
			TempComponent->ClientPredictionData = new FSinPredictionData_Client_Character(*this);
			//TempComponent->ClientPredictionData = new FSinPredictionData_Client_Character(*this);
		}
	}

	return ClientPredictionData;
}

float USinCMC::GetMaxSpeed() const
{
	float DirectionSpeed = MovementSpeedDirectionMod();
	float SpeedMod = 1.f;
	bool Found;
	GameplayAbilityComponent ? 
		SpeedMod = GameplayAbilityComponent->GetGameplayAttributeValue(USinAttributeSecondary::GetMovementSpeedAttribute(), Found):
		SpeedMod = 1.f;
	float Speed = Super::GetMaxSpeed();
	if (FLAG_Sprint)
	{
		Speed *= 1.8f * SpeedMod;
	}
	if (FLAG_Cast)
	{
		Speed *= 0.4f* SpeedMod;
	}
	return Speed* SpeedMod* DirectionSpeed;
}

void FSinMovementModes::Clear()
{
	Super::Clear();
	bSavedWantsToSprint = false;
	bSavedWantsToCast = false;
}

uint8 FSinMovementModes::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	//Super::GetCompressedFlags();
	if (bSavedWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}
	if (bSavedWantsToCast)
	{
		Result |= FLAG_Custom_1;
	}
	return Result;
}

void FSinMovementModes::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	if (C)
	{
		if (USinCMC* SinMovement = Cast<USinCMC>(C->GetMovementComponent()))
		{
			bSavedWantsToSprint = SinMovement->FLAG_Sprint;
			bSavedWantsToCast = SinMovement->FLAG_Cast;
		}
	}
	
}

void FSinMovementModes::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	if (C)
	{
		if (USinCMC* SinMovement = Cast<USinCMC>(C->GetMovementComponent()))
		{
			SinMovement->FLAG_Sprint = bSavedWantsToSprint;
			SinMovement->FLAG_Cast = bSavedWantsToSprint;
		}
	}
}

bool FSinMovementModes::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	if (bSavedWantsToSprint != ((FSinMovementModes*)&NewMove)->bSavedWantsToSprint)
	{
		return false;
	}
	if (bSavedWantsToCast != ((FSinMovementModes*)&NewMove)->bSavedWantsToSprint)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

FSavedMovePtr FSinPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FSinMovementModes());
}
