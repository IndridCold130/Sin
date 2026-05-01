// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SinCam.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIN_API USinCam : public UCameraComponent
{
	GENERATED_BODY()

public:

	USinCam();

	TObjectPtr<USpringArmComponent> Arm;

	UFUNCTION(BlueprintCallable, Category = "SinCamera")
		void SetTargetLength(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinCamera")
		float MinArmLength = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SinCamera")
		float MaxArmLength = 550.f;

	float ProjectedLength = 300.f;

	UFUNCTION()
		void SmoothCam(float Delta);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//virtual void SetComponentTickEnabled(bool bEnabled) override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
