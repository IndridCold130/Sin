// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/SinCommonLibrary.h"

#include "Engine/AssetManager.h"
#include "Components/VerticalBoxSlot.h"
#include "GAS/SinAttributeProgression.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
//#include "EnhancedPlayerMappableKeyProfile.h"
#include "InputAction.h"

void USinCommonLibrary::GetAllMetaData(const UAnimSequenceBase* AnimationSequence, TArray<UAnimMetaData*>& MetaData)
{
	MetaData.Empty();
	if (AnimationSequence)
	{
		//AnimationSequence->FindMetaDataByClass();
		MetaData = AnimationSequence->GetMetaData();
	}
}

void USinCommonLibrary::GetFirstMetaData(const UAnimSequenceBase* AnimationSequence, UAnimMetaData*& MetaData)
{
	MetaData = nullptr;
	if (AnimationSequence)
	{
		if (AnimationSequence->GetMetaData().IsValidIndex(0))
		{
			MetaData = AnimationSequence->GetMetaData()[0];
		}
	}
}

void USinCommonLibrary::GetMontageSubstateTag(const UAnimSequenceBase* AnimationSequence, bool& Success, FGameplayTag& Substate)
{
	Success = false;
	if (AnimationSequence)
	{
		for (auto It(AnimationSequence->GetMetaData().CreateConstIterator()); It; ++It)
		{
			USinAnimMetaData* SinMeta = Cast< USinAnimMetaData>(*It);
				if (SinMeta)
				{
					Success = true;
					Substate = SinMeta->SubstateTag;
					break;
				}
		}
	}
}

ASinCharacter* USinCommonLibrary::SinPawnComponentOwner(UActorComponent* Component)
{
	return Cast<ASinCharacter>(Component->GetOwner());
}

ASinGameHUD* USinCommonLibrary::GetSinGameHUD(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		if (World->GetFirstPlayerController())
		{
			return Cast<ASinGameHUD>(World->GetFirstPlayerController()->MyHUD);
		}
	}
	return nullptr;
}

FText USinCommonLibrary::FormatPerkDescription(const int32 CurrentRank, const FSinPerkRow& PerkRow)
{
	int32 TranslatedRank = CurrentRank-1; int32 MaxRank = PerkRow.MaxRankBase; FText CurrentDescription; FText NextDescription;

	const FSinPerkRequirement* CurrentRankPtr = nullptr;
	const FSinPerkRequirement* NextRankPtr = nullptr;
	//const TArray<FSinPerkRequirement>* RequirementsPtr = nullptr;
	switch (PerkRow.PerkType)
	{
	case EPerkType::Passive:
		if (PerkRow.PassivePerks.IsValidIndex(TranslatedRank))
		{
			CurrentRankPtr = &PerkRow.PassivePerks[TranslatedRank].Requirements; CurrentDescription = CurrentRankPtr->RankDescription;
		}
		if (PerkRow.PassivePerks.IsValidIndex(TranslatedRank+1))
		{
			NextRankPtr = &PerkRow.PassivePerks[TranslatedRank+1].Requirements; NextDescription = NextRankPtr->RankDescription;
		}
		break;
	case EPerkType::Proc:
		if (PerkRow.Proc.Requirements.IsValidIndex(TranslatedRank))
		{
			CurrentRankPtr = &PerkRow.Proc.Requirements[TranslatedRank];CurrentDescription = CurrentRankPtr->RankDescription;
		}
		if (PerkRow.Proc.Requirements.IsValidIndex(TranslatedRank+1))
		{
			CurrentRankPtr = &PerkRow.Proc.Requirements[TranslatedRank+1]; NextDescription = CurrentRankPtr->RankDescription;
		}
		break;
	case EPerkType::UniqueEffect:
	default:
		break;
	}
	if (CurrentDescription.IsEmpty()&& !NextDescription.IsEmpty())
	{
		return FText::Format(
			NSLOCTEXT("Perk", "FormattedPerkDescription",
				"<Perk.Misc>Next Rank:</>\n"
				"<Perk.Description>{0}</>"
			),
			NextDescription
		);
	}
	if (!NextDescription.IsEmpty())
	{
		return FText::Format(
			NSLOCTEXT("Perk", "FormattedPerkDescription",
				"<Perk.Description>{0}</>\n\n"
				"<Perk.Misc>Next Rank:</>\n"
				"<Perk.Description>{1}</>"
			),
			CurrentDescription,
			NextDescription
		);
	}
	return FText::Format(
		NSLOCTEXT("Perk", "SinglePerkDescription",
			"<Perk.Description>{0}</>"
		),
		CurrentDescription
	);
}

 // TSoftClassPtr<UObject>
