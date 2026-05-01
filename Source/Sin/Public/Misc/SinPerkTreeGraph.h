// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraph.h"
#include "GAS/SinASC.h"

#include "SinPerkTreeGraph.generated.h"

class UTexture2D;
/**
 * 
 */
UCLASS(BlueprintType)
class SIN_API USinPerkTreeGraph : public UGenericGraph
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	TObjectPtr<UDataTable> ClassPerkTable;
};

UCLASS(BlueprintType)
class USinPerkTreeNode : public UGenericGraphNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	FName PerkID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	TSoftObjectPtr <UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	int32 Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	int32 Column;

};
