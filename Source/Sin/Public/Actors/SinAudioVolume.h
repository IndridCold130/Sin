// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/AudioVolume.h"
#include "SinGlobalStructs.h"
#include "SinAudioVolume.generated.h"

class UAudioComponent;

/**
 * 
 */
UCLASS()
class SIN_API ASinAudioVolume : public AAudioVolume
{
	GENERATED_UCLASS_BODY()
public:
	// Sets default values for this character's properties
	//ASinAudioVolume();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AmbientZone, meta = (AllowPrivateAccess = "true"))
		UDataTable* AmbientSoundsTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AmbientZone, meta = (AllowPrivateAccess = "true"))
		FName Location_ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UAudioComponent*> AmbientComponents;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	void SetSoundAsyncDeux(TSoftObjectPtr<USoundBase> Asset, UAudioComponent* Audio);
};
