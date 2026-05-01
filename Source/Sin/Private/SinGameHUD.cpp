// Fill out your copyright notice in the Description page of Project Settings.


#include "SinGameHUD.h"

void ASinGameHUD::BeginPlay()
{
	Super::BeginPlay();
	SinController =Cast<ASinPlayerController>(GetOwningPlayerController());
	if (SinController)
	{
		SinController->OnClientPossessedDelegate.AddDynamic(this, &ASinGameHUD::GenerateHUD);
		ASinCharacter* LocalCharacter = Cast<ASinCharacter>(SinController->GetPawn());
		if (LocalCharacter)
		{
			GenerateHUD();
			return;
		}
		//SinController->OnClientPossessedDelegate.AddDynamic(this, &ASinGameHUD::GenerateHUD);
	}
}

void ASinGameHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASinGameHUD::GenerateHUD_Implementation()
{
	if (SinController->MainHUD)
	{
		SinController->MainHUD->RemoveFromParent();
		SinController->MainHUD = nullptr;
	}
	UHUD = CreateWidget<USinHUD>(SinController, UHUD_Subclass);
	if (UHUD)
	{
		UHUD->AddToViewport();
		SinController->MainHUD = UHUD;
	}
}

USinItemTooltip* ASinGameHUD::GetOrCreateTooltip()
{
	if (!ActiveTooltip) // If tooltip doesn't exist, create it
	{
		UClass* LoadedClass = TooltipWidgetClass.LoadSynchronous();
		if (LoadedClass)
		{
			ActiveTooltip = CreateWidget<USinItemTooltip>(GetWorld(), LoadedClass);
			ActiveTooltip->AddToViewport(33);
		}
	}
	return ActiveTooltip;
}

void ASinGameHUD::ShowTooltip(bool bShow, UUserWidget* Master, UGameItemBase* GameItem)
{
	if (!GetOrCreateTooltip()) { return; }
	else {
		if (!bShow) {
			GetOrCreateTooltip()->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
		GetOrCreateTooltip()->ShowTooltip(Master, GameItem);
	}
}

void USinItemTooltip::ShowTooltip_Implementation(UUserWidget* Master, UGameItemBase* GameItem)
{
}

void USinItemTooltip::NativeConstruct()
{
	Super::NativeConstruct();
}

void USinItemTooltip::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Collapsed);
}
