// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Actors/Interactable.h"
#include "Interfaces/SinPlayerInterface.h"
#include "Widgets/SinHUD_Widget.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "SinPlayerController.generated.h"

class ASinCharacter;
class USinInputConfig;
class USinASC;
class USinInventoryRPC;
struct FInputActionValue;
struct FInputActionInstance;

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPossessedOnClientSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputModeSignature, bool, bGamepadMode);
UCLASS()
class SIN_API ASinPlayerController : public APlayerController, public ISinPlayerInterface
{
	GENERATED_BODY()

public:

	ASinPlayerController();
	
	UFUNCTION(BlueprintPure, Category="Inventory")
	USinInventoryRPC* GetInventoryRPC() const { return InventoryRPC; }
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> MappingsToRegister;

	UPROPERTY(EditDefaultsOnly, Category = "InputConfig")
	TObjectPtr<USinInputConfig> InputConfig;

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<USinASC> AbilitySystem;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Possession")
		FPossessedOnClientSignature OnClientPossessedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
		FInputModeSignature OnInputModeChangedDelegate;

	UPROPERTY(BlueprintReadWrite)
		TObjectPtr <ASinCharacter> LockedOnActor;

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<ASinCharacter> SinControlledPawn;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		TEnumAsByte<ECollisionChannel> TraceChannel;
	
		//UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Level")
	UFUNCTION(BlueprintCallable, Category = "Level")
		void TransferLevelsClient(FString LevelID);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "NetOwnership")
		void TakeOwnership(AActor* Object);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "InventoryFunctions")
		void MoveItem(UInventory* Inventory, UGameItemBase* Item, int32 DstIndex);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "InventoryFunctions")
		void SetStackPRC(UGameItemBase* Item, int32 NewAmount);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "InventoryFunctions")
		void TryAddItemPRC(UInventory* Inventory, UGameItemBase* Item, int32 TargetIndex, int32 SrcIndex = -1);

protected:

	UFUNCTION()
		void OnSubstateChangedDelegate(ASinCharacter* SinCharacter, FGameplayTag Substate, bool Entered);
	UFUNCTION()
		void OnSinPawnReceived(APawn* PreviousPawn, APawn* NewPawn);
	// TEMPORARILY PUTTING INPUT CONTROLS IN HERE
		//virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
		UPROPERTY()
		TObjectPtr<UEnhancedInputLocalPlayerSubsystem>IA_Subsystem;
		void RegisterInputSettings();
		void Input_AbilityInputTagPressed(FGameplayTag InputTag);
		void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	struct FPlayerTraceResult
	{
		FPlayerTraceResult()
			: HitResult()
			, LocalHitLocation(FVector2D::ZeroVector)
			, bWasHit(false)
			, LineStartLocation(FVector::ZeroVector)
			, LineEndLocation(FVector::ZeroVector)
		{
		}
		FHitResult HitResult;
		FVector2D LocalHitLocation;
		bool bWasHit;
		FVector LineStartLocation;
		FVector LineEndLocation;

	};

	virtual FPlayerTraceResult PerformTrace(float Distance);

	FTimerHandle TraceTimer;

	UFUNCTION()
		void CalculateTraceDistance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 400;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
		AActor* FocusedActor;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//TSubclassOf<USinHUD> HUD_Subclass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
		TObjectPtr<USinInventoryRPC> InventoryRPC;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void SetOutlineColour(FLinearColor OutlineColor);
	virtual void SetOutlineColour_Implementation(FLinearColor OutlineColor);

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<USinHUD> MainHUD;

	//virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
		void PauseTracing(bool Pause);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
		void SinInputMode(UUserWidget* Focus, EInputMode InputMode);
		virtual void SinInputMode_Implementation(UUserWidget* Focus, EInputMode InputMode);
	
	UPROPERTY(BlueprintReadOnly)
		EInputMode CurrentInputMode;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
		void NewPawnPossessed(const ASinCharacter* PreviousPawn, const ASinCharacter* NewPawn);
		virtual void NewPawnPossessed_Implementation(const ASinCharacter* PreviousPawn, const ASinCharacter* NewPawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
		void LoadSinMenu(FGameplayTag SubmenuTag, AActor* Interactable, bool bAsync);
		virtual void LoadSinMenu_Implementation(FGameplayTag SubmenuTag, AActor* Interactable, bool bAsync = false);
	
	UFUNCTION(BlueprintPure, Category="Sin|Menu")
		bool IsSinMenuLoaded(FGameplayTag MenuTag, bool bRequireActive = true) const;

private:
	bool bIsUsingGamepad = false; // Tracks the current input device
	void AnyKey(FKey Key);
	// Movement functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	FTimerHandle InterTimer;
	void Input_Move(const FInputActionInstance& InputActionValue);
	void Input_Crouch(const FInputActionInstance& InputActionValue);
	void Input_Action(const FInputActionValue& InputActionValue);
	void Input_ActionSecondary(const FInputActionValue& InputActionValue);
	void Input_ActionSpecial(const FInputActionValue& InputActionValue);
	void Input_SwapMainHand(const FInputActionValue& InputActionValue);
	void Input_SwapOffHand(const FInputActionValue& InputActionValue);
	void Input_QuickSlotUse(const FInputActionInstance& InputActionValue);
	void Input_QuickSlotCycle(const FInputActionInstance& InputActionValue);
};
