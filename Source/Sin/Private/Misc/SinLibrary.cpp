// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/SinLibrary.h"

#include "SinCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

USinLibrary::USinLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USinLibrary::GetAllMatchingGPTs(const FGameplayTagContainer& TagContainer,FGameplayTag MasterTag, bool& NotEmpty, TArray<FGameplayTag>& FoundTags)
{
    for (auto It = TagContainer.CreateConstIterator(); It; ++It)
    {
        if (It->MatchesTag(MasterTag))
        {
           FoundTags.AddUnique(*It);
        }
    }
    NotEmpty = !FoundTags.IsEmpty();
}

void USinLibrary::GetFirstMatchingGPT(const FGameplayTagContainer& TagContainer, FGameplayTag MasterTag, bool FindExact, bool& Found, FGameplayTag& FoundTag)
{
    for (auto It = TagContainer.CreateConstIterator(); It; ++It)
    {
        if (FindExact)
        {
            if (It->MatchesTagExact(MasterTag))
            {
                Found = true;
                FoundTag = *It;
                return;
            }
        }
        else
        {
            if (It->MatchesTag(MasterTag))
            {
                Found = true;
                FoundTag = *It;
                return;
            }
        }
    }
    Found = false;
}

void USinLibrary::FilterGPTMap(FGameplayTag Filter, const TMap<FGameplayTag, float>& IncomingMap, TMap<FGameplayTag, float>& FilteredMap, bool& bHasItems)
{
    FGameplayTagContainer IncomingContainer;
    for (const auto& Pair : IncomingMap)
    {
        IncomingContainer.AddTag(Pair.Key);
    }
    if (IncomingContainer.IsEmpty())
    {
        bHasItems = false;
        return;
    }
    TArray<FGameplayTag> FoundTags;
    bool NotEmpty;
    GetAllMatchingGPTs(IncomingContainer, Filter, NotEmpty, FoundTags);
    if (!NotEmpty)
    {
        bHasItems = false;
        return;
    }
    FilteredMap.Empty();
    for (const auto& Pair : IncomingMap)
    {
        if (FoundTags.Contains(Pair.Key)) {
            FilteredMap.Add(Pair.Key, Pair.Value);
        }
    }
    bHasItems = true;
}

FGameplayTag USinLibrary::GetTagFromAttribute(EPrimaryAttribute Attribute)
{
    switch (Attribute)
    {
    case EPrimaryAttribute::Strength:     return TAG_Attribute_Major_Strength_Scaling;
    case EPrimaryAttribute::Dexterity:    return TAG_Attribute_Major_Dexterity_Scaling;
    case EPrimaryAttribute::Constitution:    return TAG_Attribute_Major_Constitution_Scaling;
    case EPrimaryAttribute::Sorcery:      return TAG_Attribute_Major_Sorcery_Scaling;
    case EPrimaryAttribute::Faith:      return TAG_Attribute_Major_Faith_Scaling;
    case EPrimaryAttribute::Charisma: return TAG_Attribute_Major_Charisma_Scaling;
        // etc...
    default:                     return FGameplayTag();
    }
}

FGameplayTag USinLibrary::GetTagFromDamageType(EDamageType DamageType)
{
    switch (DamageType)
    {
    case EDamageType::Slash:
        return TAG_Attribute_Secondary_DamageType_Physical_Slash;
    case EDamageType::Strike:
        return TAG_Attribute_Secondary_DamageType_Physical_Strike;
    case EDamageType::Pierce:
        return TAG_Attribute_Secondary_DamageType_Physical_Pierce;
    case EDamageType::Fire:
        return TAG_Attribute_Secondary_DamageType_Magic_Fire;
    case EDamageType::Cold:
        return TAG_Attribute_Secondary_DamageType_Magic_Cold;
    case EDamageType::Lightning:
        return TAG_Attribute_Secondary_DamageType_Magic_Lightning;
    case EDamageType::Arcane:
        return TAG_Attribute_Secondary_DamageType_Magic;
    case EDamageType::Dark:
        return TAG_Attribute_Secondary_DamageType_Magic_Dark;
    case EDamageType::Holy:
        return TAG_Attribute_Secondary_DamageType_Magic_Holy;
    case EDamageType::Poison:
        return TAG_Attribute_Secondary_DamageType_Internal_Poison;
    case EDamageType::Bleed:
        return TAG_Attribute_Secondary_DamageType_Internal_Bleed;
    case EDamageType::Disease:
        return TAG_Attribute_Secondary_DamageType_Internal_Disease;
    default:
        return TAG_Attribute_Secondary_DamageType_Physical_Slash; // Invalid / fallback
    }
}

