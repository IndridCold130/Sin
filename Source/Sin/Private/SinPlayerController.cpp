// Fill out your copyright notice in the Description page of Project Settings.


#include "SinPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/I_Interactable.h"
#include "SinCharacter.h"
#include "Misc/SinEnhancedInput.h"
#include "PlayerMappableInputConfig.h"
#include "EnhancedInputComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GameFramework/InputSettings.h"
//#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/SinASC.h"

void ASinPlayerController::BeginPlay()
{
	IA_Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	RegisterInputSettings();
	OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnSinPawnReceived);
	Super::BeginPlay();
	if (GetPawn())
	{
		OnSinPawnReceived(nullptr, Cast<ASinCharacter>(GetPawn()));
	}
	TraceChannel = ECC_GameTraceChannel4;
	//if (GetNetMode() != NM_DedicatedServer)
	if (IsLocalController())
	{
		GetWorld()->GetTimerManager().SetTimer(InterTimer, this, &ASinPlayerController::CalculateTraceDistance, .1f, true);
	}
	ASinCharacter* PlayerPawn = Cast<ASinCharacter>(GetPawn());
	if (PlayerPawn)
	{
		PlayerPawn->OnSignalSubstateChanged.AddDynamic(this, &ASinPlayerController::OnSubstateChangedDelegate);
	}
	//InputComponent->BindAction(EKeys::AnyKey, IE_Pressed, this, &ASinPlayerController::ProcessPlayerInput);
	InputComponent->BindAction("AnyKey", IE_Pressed, this, &ASinPlayerController::AnyKey);
	// Bind axis mappings
	InputComponent->BindAxis("MouseX", this, &ASinPlayerController::MoveForward);
	InputComponent->BindAxis("MouseY", this, &ASinPlayerController::MoveRight);
}

void ASinPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if (GetNetMode() != NM_DedicatedServer)
	if (IsLocalController())
	{
	}
}

void ASinPlayerController::TransferLevelsClient(FString LevelID)
{
	ClientTravel(LevelID, ETravelType::TRAVEL_Relative);
}

void ASinPlayerController::TakeOwnership_Implementation(AActor* Object)
{
	Object->SetOwner(this);
}

bool ASinPlayerController::TakeOwnership_Validate(AActor* Object) { return true; }

void ASinPlayerController::MoveItem_Implementation(UInventory* Inventory, UGameItemBase* Item, int32 DstIndex)
{
	if (Inventory)
	{
		Inventory->MoveItemDeux(Item, DstIndex);
	}
}

bool ASinPlayerController::MoveItem_Validate(UInventory* Inventory, UGameItemBase* Item, int32 DstIndex) { return true; }

void ASinPlayerController::SetStackPRC_Implementation(UGameItemBase* Item, int32 NewAmount)
{
	Item->SetStack(NewAmount);
}

bool ASinPlayerController::SetStackPRC_Validate(UGameItemBase* Item, int32 NewAmount) { return true; }

void ASinPlayerController::TryAddItemPRC_Implementation(UInventory* Inventory, UGameItemBase* Item, int32 TargetIndex, int32 SrcIndex)
{
	bool Success;
	Inventory->TryAddItem(Item, TargetIndex, Success, SrcIndex);
}

bool ASinPlayerController::TryAddItemPRC_Validate(UInventory* Inventory, UGameItemBase* Item, int32 TargetIndex, int32 SrcIndex) { return true; }

ASinPlayerController::FPlayerTraceResult ASinPlayerController::PerformTrace(float Distance)
{

	FPlayerTraceResult TraceResult;
	TArray<FHitResult> MultiHits;
	FVector WorldDirection;
	TArray<UPrimitiveComponent*> PrimitiveChildren;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SinPlayerControllerTrace));
	Params.AddIgnoredComponents(PrimitiveChildren);

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	FVector2D ViewportSize;
	if(!LocalPlayer)
	{
		return TraceResult;
	}
	LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
	FVector WorldOrigin;
	if (UGameplayStatics::DeprojectScreenToWorld(this, ViewportSize * 0.5f, WorldOrigin, WorldDirection) == true)
	{
		TraceResult.LineStartLocation = WorldOrigin;
		TraceResult.LineEndLocation = WorldOrigin + WorldDirection * InteractionDistance;

		bool bTraceSuccessful = GetWorld()->LineTraceMultiByChannel(MultiHits, WorldOrigin, WorldOrigin + WorldDirection * Distance, TraceChannel, Params);
		if (bTraceSuccessful)
		{
			const FHitResult& HitResult = MultiHits[0];
			AActor* Interactable = HitResult.GetActor();
			if (Interactable != FocusedActor)
			{
				if (FocusedActor)
				{
					II_Interactable* Interface = Cast<II_Interactable>(FocusedActor);
					if (Interface)
					{
						Interface->Execute_EndFocus(FocusedActor, this);
					}
				}
				II_Interactable* Interface = Cast<II_Interactable>(Interactable);
				if (Interface)
				{
					Interface->Execute_Focus(Interactable, this);
				}
				FocusedActor = Interactable;
			}
			//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint,
				//30.0, 10, FColor::Red, false, 2.5);
		}
		else
		{
			if(FocusedActor)
			{
				II_Interactable* Interface = Cast<II_Interactable>(FocusedActor);
				if (Interface)
				{
					Interface->Execute_EndFocus(FocusedActor, this);
				}
			}
			FocusedActor = nullptr;
		}
	}
	return TraceResult;
}

