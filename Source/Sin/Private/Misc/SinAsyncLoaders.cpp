// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/SinAsyncLoaders.h"


void USinAsyncLoadersBase::Activate()
{
	Start(AssetClass);
}

void USinAsyncLoadersBase::Start(TSoftClassPtr<UObject> Asset)
{
	if (Asset.Get())
	{
		ReturnClass = Asset.Get(); 
		Loaded.Broadcast(ReturnClass); 
		SetReadyToDestroy(); 
	} 
	else
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(Asset.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(this, [this, Asset]
				{Loaded.Broadcast(ReturnClass = Asset.Get());
				})); SetReadyToDestroy();
	}
}

void USinAsyncLoadersBase::StartDeux(TArray<TSoftClassPtr<UObject>> Assets)
{
	for (auto It = Assets.CreateIterator(); It; ++It)
	{
		if (It->Get())
		{
			Classes.Add(It->Get());
		}
		else
		{
			UAssetManager::GetStreamableManager().RequestAsyncLoad(It->ToSoftObjectPath(),
				FStreamableDelegate::CreateWeakLambda(this, [this, Assets]
					{})); SetReadyToDestroy();
		}
		if (Assets.Num() == Classes.Num())
		{
		}
	}
}

USinClassLoader* USinClassLoader::SinAsyncLoadClass(UObject* WorldContextObject, TSoftClassPtr<class UObject> AssetClass)
{
	USinClassLoader* Task = NewObject<USinClassLoader>();
	Task->RegisterWithGameInstance(WorldContextObject);
	Task->CompletedStatus = EAssetLoadedOp::Completed;
	Task->AssetClass = AssetClass;

	return Task;
}

FVoidCoroutine USinClassLoader::SinLoadClassDeux(
	UObject* WorldContextObject,
	FLatentActionInfo LatentInfo,
	TSoftClassPtr<UObject> Asset,
	EAssetLoadedOp& Completed,
	UClass*& Class)
{
	Class = nullptr;

	Class = Cast<UClass>(
		co_await UE5Coro::Latent::AsyncLoadClass(Asset)
	);

	Completed = Class
		? EAssetLoadedOp::Completed
		: EAssetLoadedOp::Failed;
}