// TSubclassOf<UObject>
// TSubclassOf<UClass>
FAsyncCoroutine USinCommonLibrary::SinLoadClassTris(FLatentActionInfo LatentInfo, TSoftClassPtr<UObject> Asset, EAssetLoadedOp& Completed, TSubclassOf<UObject>& Class)
{
	Class = Cast<UClass>(co_await UE5Coro::Latent::AsyncLoadClass(Asset));
	//Class = co_await UE5Coro::Latent::AsyncLoadClass(Asset);
	Completed = EAssetLoadedOp::Completed;
}

FAsyncCoroutine USinCommonLibrary::SinLoadDataTable(FLatentActionInfo LatentInfo, TSoftObjectPtr<UObject> SoftTable, EAssetLoadedOp& Completed, UDataTable*& Table)
{
	Table = nullptr;
	UObject* LoadedObject = co_await UE5Coro::Latent::AsyncLoadObject(SoftTable);
	Table = Cast<UDataTable>(LoadedObject);

	Completed = Table ? EAssetLoadedOp::Completed : EAssetLoadedOp::Failed;
}

FAsyncCoroutine USinCommonLibrary::SinLoadSkeletalMeshes(FLatentActionInfo LatentInfo, TArray<TSoftObjectPtr<UObject>> Assets, EAssetLoadedOp& Completed, TArray<UObject*>& SkeletalMeshes)
{
	SkeletalMeshes = co_await UE5Coro::Latent::AsyncLoadObjects(Assets);
	Completed = EAssetLoadedOp::Completed;
}

UClass* USinCommonLibrary::SinLoadClassQuatre(FLatentActionInfo LatentInfo, TSubclassOf<UObject> Asset, TSoftClassPtr<UObject> AssetTwo, EAssetLoadedOp& Completed)
{
	AssetTwo = Asset.Get();
	return nullptr;
}

USinAnimMetaData::USinAnimMetaData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncTaskAttributeChanged* UAsyncTaskAttributeChanged::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UAsyncTaskAttributeChanged* WaitForAttributeChangedTask = NewObject<UAsyncTaskAttributeChanged>();
	WaitForAttributeChangedTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangedTask->AttributeToListenFor = Attribute;
	if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
	{
		WaitForAttributeChangedTask->RemoveFromRoot();
		return nullptr;
	}
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangedTask, &UAsyncTaskAttributeChanged::AttributeChanged);
	return WaitForAttributeChangedTask;
}

int USinCommonLibrary::RequiredExperienceForNextLevel(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return 0;
	}
	bool Found;
	float LocLevel = ASC->GetGameplayAttributeValue(USinAttributeProgression::GetLevelAttribute(), Found);
	if (LocLevel == 0.f)
	{
		return 0;
	}
	float LocalCoefficient = 600.0;
	return FMath::TruncToInt(FMath::Max(LocLevel * (LocLevel - 1) * LocalCoefficient, LocalCoefficient));
}

USceneComponent* USinCommonLibrary::GetMainCharacterMesh(AActor* Owner)
{
	return Owner->FindComponentByTag<USceneComponent>(FName("CharacterMesh"));
}