FGameplayTag USinLibrary::GetBlockTagFromDamageType(EDamageType DamageType)
{
    switch (DamageType)
    {
    case EDamageType::Slash:     return TAG_Block_Slash;
    case EDamageType::Strike:    return TAG_Block_Strike;
    case EDamageType::Pierce:    return TAG_Block_Pierce;
    case EDamageType::Fire:      return TAG_Block_Fire;
    case EDamageType::Cold:      return TAG_Block_Cold;
    case EDamageType::Lightning: return TAG_Block_Lightning;
    case EDamageType::Arcane: return TAG_Block_Arcane;
    case EDamageType::Dark: return TAG_Block_Dark;
    case EDamageType::Holy: return TAG_Block_Holy;
        // etc...
    default:                     return FGameplayTag();
    }
}

TMap<FGameplayTag, float> USinLibrary::ConvertAttributeMap(int32 MapIndex, const TMap<EPrimaryAttribute, float>& EnumMap)
{
    TMap<FGameplayTag, float> TagMap;
    for (const TPair<EPrimaryAttribute, float>& Pair : EnumMap)
    {
        FGameplayTag Tag;
        Tag = GetTagFromAttribute(Pair.Key);
        if (Tag.IsValid())
        {
            TagMap.Add(Tag, Pair.Value);
        }
    }
    return TagMap;
}

TMap<FGameplayTag, float> USinLibrary::ConvertDamageMap(int32 MapIndex, const TMap<EDamageType, float>& EnumMap)
{
    TMap<FGameplayTag, float> TagMap;

    for (const TPair<EDamageType, float>& Pair : EnumMap)
    {
        FGameplayTag Tag;
        switch (MapIndex)
        {
        case 0: Tag = GetTagFromDamageType(Pair.Key); break;
        case 1: Tag = GetBlockTagFromDamageType(Pair.Key); break;
        default: GetBlockTagFromDamageType(Pair.Key); break;
        }
        if (Tag.IsValid())
        {
            TagMap.Add(Tag, Pair.Value);
        }
    }
    return TagMap;
}

UObject* USinLibrary::GetDefaultObject(TSubclassOf<UObject> Class)
{
    return Class.GetDefaultObject();
}

void USinLibrary::CreateItem(FName ID, UDataTable* Table, int32 Quantity, bool& Created, UGameItemBase*& CreatedItem)
{
    Created = false;
    CreatedItem = nullptr;
    if(!Table)
    {
        return;
    }
        FEquipmentItem* LocalRowOne = Table->FindRow<FEquipmentItem>(ID, TEXT("ContextString"), true);
        if (LocalRowOne)
        {
            TSubclassOf<UGameItemEquipment> DefaultClass = UGameItemEquipment::StaticClass();
            UGameItemEquipment* LocalItem = NewObject<UGameItemEquipment>(Table, LocalRowOne->ItemClass->IsChildOf(DefaultClass) ? LocalRowOne->ItemClass : DefaultClass);
            LocalItem->MaxStack = LocalRowOne->MaxStack;
            LocalItem->InitGameItemVars(ID, LocalRowOne->ItemTags, Table, Quantity);
            LocalItem->EquipmentData = *LocalRowOne;
            LocalItem->PrimaryType = EPrimaryItemType::Equipment;
            Created = true;
            CreatedItem = LocalItem;
            return;
        }
        FSinWeapon* LocalRowTwo = Table->FindRow<FSinWeapon>(ID, TEXT("ContextString"), true);
        if (LocalRowTwo)
        {
            UGameItemWeapon* LocalWeapon = NewObject<UGameItemWeapon>(Table);
            LocalWeapon->InitGameItemVars(ID, LocalRowTwo->ItemTags, Table);
            LocalWeapon->Stack = FMath::Clamp(Quantity, 1, LocalRowTwo->MaxStack);
            LocalWeapon->EquipmentData.Price = LocalRowTwo->Price;
            LocalWeapon->PrimaryType = EPrimaryItemType::Weapon;
            Created = true;
            CreatedItem = LocalWeapon;
            return;
        }
        FItemBase* LocalRowThree = Table->FindRow<FItemBase>(ID, TEXT("ContextString"), true);
        if (LocalRowThree)
        {
            TSubclassOf<UGameItemBase> DefaultClass = UGameItemBase::StaticClass();
            UGameItemBase* LocalItem = NewObject<UGameItemBase>(Table, LocalRowThree->ItemClass->IsChildOf(DefaultClass) ? LocalRowThree->ItemClass : DefaultClass);
            if (LocalItem->InitGameItemVars(ID, LocalRowThree->ItemTags, Table))
            {
                LocalItem->DefaultData = *LocalRowThree;
                if (LocalItem)
                {
                    LocalItem->Stack = FMath::Clamp(Quantity, 1, LocalRowThree->MaxStack);
                    LocalItem->PrimaryType = EPrimaryItemType::Regular;
                    Created = true;
                    CreatedItem = LocalItem;
                    return;
                }
            }
        }
}

