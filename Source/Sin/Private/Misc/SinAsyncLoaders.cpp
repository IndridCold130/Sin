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

//FLatentActionInfo LatentInfo, 
//using namespace UE5Coro;

FAsyncCoroutine USinClassLoader::SinLoadClassDeux(UObject* WorldContextObject, TSoftClassPtr<class UObject> Asset, EAssetLoadedOp& Completed, UClass*& Class)
{
	this->RegisterWithGameInstance(WorldContextObject);
	Class = nullptr;
	//if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	//{
		//FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		//if (LatentActionManager.FindExistingAction<Start>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
	//}
	//UE5Coro::Latent::AsyncLoadClass
	co_await UE5Coro::Latent::AsyncLoadClass(Asset);
	Class = Asset.Get();
	Completed = EAssetLoadedOp::Completed;
	//co_await UE5Coro::Latent::Seconds(1.0f);
}



//Class = nullptr;
	//co_await 
	//if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	//{
		//FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		//LatentActionManager.
	//}
	//RegisterWithGameInstance(WorldContextObject);
	//UAssetManager::GetStreamableManager().RequestAsyncLoad(Asset.ToSoftObjectPath(),
			//FStreamableDelegate::CreateWeakLambda(this, [this, Asset, Class]
				//{Class = Asset.Get();
				//})); Completed = EAssetLoadedOp::Completed; return Asset.Get(); SetReadyToDestroy();
	//if (Asset.Get())
	//{
		//Class = Asset.Get();
		//ReturnClass = Asset.Get();
	//	Completed = EAssetLoadedOp::Completed;
		//SetReadyToDestroy();
	//}
	//else
	//{
		//Completed = EAssetLoadedOp::Failed;
		//UAssetManager::GetStreamableManager().RequestAsyncLoad(Asset.ToSoftObjectPath(),
			//FStreamableDelegate::CreateWeakLambda(this, [this, Asset]
				//{ReturnClass = Asset.Get();
				//})); Completed = EAssetLoadedOp::Completed; return Asset.Get(); SetReadyToDestroy();
	//}
	//Completed = Asset.Get()?  EAssetLoadedOp::Completed : EAssetLoadedOp::Failed;
	//return;
	//if(!Asset.Get())
	//{
		//Completed = EAssetLoadedOp::Failed;
		//return nullptr;
	//}
