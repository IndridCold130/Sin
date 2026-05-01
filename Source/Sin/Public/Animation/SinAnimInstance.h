// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SinCharacter.h"
#include "SinAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Transient, Blueprintable, hideCategories=AnimInstance, BlueprintType)
class SIN_API USinAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	USinAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner");
	TObjectPtr<ASinCharacter> SinOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion");
		float GroundSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion");
		float Direction = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion");
		bool bStill = false;


protected:

		virtual void NativeInitializeAnimation() override;
		//virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
};
