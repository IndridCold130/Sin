// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/SoftObjectPath.h"
#include "SinGlobalStructs.h"
//#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Templates/SubclassOf.h"
#include "UE5Coro.h"
//#include "Experimental/Async/AwaitableTask.h"
//#include "Experimental/Coroutine/CoroEvent.h"
//#include "Experimental/Coroutine/CoroParallelFor.h"
//#include "Experimental/Coroutine/CoroSpinLock.h"
//#include "Experimental/Coroutine/CoroTimeout.h"

#include "SinAsyncLoaders.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReturnClassDelegate, TSubclassOf<UObject>, ReturnClass);

UCLASS(Abstract)
class SIN_API USinAsyncLoadersBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Execute the actual load */
	virtual void Activate() override;

	//UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DeterminesOutputType = "AssetClass", WorldContext = "WorldContextObject"), Category = "Async")
	//static USinAsyncLoadersBase* SinAsyncLoadClass(UObject* WorldContextObject, TSoftClassPtr<class UObject> AssetClass); //TSoftClassPtr<UObject> AssetClass 

	UPROPERTY(BlueprintAssignable)
		FReturnClassDelegate Loaded;

	UPROPERTY(BlueprintAssignable)
		FReturnClassDelegate Failed;

	UFUNCTION(meta = (DeterminesOutputType = "Class"))
	void Start(TSoftClassPtr<UObject> Asset);

	UFUNCTION(meta = (DeterminesOutputType = "Class"))
		void StartDeux(TArray<TSoftClassPtr<UObject>> Assets);

protected:

	/** Specific assets requested */
	TSoftClassPtr<UObject> AssetClass;

	TArray<TSoftClassPtr<UObject>> AssetClasses;

	TSubclassOf<UObject> ReturnClass;

	TArray<TSubclassOf<UObject>> Classes;

	EAssetLoadedOp CompletedStatus;

};


UCLASS()
class SIN_API USinClassLoader : public USinAsyncLoadersBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DeterminesOutputType = "Asset", WorldContext = "WorldContextObject"), Category = "SinAsync")
		static USinClassLoader* SinAsyncLoadClass(UObject* WorldContextObject, TSoftClassPtr<class UObject> Asset); //TSoftClassPtr<UObject> AssetClass 

	UFUNCTION(BlueprintCallable, Category = "SinAsync",
	meta = (Latent, LatentInfo = "LatentInfo", DeterminesOutputType = "Asset", WorldContext = "WorldContextObject", ExpandEnumAsExecs = "Completed"))
	static FVoidCoroutine SinLoadClassDeux(
	UObject* WorldContextObject,
	FLatentActionInfo LatentInfo,
	TSoftClassPtr<UObject> Asset,
	EAssetLoadedOp& Completed,
	UClass*& Class);
};
