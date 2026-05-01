// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "Templates/SubclassOf.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Inventory/Inventory.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "SinLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FTextureLoadedSignature, const UTexture2D*, Texture);
/**
 * 
 */
UCLASS(meta = (ScriptName = "SinBlueprintLibrary"))
class SIN_API USinLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
		//Get all Gameplay Tags from the Container that match the parent GPT.
		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "Get All Matching GPTs", BlueprintThreadSafe))
		static void GetAllMatchingGPTs(const FGameplayTagContainer& TagContainer, FGameplayTag BaseTag, bool& NotEmpty, TArray<FGameplayTag>& FoundTags);

		//Get all Gameplay Tags from the Container that match the parent GPT.
		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "Get first matching GPT", BlueprintThreadSafe))
		static void GetFirstMatchingGPT(const FGameplayTagContainer& TagContainer, FGameplayTag MasterTag, bool FindExact, bool& Found, FGameplayTag& Tag);
		// filter through tags
		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "Get first matching GPT", BlueprintThreadSafe))
		static void FilterGPTMap(FGameplayTag Filter, const TMap<FGameplayTag, float>& IncomingMap, TMap<FGameplayTag, float>& FilteredMap, bool& NotEmpty);

		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "Retrieve gameplay tag", BlueprintThreadSafe))
		static FGameplayTag GetTagFromAttribute(EPrimaryAttribute Attribute);

		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "Retrieve gameplay tag", BlueprintThreadSafe))
		static FGameplayTag GetTagFromDamageType(EDamageType DamageType);

		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "Retrieve gameplay tag", BlueprintThreadSafe))
		static FGameplayTag GetBlockTagFromDamageType(EDamageType DamageType);

		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "TagConversion", BlueprintThreadSafe))
		static TMap<FGameplayTag, float> ConvertAttributeMap(int32 MapIndex, const TMap<EPrimaryAttribute, float>& EnumMap);

		UFUNCTION(BlueprintPure, Category = "GameplayTags", meta = (Keywords = "TagConversion", BlueprintThreadSafe))
		static TMap<FGameplayTag, float> ConvertDamageMap(int32 MapIndex, const TMap<EDamageType, float>& EnumMap);

		//Get the default object representing the class.
		UFUNCTION(BlueprintPure, BlueprintCallable, meta = (DeterminesOutputType = "Class"), Category = "Utility")
			static UObject* GetDefaultObject(TSubclassOf<UObject> Class);

		UFUNCTION(BlueprintCallable, Category = "Inventory")
			static void CreateItem(FName ID, UDataTable* Table, int32 Quantity, bool& Created, UGameItemBase*& CreatedItem);
		UFUNCTION(BlueprintCallable, Category = "Animation Functions")
			static void SinSetAnimRootMotionTranslationScale(ACharacter* Character, float InAnimRootMotionTranslationScale = 1.f);
		//UFUNCTION(BlueprintCallable, Category = "SQL")
			//static void CreateElevatedProcess(FString Path, FString Command, int32& OutReturnCode);
			// BASED ON PlasticSourceControlShell.cpp / .h
		UFUNCTION(BlueprintCallable, Category = "SQL")
			static void CreateProcess(FString Path, FString Command, bool bDetached, bool bLaunchHidden)
			{
				static FProcHandle SQLiteHandle;
				const bool bLaunchReallyHidden = bLaunchHidden;
				static void* InputPipeRead = nullptr;
				static void* InputPipeWrite = nullptr;
				static void* OutputPipeRead = nullptr;
				static void* OutputPipeWrite = nullptr;

				verify(FPlatformProcess::CreatePipe(OutputPipeRead, OutputPipeWrite, false));	// For reading outputs from cm shell child process
				verify(FPlatformProcess::CreatePipe(InputPipeRead, InputPipeWrite, true));

				SQLiteHandle = FPlatformProcess::CreateProc(*Path, *Command, bDetached, bLaunchHidden, bLaunchReallyHidden, nullptr, 0, nullptr, OutputPipeWrite, InputPipeRead);
				if (!SQLiteHandle.IsValid())
				{
					FPlatformProcess::ClosePipe(OutputPipeRead, OutputPipeWrite);
					FPlatformProcess::ClosePipe(InputPipeRead, InputPipeWrite);
					//UE_LOG(LogSourceControl, Warning, TEXT("Failed to launch 'cm shell'"));
					return;
				}
				FPlatformProcess::WritePipe(InputPipeWrite, TEXT(".open game.db"));
				FPlatformProcess::WaitForProc(SQLiteHandle);
			}
			//FPlatformProcess::CreateProc(*Path, nullptr, false, false, false, nullptr, 0, nullptr, nullptr); <-- THIS WORKED

			UFUNCTION(BlueprintCallable, Category = "Texture")
				static UTexture2D* SinGetTextureRenderTargetDeux(USceneCaptureComponent2D* CaptureComp);

			UFUNCTION(BlueprintCallable, Category = "SinAsync", meta = (WorldContext = "WorldContextObject"))
				static void SinGetTextureRenderTargetTris(UObject* WorldContextObject, USceneCaptureComponent2D* CaptureComp, const FTextureLoadedSignature& Result);


};
