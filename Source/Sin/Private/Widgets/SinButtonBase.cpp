// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SinButtonBase.h"

void USinButtonBase::SpecialBind()
{
	OnSpecialClicked.Broadcast(this);
}

void USinButtonBase::NativeOnInitialized()
{
	SpecialButton->OnClicked.AddDynamic(this, &USinButtonBase::SpecialBind);
}