void ASinPlayerController::CalculateTraceDistance()
{
	float FinalLength = InteractionDistance;
	APawn* PlayerPawn = GetPawn();
	if (PlayerCameraManager && PlayerPawn)
	{
		float ExtraDistance = FVector::Dist(PlayerCameraManager->GetCameraLocation(), PlayerPawn->GetActorLocation());
		FinalLength = InteractionDistance + ExtraDistance;
	}
	PerformTrace(FinalLength);
}

void ASinPlayerController::SetOutlineColour_Implementation(FLinearColor OutlineColor)
{
}

void ASinPlayerController::RegisterInputSettings()
{
	if (!IA_Subsystem || MappingsToRegister.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Local Player Subsystem not found or no mappings to register!"));
		return;
	}
	// Option 1: Explicitly add to the settings
	if (UEnhancedInputUserSettings* UserSettings = IA_Subsystem->GetUserSettings())
	{
		for (UInputMappingContext* MappingContext : MappingsToRegister)
		{
			if (MappingContext)
			{
				UserSettings->RegisterInputMappingContext(MappingContext);
			}
		}
	}
}

void ASinPlayerController::OnSubstateChangedDelegate(ASinCharacter* SinCharacter, FGameplayTag Substate, bool Entered)
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (Substate == TAG_Substate_Attack)
		{
			PauseTracing(Entered);
		}
	}
}

void ASinPlayerController::OnSinPawnReceived(APawn* PreviousPawn, APawn* NewPawn)
{
	if (NewPawn)
	{
		AbilitySystem = Cast<USinASC>(NewPawn->GetComponentByClass(USinASC::StaticClass()));
		if (IsLocalController())
		{
			NewPawnPossessed(Cast<ASinCharacter>(PreviousPawn), Cast<ASinCharacter>(NewPawn));
			if (AbilitySystem)
			{
				USinEnhancedInput* SinEnhancedInput = Cast<USinEnhancedInput>(NewPawn->GetComponentByClass(USinEnhancedInput::StaticClass()));
				UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
				TArray<uint32> BindHandles;
				SinEnhancedInput->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
				// NATIVE INPUTS BOII
				SinEnhancedInput->BindNativeAction(InputConfig, TAG_Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
				SinEnhancedInput->BindNativeAction(InputConfig, TAG_Input_Action, ETriggerEvent::Started, this, &ThisClass::Input_Action); //*bLogIfNotFound=*/ false
				SinEnhancedInput->BindNativeAction(InputConfig, TAG_Input_ActionSecondary, ETriggerEvent::Started, this, &ThisClass::Input_ActionSecondary);
				SinEnhancedInput->BindNativeAction(InputConfig, TAG_Input_ActionSpecial, ETriggerEvent::Started, this, &ThisClass::Input_ActionSpecial);
				SinEnhancedInput->BindNativeAction(InputConfig, TAG_Input_SwitchMainHand, ETriggerEvent::Started, this, &ThisClass::Input_SwapMainHand);
				SinEnhancedInput->BindNativeAction(InputConfig, TAG_Input_SwitchOffHand, ETriggerEvent::Started, this, &ThisClass::Input_SwapOffHand);
			}
		}
		else if (HasAuthority())
		{
			NewPawnPossessed(Cast<ASinCharacter>(PreviousPawn), Cast<ASinCharacter>(NewPawn));
		}
	}
}

// GAS RELATED INPUT STUFF //
void ASinPlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	AbilitySystem->AbilityInputTagPressed(InputTag);
}

void ASinPlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	AbilitySystem->AbilityInputTagReleased(InputTag);
}

void ASinPlayerController::PauseTracing(bool Pause)
{
	if (Pause)
	{
		GetWorld()->GetTimerManager().PauseTimer(InterTimer);
		if (FocusedActor!=nullptr)
		{
			II_Interactable* Interface = Cast<II_Interactable>(FocusedActor);
			if (Interface)
			{
				Interface->Execute_EndFocus(FocusedActor, this);
			}
			FocusedActor = nullptr;
		}
		return;
	}
	GetWorld()->GetTimerManager().UnPauseTimer(InterTimer);
}

