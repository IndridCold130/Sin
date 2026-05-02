// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SinGlobalStructs.h"
#include "GAS/SinASC.h"

#include "SinCMC.generated.h"

/**
 * 
 */
UCLASS()
class SIN_API USinCMC : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	USinCMC();
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;
	UPROPERTY()
	TObjectPtr<USinASC> GameplayAbilityComponent;
	uint8 FLAG_Sprint : 1 = 0;
	uint8 FLAG_Cast : 1 = 0;

	UFUNCTION(BlueprintCallable, Category = "SinMovement")
	void SetMovementModeFlag(bool Add, EMovementModifier Flag);
	UFUNCTION()
	float MovementSpeedDirectionMod() const;
	//void SetMovementModeFlag(bool Sprint);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};

class FSinMovementModes : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;

	uint8 bSavedWantsToSprint : 1 = 0;
	uint8 bSavedWantsToCast : 1 = 0;

};
class FSinPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;
	FSinPredictionData_Client_Character(const UCharacterMovementComponent& CharacterMovementComponent) : Super(CharacterMovementComponent){}
	virtual FSavedMovePtr AllocateNewMove() override;

};