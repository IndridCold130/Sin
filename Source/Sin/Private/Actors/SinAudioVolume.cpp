// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SinAudioVolume.h"
#include "Components/BrushComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"


ASinAudioVolume::ASinAudioVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(TEXT("AudioTriggerOverlap"), true);
}

void ASinAudioVolume::BeginPlay() {
	Super::BeginPlay();
}

void ASinAudioVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	APawn* PotentialPlayer = Cast<APawn>(OtherActor);
	if (!PotentialPlayer)
	{
		return;
	}
	//if (PotentialPlayer && AmbientComponents.IsEmpty())
		if (PotentialPlayer->IsPlayerControlled()&&AmbientSoundsTable)
	{
		for (auto It = Tags.CreateIterator(); It; ++It)
		{
			FSinSoundAmbience* LocData = AmbientSoundsTable->FindRow<FSinSoundAmbience>(*It, TEXT("ContextString"), true);
			if (LocData)
			{
				for (auto That = LocData->Sounds.CreateIterator(); That; ++That)
				{
					if (AmbientComponents.IsValidIndex(That.GetIndex()))
					{
						SetSoundAsyncDeux(*That, AmbientComponents[That.GetIndex()]);
					}
					else 
					{
						UAudioComponent* LocalAudio = Cast<UAudioComponent>(AddComponentByClass(UAudioComponent::StaticClass(), true, GetActorTransform(), false));
						AmbientComponents.Add(LocalAudio);
						SetSoundAsyncDeux(*That, LocalAudio);
					}
				}
			}
		}
	}
}

void ASinAudioVolume::SetSoundAsyncDeux(TSoftObjectPtr<USoundBase> Asset, UAudioComponent* Audio)
{
		if (auto* sound = Asset.Get())
		{
			Audio->SetSound(Asset.LoadSynchronous());
			Audio->FadeIn(4.5, 1.0, 0.0);
		}
		else if (!Asset.IsNull())
		{
			UAssetManager::GetStreamableManager()
				.RequestAsyncLoad(Asset.ToSoftObjectPath(),
					[this, Asset, Audio]
					{SetSoundAsyncDeux(Asset, Audio); }
			);
		}
}




//if (UBuoyancyComponent* BuoyancyComponent = OtherActor->FindComponentByClass<UBuoyancyComponent>())
	//{
		//BuoyancyComponent->EnteredWaterBody(GetWaterBodyComponent());
	//}