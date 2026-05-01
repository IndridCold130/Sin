// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SinToolTip_Interact.h"


void USinToolTip_Interact::GetSinToolTipInfo_Implementation(const FString& Info)
{
	TArray<FString> unpackedValues;
	Info.ParseIntoArray(unpackedValues, TEXT(","), true);
	LabelText->SetText(FText::FromString(unpackedValues[0]));
	PromptText->SetText(FText::FromString(unpackedValues[1]));
}
