// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SinAnimInstance.h"

#include "KismetAnimationLibrary.h"

USinAnimInstance::USinAnimInstance()
{
}

void USinAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	SinOwner = Cast<ASinCharacter>(GetOwningActor());
}

void USinAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	const ASinCharacter* Character = Cast<ASinCharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	FVector LocalVelocity = Character->GetVelocity();
	GroundSpeed = FVector(LocalVelocity.X, LocalVelocity.Y, 0.0f).Size();
	bStill = FMath::IsNearlyZero(GroundSpeed);
	FRotator BaseRotation = Character->GetActorRotation();
	//Direction = CalculateDirection(LocalVelocity, BaseRotation);
	Direction = UKismetAnimationLibrary::CalculateDirection(LocalVelocity, BaseRotation);
}

//void USinAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
//{
	//Super::NativeUpdateAnimation(DeltaSeconds);
//}