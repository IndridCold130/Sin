// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/StreamableManager.h"
#include "SinCharacter.h"
#include "SinGameHUD.h"
#include "Animation/AnimMetaData.h"
#include "UE5Coro.h"
#include "Engine/AssetManager.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/RichTextBlock.h"

#include "SinCommonLibrary.generated.h"

class APlayerController;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);
/**
 * 
 */
UCLASS()
class SIN_API USinCommonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
		/** Retrieves all Meta Data Instances from the given Animation Sequence */
	UFUNCTION(BlueprintPure, Category = "Animation")
		static void GetAllMetaData(const UAnimSequenceBase* AnimationSequence, TArray<UAnimMetaData*>& MetaData);

	UFUNCTION(BlueprintPure, Category = "Animation")
		static void GetFirstMetaData(const UAnimSequenceBase* AnimationSequence, UAnimMetaData*& MetaData);

	UFUNCTION(BlueprintPure, Category = "Animation")
		static void GetMontageSubstateTag(const UAnimSequenceBase* AnimationSequence, bool& Success, FGameplayTag& Substate);

	UFUNCTION(BlueprintPure, Category = "Character")
		static ASinCharacter* SinPawnComponentOwner(UActorComponent* Component);

	UFUNCTION(BlueprintPure, Category = "HUD", meta = (WorldContext = "WorldContextObject"))
		static ASinGameHUD* GetSinGameHUD(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "UI|Perks", meta = (WorldContext = "WorldContextObject"))
		static FText FormatPerkDescription(const int32 CurrentRank, const FSinPerkRow& PerkRow); // , UDataTable* Attribute

	UFUNCTION(BlueprintCallable, Category = "SinAsync", meta = (Latent, LatentInfo = "LatentInfo", DeterminesOutputType = "Asset", ExpandEnumAsExecs = "Completed", BlueprintAutocast))
		static FVoidCoroutine SinLoadClassTris(FLatentActionInfo LatentInfo, TSoftClassPtr<UObject> Asset, EAssetLoadedOp& Completed, TSubclassOf<UObject>& Class);

	UFUNCTION(BlueprintCallable, Category = "SinAsync", meta = (Latent, LatentInfo = "LatentInfo", DeterminesOutputType = "Asset", ExpandEnumAsExecs = "Completed", BlueprintAutocast))
		static FVoidCoroutine SinLoadDataTable(FLatentActionInfo LatentInfo, TSoftObjectPtr<UObject> SoftTable, EAssetLoadedOp& Completed, UDataTable*& Table);

	UFUNCTION(BlueprintCallable, Category = "SinAsync", meta = (Latent, LatentInfo = "LatentInfo", DeterminesOutputType = "Asset", ExpandEnumAsExecs = "Completed", BlueprintAutocast))
		static FVoidCoroutine SinLoadSkeletalMeshes(FLatentActionInfo LatentInfo, TArray<TSoftObjectPtr<UObject>> Assets, EAssetLoadedOp& Completed, TArray<UObject*>& SkeletalMeshes);

	UFUNCTION(BlueprintCallable, Category = "SinAsync", meta = (Latent, LatentInfo = "LatentInfo", DeterminesOutputType = "AssetTwo", ExpandEnumAsExecs = "Completed")) //BlueprintAutocast
		static UClass* SinLoadClassQuatre(FLatentActionInfo LatentInfo, TSubclassOf<UObject> Asset, TSoftClassPtr<UObject> AssetTwo, EAssetLoadedOp& Completed);

	UFUNCTION(BlueprintPure, Category = "Progression")
	static int RequiredExperienceForNextLevel(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintPure, Category = "MeshManagement")
	static USceneComponent* GetMainCharacterMesh(AActor* Owner);

	UFUNCTION(BlueprintCallable, Category = "MeshManagement")
	static void AttachMeshBasedOnTransform(USceneComponent* Master, FName MasterSocket, USceneComponent* Child, FName SocketTwo);

	UFUNCTION(BlueprintCallable, Category = "Sound", meta = (WorldContext = "WorldContextObject"))
	static void PlaySoftSoundAtLocation(UObject* WorldContextObject, TSoftObjectPtr<USoundBase> SoftSound, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings=nullptr, USoundConcurrency* ConcurrencySettings=nullptr, UInitialActiveSoundParams* InitialParams=nullptr);
public:
	UFUNCTION(BlueprintCallable, Category = "Sound", meta = (WorldContext = "WorldContextObject"))
	static void PlaySoftSound2D(UObject* WorldContextObject, TSoftObjectPtr<USoundBase> SoftSound, float VolumeM =1.0f, float PitchM=1.0f, float StartTime=0.0f);

	UFUNCTION(BlueprintCallable, Category = "UI|Rich Text")
	static URichTextBlock* AddStyledRichTextBlock(UPanelWidget* Panel, UDataTable* TextStyleSet, const FString Style, const FText& InText, EHorizontalAlignment Alignment = HAlign_Left);
	
	UFUNCTION(BlueprintPure, Category="UI|Rich Text")
	static FText MakeRichText(FName StyleName, const FText& Text);
	
	UFUNCTION(BlueprintPure, Category="UI|Rich Text")
	static FText MakeRichTextParagraphs(FName StyleName, const TArray<FText>& Paragraphs);
	
	UFUNCTION(BlueprintPure, Category="Input|Display")
	static FText GetKeyDisplayTextForInputAction(
		APlayerController* PlayerController,
		const UInputAction* InputAction,
		bool bGamepadMode
	);
};

// last working static TSubclassOf<UObject> SinLoadClassQuatre(FLatentActionInfo LatentInfo, UClass* Asset, EAssetLoadedOp& Completed);
// static UClass* SinLoadClassQuatre(FLatentActionInfo LatentInfo, TSubclassOf<UObject> Asset, EAssetLoadedOp& Completed);


UCLASS(Blueprintable, abstract, editinlinenew, hidecategories = Object, collapsecategories, MinimalAPI)
class USinAnimMetaData : public UAnimMetaData
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (ExposeOnSpawn = "true"))
		FGameplayTag SubstateTag = TAG_Substate_Attack;
};


/**
 * Blueprint node to automatically register a listener for all attribute changes in an AbilitySystemComponent.
 * Useful to use in UI.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class SIN_API UAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	// Listens for an attribute changing.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);
	// Listens for an attribute changing.
	// Version that takes in an array of Attributes. Check the Attribute output for which Attribute changed.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> Attributes);

	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};

/**
 * Async Node for Loading Soft Widget Classes and Auto-Resolving the Output
 */
UCLASS()
class SIN_API UAsyncWidgetLoader : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Delegate for when the class is loaded */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClassLoaded, UClass*, LoadedClass);

	/** The event triggered when the class is successfully loaded */
	UPROPERTY(BlueprintAssignable)
	FOnClassLoaded OnSuccess;

	/**
	 * Loads a widget class asynchronously and returns the resolved class automatically.
	 * @param SoftWidgetClass The soft class reference to load.
	 * @return The Async Loader instance.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Async|UI")
	static UAsyncWidgetLoader* AsyncLoadWidgetClass(TSoftClassPtr<UUserWidget> SoftWidgetClass);

	/** Called when the asset is loaded */
	void OnClassLoaded();

private:
	/** The soft reference being loaded */
	TSoftClassPtr<UUserWidget> WidgetClass;

	/** Streamable manager for asset loading */
	TSharedPtr<FStreamableHandle> StreamHandle;
};