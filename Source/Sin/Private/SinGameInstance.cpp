// Fill out your copyright notice in the Description page of Project Settings.


#include "SinGameInstance.h"

void USinGameInstance::Init()
{
	Super::Init();
	InstanceBeginPlay();
}

void USinGameInstance::Shutdown()
{
	Super::Shutdown();
}

void USinGameInstance::InstanceBeginPlay_Implementation()
{
}
