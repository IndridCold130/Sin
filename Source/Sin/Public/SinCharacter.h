// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Misc/SinGPTs.h"
#include "GAS/SinASC.h"
#include "Interfaces/I_Interactable.h"
#include "Interfaces/I_CombatInterface.h"
#include "Interfaces/SinWeaponInterface.h"
#include "AbilitySystemInterface.h"
#include "SinCharacter.generated.h"

class ASinBaseWeapon;
class USinAttributes;
struct FHitResult;
class USinAttributeHealth;
class USinInputConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSignalTargetHitSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSignalSubstateChangedSignature, ASinCharacter*, Character, FGameplayTag, Substate, bool, Entered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSignalStateChangedSignature, ASinCharacter*, Character, FGameplayTag, NewState, FGameplayTag, DeltaState);

UCLASS()
class SIN_API ASinCharacter : public ACharacter, public II_Interactable, public II_CombatInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASinCharacter();
	//**GAS */
	UPROPERTY(Category = "GAS", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<USinASC>AbilitySystemComponent;

protected:
	virtual class USinASC* GetAbilitySystemComponent() const override;
	/** Name of the CharacterMovement component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	//FName SinASCName;
public:

	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Identity")
		FString SinCharacterName = "Jane Doe";

	UPROPERTY(SaveGame, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Identity")
		FGameplayTagContainer IdentityHolder;

	UFUNCTION(BlueprintPure, Category = "Identity")
		bool HasIdentityMarker(FGameplayTag State) const;

	UFUNCTION(BlueprintPure, Category = "Identity")
		bool IsFemale();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		TObjectPtr<ASinBaseWeapon> MainHandWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		TObjectPtr<ASinBaseWeapon> OffHandWeapon;

	UFUNCTION(BlueprintPure, Category = "States")
		bool IsInState(FGameplayTag State);

	UFUNCTION(BlueprintPure, Category = "States")
		bool CanEnterState(FGameplayTag State);

	UFUNCTION(BlueprintCallable, Category = "States")
		bool EnterState(FGameplayTag State);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool AdjustMovementSpeedMult(EAttributeSource Source, bool Apply, float Amount);
		virtual bool AdjustMovementSpeedMult_Implementation(EAttributeSource Source, bool Apply, float Amount);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
		float MovementSpeedMult = 1.0f;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void GetCharacterIdentity(FGameplayTagContainer& Identity);
		virtual void GetCharacterIdentity_Implementation(FGameplayTagContainer& Identity);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Identity")
		void AddIdentityTag(FGameplayTag Tag, bool Add);
		virtual void AddIdentityTag_Implementation(FGameplayTag Tag, bool Add);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		FRotator GetCharacterLookRotation(FRotator Rotation);
		virtual FRotator GetCharacterLookRotation_Implementation(FRotator Rotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		FRotator SinLookAtRotation;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "States")
		bool EnterSubstateCall(FGameplayTag State);
		virtual bool EnterSubstateCall_Implementation(FGameplayTag State);

	UFUNCTION(BlueprintPure, Category = "States")
		bool IsInSubstate(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "States")
		bool IsInSubstateCall(FGameplayTag State);
		virtual bool IsInSubstateCall_Implementation(FGameplayTag State);

	UFUNCTION(BlueprintPure)
		bool CanEnterSubstate(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		bool I_CanEnterSubstate(FGameplayTag State);
		virtual bool I_CanEnterSubstate_Implementation(FGameplayTag State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		void HandleDeath(AController* Killer, AActor* Tool);
		virtual void HandleDeath_Implementation(AController* Killer, AActor* Tool);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
		ASinCharacter* GetCurrentTarget();
		virtual ASinCharacter* GetCurrentTarget_Implementation();
	
	UFUNCTION(BlueprintCallable, Category = "Subtates")
		bool EnterSubstate(FGameplayTag State);

	UFUNCTION(BlueprintCallable, Category = "Subtates")
		bool CanLeaveSubstate(FGameplayTag State);

	UFUNCTION(BlueprintCallable)
		bool LeaveSubstate(FGameplayTag State);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void GrantExperience(float EXP);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "State")
		void StateChangedPRC(FGameplayTag State, bool FromClient, bool Entered);

	UFUNCTION(BlueprintPure, Category = "Combat")
		bool HarmFilter(ASinCharacter* Attacker);

	UFUNCTION(BlueprintCallable, Category = "Default")
		void EnableRagdoll(bool Enable);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		float GetBackpackWeight();
		UFUNCTION(BlueprintPure, Category = "Inventory")
		float GetEquipLoad();

	UPROPERTY(EditDefaultsOnly, Category = "Substate")
		TMap<FGameplayTag, FGameplayTagContainer> SubstateEnterRules;

	UPROPERTY(EditDefaultsOnly, Category = "Substate")
		TMap<FGameplayTag, FGameplayTagContainer> SubstateExitRules;

	UPROPERTY(EditDefaultsOnly, Category = "Substate")
		TMap<FGameplayTag, FGameplayTagContainer> SubstateExitRulesDeux;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Substate")
		FSignalSubstateChangedSignature OnSignalSubstateChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "State")
		FSignalStateChangedSignature OnSignalStateChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "State")
		FSignalTargetHitSignature OnSignalHitTarget;

	UFUNCTION()
		void OnRep_Substates();

	UFUNCTION()
		void OnRep_MainState(FGameplayTag& Delta);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		void GetEquippedWeapon(bool OffHand, bool& bFound, UGameItemBase*& Weapon);
		UFUNCTION(BlueprintPure, Category = "Inventory")
		UInventory* GetInventoryByTag(FGameplayTag Tag);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(ReplicatedUsing = OnRep_Substates);
	FGameplayTagContainer ActiveSubstates;

	UPROPERTY(ReplicatedUsing = OnRep_MainState);
	FGameplayTag MainState = TAG_State_Passive;

	/** Default locomotion anim layer.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TSubclassOf<UAnimInstance> DefaultLocoAnimClass;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		//TMap<EActionType, FAnimMontageArray> AttackMap;
	float LastDamageAmount = 0.f;
	protected:
		UFUNCTION(BlueprintImplementableEvent)
		void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ASinCharacter* InstigatorCharacter, AActor* DamageCauser);
		UFUNCTION(BlueprintImplementableEvent)
		void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
		// Called from RPGAttributeSet, these call BP events above
		virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ASinCharacter* InstigatorCharacter, AActor* DamageCauser);
		virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

		// Friended to allow access to handle functions above
		friend USinAttributeHealth;


		// TEMPORARILY PUTTING INPUT CONTROLS IN HERE
		void Input_AbilityInputTagPressed(FGameplayTag InputTag);
		void Input_AbilityInputTagReleased(FGameplayTag InputTag);

};