void ASinPlayerController::SinInputMode_Implementation(UUserWidget* Focus, EInputMode InputMode)
{
	if (InputMode == CurrentInputMode) { return; }
	FInputModeUIOnly LocalInputMode;
	FInputModeGameAndUI LocalGameAndUI;
	switch (InputMode)
	{
	case EInputMode::VE_UIOnly:
		if (Focus)
		{
			LocalInputMode.SetWidgetToFocus(Focus->TakeWidget());
		}
		SetInputMode(LocalInputMode);
		bShowMouseCursor = true;
		PauseTracing(true);
		break;
	case EInputMode::VE_GameOnly:
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
		PauseTracing(false);
		break;
	case EInputMode::VE_GameAndUI:
		LocalGameAndUI.SetHideCursorDuringCapture(true);
		if (Focus)
		{
			LocalGameAndUI.SetWidgetToFocus(Focus->TakeWidget());
		}
		SetInputMode(LocalGameAndUI);
		bShowMouseCursor = true;
		PauseTracing(true);
		break;
	default:
		break;
	}
	CurrentInputMode = InputMode;
}

void ASinPlayerController::NewPawnPossessed_Implementation(const ASinCharacter* PreviousPawn, const ASinCharacter* NewPawn)
{
}

void ASinPlayerController::LoadSinMenu_Implementation(FGameplayTag SubmenuTag, AActor* Interactable, bool bAsync)
{
	if (MainHUD) { MainHUD->LoadSinMenu_Implementation(this, SubmenuTag, Interactable, bAsync); }
}

void ASinPlayerController::AnyKey(FKey Key)
{
	bool bLocalGamePad = Key.IsGamepadKey();
	if (bLocalGamePad != bIsUsingGamepad)
	{
		bIsUsingGamepad = bLocalGamePad;
		OnInputModeChangedDelegate.Broadcast(bLocalGamePad);
	}
}

void ASinPlayerController::MoveForward(float Value)
{
	if (Value != 0.0f&& bIsUsingGamepad==true) {
		bIsUsingGamepad = false;
		OnInputModeChangedDelegate.Broadcast(false);
	}
}

void ASinPlayerController::MoveRight(float Value)
{
	if (Value != 0.0f && bIsUsingGamepad == true) {
		bIsUsingGamepad = false;
		OnInputModeChangedDelegate.Broadcast(false);
	}
}

void ASinPlayerController::Input_Move(const FInputActionInstance& InputActionValue)
{
	FInputActionValue Vectors = InputActionValue.GetValue();
	const FVector2D Value = Vectors.Get<FVector2D>();
	float ElapsedTime = FMath::Clamp(1.f, 0.5f, InputActionValue.GetElapsedTime()*1.8f);
	const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		GetPawn()->AddMovementInput(MovementDirection, Value.X* ElapsedTime);
	}
	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		GetPawn()->AddMovementInput(MovementDirection, Value.Y* ElapsedTime);
	}
}

void ASinPlayerController::Input_Crouch(const FInputActionInstance& InputActionValue)
{
	FGameplayEventData Payload;
	Payload.EventTag = TAG_Input_Crouch;
	FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
	AbilitySystem->HandleGameplayEvent(TAG_Input_Crouch, &Payload);
}

void ASinPlayerController::Input_Action(const FInputActionValue& InputActionValue)
{
		FGameplayEventData Payload;
		Payload.EventTag = TAG_Input_Action;
		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		AbilitySystem->HandleGameplayEvent(TAG_Input_Action, &Payload);
}

void ASinPlayerController::Input_ActionSecondary(const FInputActionValue& InputActionValue)
{
		FGameplayEventData Payload;
		Payload.EventTag = TAG_Input_Action_Secondary;
		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		AbilitySystem->HandleGameplayEvent(TAG_Input_Action_Secondary, &Payload);
}

void ASinPlayerController::Input_ActionSpecial(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Payload;
	Payload.EventTag = TAG_Input_Action_Special;
	FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
	AbilitySystem->HandleGameplayEvent(TAG_Input_Action_Special, &Payload);
}

void ASinPlayerController::Input_SwapMainHand(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Payload;
	Payload.EventTag = TAG_Input_DrawWeapon;
	Payload.InstigatorTags.AddTag(TAG_Input_SwitchMainHand);
	FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
	AbilitySystem->HandleGameplayEvent(TAG_Input_SwitchMainHand, &Payload);
}

void ASinPlayerController::Input_SwapOffHand(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Payload;
	Payload.EventTag = TAG_Input_SwitchOffHand;
	FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
	AbilitySystem->HandleGameplayEvent(TAG_Input_SwitchOffHand, &Payload);
}

//void ASinPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ASinPlayerController, SinControlledPawn, COND_OwnerOnly);
//}