void USinCommonLibrary::AttachMeshBasedOnTransform(USceneComponent* Master, FName MasterSocket, USceneComponent* Child, FName Attachment)
{
	if(!Master || !Child)
	{
		return;
	}
	Child->AttachToComponent(Master, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), MasterSocket);
	FTransform RelativeTransform = Master->GetSocketTransform(MasterSocket, ERelativeTransformSpace::RTS_Actor).GetRelativeTransform(Child->GetSocketTransform(Attachment, ERelativeTransformSpace::RTS_Actor));
	Child->SetRelativeTransform(RelativeTransform);
	Child->SetHiddenInGame(false);
}

UAsyncTaskAttributeChanged* UAsyncTaskAttributeChanged::ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> Attributes)
{
	UAsyncTaskAttributeChanged* WaitForAttributeChangedTask = NewObject<UAsyncTaskAttributeChanged>();
	WaitForAttributeChangedTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangedTask->AttributesToListenFor = Attributes;
	if (!IsValid(AbilitySystemComponent) || Attributes.Num() < 1)
	{
		WaitForAttributeChangedTask->RemoveFromRoot();
		return nullptr;
	}
	// Prevent garbage collection
	//WaitForAttributeChangedTask->AddToRoot();
	for (FGameplayAttribute Attribute : Attributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangedTask, &UAsyncTaskAttributeChanged::AttributeChanged);
	}
	return WaitForAttributeChangedTask;
}

void UAsyncTaskAttributeChanged::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

		for (FGameplayAttribute Attribute : AttributesToListenFor)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		}
	}
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsyncTaskAttributeChanged::AttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}

void USinCommonLibrary::PlaySoftSoundAtLocation(UObject* WorldContextObject, TSoftObjectPtr<USoundBase> SoftSound, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings, UInitialActiveSoundParams* InitialParams)
{
	if (auto* sound = SoftSound.Get())
	{
		UGameplayStatics::PlaySoundAtLocation(WorldContextObject, sound, Location, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, InitialParams);
	}
	else if (!SoftSound.IsNull())
	{
		UAssetManager::GetStreamableManager()
			.RequestAsyncLoad(SoftSound.ToSoftObjectPath(),
				[WorldContextObject, SoftSound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, InitialParams]
				{PlaySoftSoundAtLocation(WorldContextObject, SoftSound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, InitialParams); }
			);
	}
}

void USinCommonLibrary::PlaySoftSound2D(UObject* WorldContextObject, TSoftObjectPtr<USoundBase> SoftSound, float VolumeM, float PitchM, float StartTime)
{
	if (auto* sound = SoftSound.Get())
	{
		UGameplayStatics::PlaySound2D(WorldContextObject, sound, VolumeM, PitchM, StartTime);
	}
	else if (!SoftSound.IsNull())
	{
		UAssetManager::GetStreamableManager()
			.RequestAsyncLoad(SoftSound.ToSoftObjectPath(),
				[WorldContextObject, sound, VolumeM, PitchM, StartTime]
				{PlaySoftSound2D(WorldContextObject, sound, VolumeM, PitchM, StartTime); }
			);
	}
}

URichTextBlock* USinCommonLibrary::AddStyledRichTextBlock(UPanelWidget* Panel, UDataTable* TextStyleSet, const FString Style, const FText& InText, EHorizontalAlignment Alignment)
{
	if (!Panel || !TextStyleSet) return nullptr; URichTextBlock* RichText = NewObject<URichTextBlock>(Panel); if (!RichText) return nullptr;
	
	RichText->SetTextStyleSet(TextStyleSet);
	const FString Markup = FString::Printf(TEXT("<%s>%s</>"), *Style, *InText.ToString());
	// RichText->SetDefaultTextStyle(TextStyleSet);
	FText FinalText = FText::FromString(Markup);
	RichText->SetText(FinalText);
	UPanelSlot* NewSlot = Panel->AddChild(RichText);
	if (UVerticalBoxSlot* VBSlot = Cast<UVerticalBoxSlot>(NewSlot))
	{
		VBSlot->SetHorizontalAlignment(Alignment);
	}
	return RichText;
}

