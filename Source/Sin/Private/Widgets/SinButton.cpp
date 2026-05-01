// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SinButton.h"

#include "Components/Button.h"
#include "Misc/SinCommonLibrary.h"

#define LOCTEXT_NAMESPACE "UMG"

void USinButton::SinButtonBind()
{	
	if (!OnClickSound.IsNull()){ USinCommonLibrary::PlaySoftSound2D(GetWorld(), OnClickSound); }
	OnSinButtonClicked.Broadcast(this);
}

void USinButton::LockSinButton_Implementation(bool Lock)
{
	Lock ? SetVisibility(ESlateVisibility::HitTestInvisible) : SetVisibility(ESlateVisibility::Visible);
}

void USinButton::SetSinButtonLabel_Implementation(const FText& Label)
{
}

void USinButton::NativeOnInitialized()
{
	SinButton->OnClicked.AddDynamic(this, &USinButton::SinButtonBind);
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
