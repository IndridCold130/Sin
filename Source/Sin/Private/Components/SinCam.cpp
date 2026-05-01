// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SinCam.h"


USinCam::USinCam()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USinCam::SetTargetLength(float Value)
{
	ProjectedLength = FMath::Clamp(Value * -60 + ProjectedLength, MinArmLength, MaxArmLength);
	SetComponentTickEnabled(true);
}

void USinCam::SmoothCam(float Delta)
{
	Arm->TargetArmLength = FMath::FInterpTo(Arm->TargetArmLength, ProjectedLength, Delta, 4.0f);
	Arm->CameraLagMaxDistance = Arm->TargetArmLength * 0.2;
}

void USinCam::BeginPlay()
{
	Super::BeginPlay();
	Arm = Cast<USpringArmComponent>(GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass()));
	ProjectedLength = Arm->TargetArmLength;

	// ...

}

void USinCam::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (Arm)
	{
		if (Arm->TargetArmLength != ProjectedLength)
		{
			SmoothCam(DeltaTime);
		}
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}
