// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable.h"

#include "SinPlayerController.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

// Sets default values
AInteractable::AInteractable()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("Act");
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection>MaterialCollection(TEXT("/Game/Systems/MatCollections/MPC_OutlineColours"));
	if (MaterialCollection.Succeeded())
	{
		OutlineCollection = MaterialCollection.Object;
	}
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Mesh->AttachToComponent(Root, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), NAME_None);
	Super::BeginPlay();
	
}

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractable::Focus_Implementation(ASinPlayerController* Controller)
{
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(3);
	Controller->MainHUD->ToggleToolTip(true, GetToolTipInfo_Implementation());
	if (OutlineCollection)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		CollectionInstance = World->GetParameterCollectionInstance(OutlineCollection);
		FLinearColor OutlineColor;
		CollectionInstance->GetVectorParameterValue(GetOutlineColour_Implementation(), OutlineColor);
		//Controller->SetOutlineColour_Implementation(OutlineColor);
		if (!CameraMod.IsEmpty())
		{
			AppliedCamMod = Controller->PlayerCameraManager->AddNewCameraModifier(CameraMod[0]);
		}
	}
}

UInventory* AInteractable::GetInventory_Implementation()
{
	return nullptr;
}

UEquipment* AInteractable::GetEquipment_Implementation()
{
	return nullptr;
}

FName AInteractable::GetOutlineColour_Implementation()
{
	return FName();
}

void AInteractable::EndFocus_Implementation(ASinPlayerController* Controller)
{
	Mesh->SetRenderCustomDepth(false);
	Mesh->SetCustomDepthStencilValue(0);
	Controller->MainHUD->ToggleToolTip(false);
	if (AppliedCamMod)
	{
		bool bRemoved;
		bRemoved = Controller->PlayerCameraManager->RemoveCameraModifier(AppliedCamMod);
		AppliedCamMod = nullptr;
	}
}

void AInteractable::Interact_Implementation(ASinPlayerController* Caller)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.3f, FColor::Red, TEXT("Did We Hit Anything %s"));
}

FString AInteractable::GetToolTipInfo_Implementation()
{
	//FString LocalInfo = Label;
	//FString LocalInfo = FString::Printf(TEXT("%s"),*Label);
	FString LocalInfo = FString::Printf(TEXT("%s,%s"), *Label, *InteractionPrompt);
	//FString LocalInfo = FString::Printf(TEXT("{\"Label\": \"%s\", \"Prompt\": %s}"), *Label, *InteractionPrompt);
	return LocalInfo;
}

FString AInteractable::GetInteractableLabel_Implementation()
{
	return Label;
}