FText USinCommonLibrary::MakeRichText(FName StyleName, const FText& Text)
{
	FString SafeText = Text.ToString();

	// Normalize line endings.
	SafeText.ReplaceInline(TEXT("\r\n"), TEXT("\n"));

	// Prevent accidental rich text parsing.
	SafeText.ReplaceInline(TEXT("&"), TEXT("&amp;"));
	SafeText.ReplaceInline(TEXT("<"), TEXT("&lt;"));
	SafeText.ReplaceInline(TEXT(">"), TEXT("&gt;"));

	return FText::FromString(
		FString::Printf(
			TEXT("<%s>%s</>"),
			*StyleName.ToString(),
			*SafeText
		)
	);
}

FText USinCommonLibrary::MakeRichTextParagraphs(FName StyleName, const TArray<FText>& Paragraphs)
{
	FString Result;

	for (int32 i = 0; i < Paragraphs.Num(); ++i)
	{
		Result += MakeRichText(
			StyleName,
			Paragraphs[i]
		).ToString();

		// Add paragraph spacing.
		if (i < Paragraphs.Num() - 1)
		{
			Result += TEXT("\n\n");
		}
	}

	return FText::FromString(Result);
}

FText USinCommonLibrary::GetKeyDisplayTextForInputAction(APlayerController* PlayerController,
                                                         const UInputAction* InputAction, bool bGamepadMode)
{
	if (!PlayerController || !InputAction)
	{
		return FText::GetEmpty();
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return FText::GetEmpty();
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!Subsystem)
	{
		return FText::GetEmpty();
	}

	UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings();
	if (!Settings)
	{
		return FText::GetEmpty();
	}

	UEnhancedPlayerMappableKeyProfile* Profile = Settings->GetCurrentKeyProfile();
	if (!Profile)
	{
		return FText::GetEmpty();
	}

	const TMap<FName, FKeyMappingRow>& Rows = Profile->GetPlayerMappingRows();

	for (const TPair<FName, FKeyMappingRow>& RowPair : Rows)
	{
		const FKeyMappingRow& Row = RowPair.Value;

		for (const FPlayerKeyMapping& Mapping : Row.Mappings)
		{
			if (Mapping.GetMappingName() == InputAction->GetFName())
			{
				const FKey Key = Mapping.GetCurrentKey();

				if (bGamepadMode != Key.IsGamepadKey())
				{
					continue;
				}

				return Key.GetDisplayName(false);
			}
		}
	}
	// 2. Fallback: currently active mapping contexts
	const TArray<FKey> RuntimeKeys = Subsystem->QueryKeysMappedToAction(InputAction);

	for (const FKey& Key : RuntimeKeys)
	{
		if (Key.IsGamepadKey() == bGamepadMode)
		{
			return Key.GetDisplayName(false);
		}
	}
	
	return FText::GetEmpty();
}

UAsyncWidgetLoader* UAsyncWidgetLoader::AsyncLoadWidgetClass(TSoftClassPtr<UUserWidget> SoftWidgetClass)
{
	UAsyncWidgetLoader* AsyncLoader = NewObject<UAsyncWidgetLoader>();
	AsyncLoader->WidgetClass = SoftWidgetClass;

	if (SoftWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("AsyncWidgetLoader: SoftWidgetClass is null!"));
		return nullptr;
	}

	// If the class is already loaded, broadcast immediately
	UClass* LoadedClass = SoftWidgetClass.Get();
	if (LoadedClass)
	{
		UE_LOG(LogTemp, Log, TEXT("AsyncWidgetLoader: Class already loaded!"));
		AsyncLoader->OnSuccess.Broadcast(LoadedClass);
		return AsyncLoader;
	}

	// Otherwise, perform async load
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	AsyncLoader->StreamHandle = Streamable.RequestAsyncLoad(
		SoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(AsyncLoader, &UAsyncWidgetLoader::OnClassLoaded)
	);

	return AsyncLoader;
}

void UAsyncWidgetLoader::OnClassLoaded()
{
	UClass* LoadedClass = WidgetClass.Get();
	if (LoadedClass)
	{
		OnSuccess.Broadcast(LoadedClass);
	}
}
