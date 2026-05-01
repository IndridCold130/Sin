// Fill out your copyright notice in the Description page of Project Settings.


#include "SinCharacter.h"

#include "Actors/SinBaseWeapon.h"
#include "Inventory/SinBelt.h"
#include "GAS/SinASC.h"
#include "Misc/SinEnhancedInput.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ASinCharacter::ASinCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<USinASC>(TEXT("AbilitySystem"));
}

USinASC* ASinCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ASinCharacter::HasIdentityMarker(FGameplayTag Marker) const
{
	return IdentityHolder.HasTagExact(Marker);
}

bool ASinCharacter::IsFemale()
{
	return IdentityHolder.HasTagExact(TAG_Gender_Female);
}

bool ASinCharacter::IsInState(FGameplayTag State)
{
	return MainState == State;
}

bool ASinCharacter::CanEnterState(FGameplayTag State)
{
	return true;
}

bool ASinCharacter::EnterState(FGameplayTag State)
{
	if (CanEnterState(State))
	{
		FGameplayTag PreviousState = MainState;
		MainState = State;
		OnSignalStateChanged.Broadcast(this, State, PreviousState);
		return true;
	}
	return false;
}

bool ASinCharacter::AdjustMovementSpeedMult_Implementation(EAttributeSource Source, bool Apply, float Amount)
{
	if (Amount < 0.001f)
	{
		return false;
	}
	float Value = FMath::Abs(Amount);
	Apply ? MovementSpeedMult = FMath::Clamp(MovementSpeedMult *= Value, 0.0f, MovementSpeedMult *= Value) : MovementSpeedMult = FMath::Clamp(MovementSpeedMult /= Value, 0.0f, MovementSpeedMult /= Value);
	return true;
}

void ASinCharacter::GetCharacterIdentity_Implementation(FGameplayTagContainer& Identity)
{
	Identity = IdentityHolder;
}

void ASinCharacter::AddIdentityTag_Implementation(FGameplayTag Tag, bool Add)
{
	if (Add)
	{
		IdentityHolder.AddTag(Tag);
		return;
	}
	IdentityHolder.RemoveTag(Tag);
}

FRotator ASinCharacter::GetCharacterLookRotation_Implementation(FRotator)
{
	return FRotator();
}

bool ASinCharacter::IsInSubstate(FGameplayTag State)
{
	return ActiveSubstates.HasTagExact(State);
}

bool ASinCharacter::IsInSubstateCall_Implementation(FGameplayTag State)
{
	return IsInSubstate(State);
}

bool ASinCharacter::CanEnterSubstate(FGameplayTag State)
{
	if (SubstateEnterRules.Contains(State))
		{
		for (auto LocTag = SubstateEnterRules[State].CreateConstIterator(); LocTag; ++LocTag)
			{
				if (ActiveSubstates.HasTagExact(*LocTag))
					{
						return false;
					}
				}
		}
		return true;
}

bool ASinCharacter::I_CanEnterSubstate_Implementation(FGameplayTag State)
{
	return CanEnterSubstate(State);
}

bool ASinCharacter::EnterSubstate(FGameplayTag Substate)
{
	if (CanEnterSubstate(Substate))
	{
		ActiveSubstates.AddTag(Substate);
		OnSignalSubstateChanged.Broadcast(this, Substate, true);
		if (SubstateExitRules.Contains(Substate))
		{
			for (auto LocTag = SubstateExitRules[Substate].CreateConstIterator(); LocTag; ++LocTag)
			{
				LeaveSubstate(*LocTag);
			}
		}
		return true;
	}
	return false;
}

bool ASinCharacter::EnterSubstateCall_Implementation(FGameplayTag State)
{
	return EnterSubstate(State);
}

void ASinCharacter::HandleDeath_Implementation(AController* Killer, AActor* Tool)
{

}

ASinCharacter* ASinCharacter::GetCurrentTarget_Implementation()
{
	return nullptr;
}

bool ASinCharacter::CanLeaveSubstate(FGameplayTag State)
{
	return false;
}

bool ASinCharacter::LeaveSubstate(FGameplayTag Substate)
{
	if (!IsInSubstate(Substate))
	{
		return false;
	}
	ActiveSubstates.RemoveTag(Substate);
	if (SubstateExitRulesDeux.Find(Substate))
	{
		for (auto LocTag = SubstateExitRulesDeux[Substate].CreateConstIterator(); LocTag; ++LocTag)
		{
			LeaveSubstate(*LocTag);
		}
	}
	OnSignalSubstateChanged.Broadcast(this, Substate, false);
	return true;
}

void ASinCharacter::GrantExperience_Implementation(float EXP)
{
}

