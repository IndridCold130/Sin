// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/I_Interactable.h"
#include "Inventory/Equipment.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Camera/CameraModifier.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "Interactable.generated.h"

class ASinPlayerController;
class UMaterialParameterCollection;

#define LOCTEXT_NAMESPACE "MyNamespace"

#define LOCTEXT(InKey, InTextLiteral) FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(TEXT(InTextLiteral), TEXT(LOCTEXT_NAMESPACE), TEXT(InKey))

UCLASS()
class SIN_API AInteractable : public AActor, public II_Interactable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UMaterialParameterCollection* OutlineCollection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray <TSubclassOf<UCameraModifier>> CameraMod;

	UPROPERTY(EditAnywhere)
		FString Label = "Interactable";

	UPROPERTY(EditAnywhere)
		FString InteractionPrompt = "Press 'E' to Interact";

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Focus(ASinPlayerController* Controller);
		virtual void Focus_Implementation(ASinPlayerController* Controller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		UInventory* GetInventory();
		virtual UInventory* GetInventory_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		UEquipment* GetEquipment();
		virtual UEquipment* GetEquipment_Implementation();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FName GetOutlineColour();
		virtual FName GetOutlineColour_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void EndFocus(ASinPlayerController* Controller);
		virtual void EndFocus_Implementation(ASinPlayerController* Controller);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Interact(ASinPlayerController* Caller);
		virtual void Interact_Implementation(ASinPlayerController* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FString GetToolTipInfo();
		virtual FString GetToolTipInfo_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FString GetInteractableLabel();
		virtual FString GetInteractableLabel_Implementation();
protected:
	UMaterialParameterCollectionInstance* CollectionInstance;
	UPROPERTY(Transient)
	UCameraModifier* AppliedCamMod;
};

#undef LOCTEXT_NAMESPACE