void USinLibrary::SinSetAnimRootMotionTranslationScale(ACharacter* Character, float InAnimRootMotionTranslationScale)
{
    if (Character && (Character->GetLocalRole() == ROLE_Authority))
    {
        Character->SetAnimRootMotionTranslationScale(InAnimRootMotionTranslationScale);
    }
    //if (Character && (Character->GetLocalRole() == ROLE_Authority ||
     //   (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
}

UTexture2D* USinLibrary::SinGetTextureRenderTargetDeux(USceneCaptureComponent2D* CaptureComp)
{
    if (CaptureComp)
    {
        CaptureComp->CaptureScene();
        // Creates Texture2D to store TextureRenderTarget content
        UTexture2D* Texture = UTexture2D::CreateTransient(CaptureComp->TextureTarget->SizeX, CaptureComp->TextureTarget->SizeY);
#if WITH_EDITORONLY_DATA
        Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
        Texture->SRGB = false;
        Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

        // Read the pixels from the RenderTarget and store them in a FColor array
        TArray<FColor> SurfData;
        FRenderTarget* RenderTarget = CaptureComp->TextureTarget->GameThread_GetRenderTargetResource();
        RenderTarget->ReadPixels(SurfData);
        // DEBUG: set all alpha channels to 255
        for (auto& Pixel : SurfData)
        {
            Pixel.A = 255.f;
        }

        // Lock and copies the data between the textures
        void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
        const int32 TextureDataSize = SurfData.Num() * 4;
        FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
        Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
        // Apply Texture changes to GPU memory
        Texture->UpdateResource();
        return Texture;
    }
    return nullptr;
}

void USinLibrary::SinGetTextureRenderTargetTris(UObject* WorldContextObject, USceneCaptureComponent2D* CaptureComp, const FTextureLoadedSignature& Result)
{
    AsyncTask(ENamedThreads::GameThread, [=]()
        {
            if (CaptureComp)
            {
                CaptureComp->CaptureScene();
                // Creates Texture2D to store TextureRenderTarget content
                UTexture2D* Texture = UTexture2D::CreateTransient(CaptureComp->TextureTarget->SizeX, CaptureComp->TextureTarget->SizeY);
#if WITH_EDITORONLY_DATA
                Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
                Texture->SRGB = false;
                //Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

                // Read the pixels from the RenderTarget and store them in a FColor array
                TArray<FColor> SurfData;
                FRenderTarget* RenderTarget = CaptureComp->TextureTarget->GameThread_GetRenderTargetResource();
                RenderTarget->ReadPixels(SurfData);
                // set all alpha channels to 255
                for (auto& Pixel : SurfData)
                {
                    Pixel.A = 255.f;
                }

                // Lock and copies the data between the textures
                void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
                const int32 TextureDataSize = SurfData.Num() * 4;
                FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
                Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
                // Apply Texture changes to GPU memory
                Texture->UpdateResource();
                Result.ExecuteIfBound(Texture);
            }
        });
}

static FString GetLetterGradeWithSuffix(float ScaleValue)
{
    FString Grade;
    if (ScaleValue >= 0.80f) Grade = "S";
    else if (ScaleValue >= 0.60f) Grade = "A";
    else if (ScaleValue >= 0.40f) Grade = "B";
    else if (ScaleValue >= 0.20f) Grade = "C";
    else Grade = "D";

    // Add "+" if near the top of the range
    float Fraction = FMath::Fmod(ScaleValue * 100.f, 20.f) / 20.f;
    if (Fraction >= 0.85f && Grade != "S") Grade += "+";

    return Grade;
}