bool ASinCharacter::GrantExperience_Validate(float Damage) { return true; }

void ASinCharacter::StateChangedPRC_Implementation(FGameplayTag State, bool FromClient, bool Entered)
{
	if (Entered)
	{
		ActiveSubstates.AddTagFast(State);
	}
	else
	{
		ActiveSubstates.RemoveTag(State);
	}
	OnSignalSubstateChanged.Broadcast(this, State, Entered);
	return;
}

bool ASinCharacter::StateChangedPRC_Validate(FGameplayTag State, bool FromClient, bool Entered) { return true; }


bool ASinCharacter::HarmFilter(ASinCharacter* Victim)
{
	if (!Victim->GetController())
	{
		return true;
	}
	bool bVictimIsPlayer = Victim->GetController()->IsPlayerController(); //GetController()->GetClass()->IsChildOf(APlayerController::StaticClass());
	if(bVictimIsPlayer)
	{
		return true;
	}
	else
	{
		if (GetController()->GetClass()->IsChildOf(APlayerController::StaticClass()))
		{
			return true;
		}
	}
	return false;
}

void ASinCharacter::EnableRagdoll(bool Enable)
{
	if (Enable)
	{
		GetMesh()->SetCollisionProfileName(FName("SinRagdoll"));
		GetMesh()->SetSimulatePhysics(true); // Enable physics simulation
		//GetMesh()->SetAllBodiesSimulatePhysics(true); // Enable for all bodies in the skeletal mesh
		//GetMesh()->WakeAllRigidBodies(); // Wake the rigid bodies
	}
	else {
		GetMesh()->SetCollisionProfileName(FName("SinMesh"));
		GetMesh()->SetSimulatePhysics(false);
		//GetMesh()->SetAllBodiesSimulatePhysics(false);
		//GetMesh()->PutAllRigidBodiesToSleep(); // Put the rigid bodies to sleep
	}
}

float ASinCharacter::GetBackpackWeight()
{
	float localWeight = 0.0f;
	TArray<UInventory*> Inventories;
	GetComponents<UInventory>(Inventories);
	if (Inventories.IsEmpty())
	{
		return localWeight;
	}
	for (UInventory* LocalInventory : Inventories)
	{
		FGameplayTagContainer ValidInventoryTags{ TAG_Item };
		if (LocalInventory->InventoryType.MatchesAnyExact(ValidInventoryTags))
		{
			localWeight += LocalInventory->GetInventoryAbsoluteWeight(); // Sum up item weights
		}
	}
	return localWeight;
}

float ASinCharacter::GetEquipLoad()
{
	return 0.0f;
}

// Called when the game starts or when spawned
void ASinCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (DefaultLocoAnimClass)
		{
			SkeletalMeshComp->LinkAnimClassLayers(DefaultLocoAnimClass);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DefaultLocoAnimClass is not set on %s."), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SkeletalMeshComponent is missing on %s."), *GetName());
	}
}

void ASinCharacter::OnRep_Substates()
{
}

void ASinCharacter::OnRep_MainState(FGameplayTag& Delta)
{
	OnSignalStateChanged.Broadcast(this, MainState, Delta);
}

void ASinCharacter::GetEquippedWeapon(bool OffHand, bool& bFound, UGameItemBase*& Weapon)
{
	USinBelt* Belt = FindComponentByTag<USinBelt>(OffHand ? FName("OffHand"):FName("MainHand"));
	if (!Belt)
	{
		bFound = false;
		return;
	}
	if (!Belt->Container.IsValidIndex(Belt->ActiveItemIndex))
	{
		bFound = false;
		return;
	}
	UGameItemBase* Copy= Belt->Container[Belt->ActiveItemIndex];
	if (!Copy)
	{
		bFound = false;
		return;
	}
	bFound = true;
	Weapon = Copy;
}

UInventory* ASinCharacter::GetInventoryByTag(FGameplayTag Tag)
{
	TArray<UInventory*> Inventories;
	GetComponents<UInventory>(Inventories);
	if (Inventories.IsEmpty())
	{
		return nullptr;
	}
	for (UInventory* LocalInventory : Inventories)
	{
		// InventoryData.Add(LocalInventory->InventoryType, LocalInventory);
		if (LocalInventory->InventoryType == Tag)
		{
			return LocalInventory;
		}
	}
	return nullptr;
}

// Called every frame
void ASinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASinCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);
}

void ASinCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, ASinCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	LastDamageAmount = DamageAmount;
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ASinCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	OnHealthChanged(DeltaValue, EventTags);
}

void ASinCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void ASinCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

void ASinCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASinCharacter, MainState, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ASinCharacter, ActiveSubstates, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ASinCharacter, IdentityHolder, COND_InitialOnly);
}

