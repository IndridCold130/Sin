// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SinCMC.h"

#include "GameFramework/Character.h"
#include "GAS/SinAttributeSecondary.h"

USinCMC::USinCMC()
{
}

void USinCMC::SetMovementModeFlag(bool Add, EMovementModifier Flag)
{
	switch (Flag)
	{
	case EMovementModifier::Sprint:
		FLAG_Sprint = Add;
		break;

	case EMovementModifier::SpellCast:
		FLAG_Cast = Add;
		break;

	default:
		break;
	}
}

void USinCMC::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		GameplayAbilityComponent = OwnerActor->FindComponentByClass<USinASC>();
	}
}

void USinCMC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	//MaxWalkSpeed = BaseWalkSpeed * SpeedMultiplier;
}

float USinCMC::MovementSpeedDirectionMod() const
{
	const FVector ForwardVector = GetOwner() ? GetOwner()->GetActorForwardVector() : FVector::ForwardVector;
	const FVector MovementDirection = !Acceleration.IsNearlyZero()
		? Acceleration.GetSafeNormal()
		: Velocity.GetSafeNormal();

	const float Alignment = FVector::DotProduct(ForwardVector, MovementDirection);

	if (Alignment < -0.1f)
	{
		return 0.6f;
	}

	if (Alignment < 0.5f)
	{
		return FMath::Lerp(0.6f, 1.0f, FMath::Clamp(Alignment * 2.0f, 0.0f, 1.0f));
	}

	return 1.0f;
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
	float SpeedMod = 1.f;
	bool bFound = false;

	if (GameplayAbilityComponent)
	{
		SpeedMod = GameplayAbilityComponent->GetGameplayAttributeValue(
			USinAttributeSecondary::GetMovementSpeedAttribute(),
			bFound
		);

		if (!bFound)
		{
			SpeedMod = 1.f;
		}
	}
	float StateMod = 1.f;

	if (FLAG_Sprint)
	{
		StateMod *= 1.8f;
	}

	if (FLAG_Cast)
	{
		StateMod *= 0.4f;
	}
	return Super::GetMaxSpeed() * SpeedMod * StateMod * MovementSpeedDirectionMod();
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
			SinMovement->FLAG_Cast = bSavedWantsToCast;
		}
	}
}

bool FSinMovementModes::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSinMovementModes* NewSinMove = static_cast<const FSinMovementModes*>(NewMove.Get());

	if (bSavedWantsToSprint != NewSinMove->bSavedWantsToSprint)
	{
		return false;
	}

	if (bSavedWantsToCast != NewSinMove->bSavedWantsToCast)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

FSavedMovePtr FSinPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FSinMovementModes());
}
