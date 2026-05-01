// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/SinPhysicalDamageCalculation.h"
#include "SinCharacter.h"
#include "GAS/SinASC.h"
#include "GAS/SinAttributePrimary.h"
#include "GAS/SinAttributeSecondary.h"
#include "GAS/SinAttributeStamina.h"
#include "GAS/Calculations/SinCombatInstance.h"
#include "Misc/SinGPTs.h"
#include "GameplayAbilitySpec.h"

struct PhysicalDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength); DECLARE_ATTRIBUTE_CAPTUREDEF(Dexterity); DECLARE_ATTRIBUTE_CAPTUREDEF(Constitution); 
	DECLARE_ATTRIBUTE_CAPTUREDEF(Sorcery); DECLARE_ATTRIBUTE_CAPTUREDEF(Faith); DECLARE_ATTRIBUTE_CAPTUREDEF(Charisma);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage); DECLARE_ATTRIBUTE_CAPTUREDEF(Health); DECLARE_ATTRIBUTE_CAPTUREDEF(Stamina);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Toughness);
	PhysicalDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributePrimary, Strength, Source, true); DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributePrimary, Dexterity, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributePrimary, Constitution, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributePrimary, Sorcery, Source, true); DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributePrimary, Faith, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributePrimary, Charisma, Source, true); DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeHealth, Damage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeHealth, Health, Target, false); DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeStamina, Stamina, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USinAttributeSecondary, Toughness, Target, false);
	}
};

static const PhysicalDamageStatics& DStatics()
{
	static PhysicalDamageStatics DStatics;
	return DStatics;
}

USinPhysicalDamageCalculation::USinPhysicalDamageCalculation()
{
	RelevantAttributesToCapture.Add(DStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DStatics().DexterityDef);
	RelevantAttributesToCapture.Add(DStatics().ConstitutionDef);
	RelevantAttributesToCapture.Add(DStatics().SorceryDef);
	RelevantAttributesToCapture.Add(DStatics().FaithDef);
	RelevantAttributesToCapture.Add(DStatics().CharismaDef);
	RelevantAttributesToCapture.Add(DStatics().DamageDef);
	RelevantAttributesToCapture.Add(DStatics().HealthDef);
	RelevantAttributesToCapture.Add(DStatics().StaminaDef);
	RelevantAttributesToCapture.Add(DStatics().ToughnessDef);
}

void USinPhysicalDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// BOILER //
	USinASC* SourceASC = Cast<USinASC>(ExecutionParams.GetSourceAbilitySystemComponent()); if (!IsValid(SourceASC)) return;
	USinASC* TargetASC = Cast<USinASC>(ExecutionParams.GetTargetAbilitySystemComponent()); if (!IsValid(TargetASC)) return;
	FSinCombatInstance DamageReport;
	const FGameplayEffectSpec& DamageSpec = SourceASC->GetGameEffectSpecBasedOnTag(OffHand ? TAG_Item_Equipment_Weapon_OffHand : TAG_Item_Equipment_Weapon_MainHand);
	const FGameplayEffectSpec& DefenseSpec = TargetASC->GetGameEffectSpecBasedOnTag(TAG_Gear);
	const FGameplayEffectSpec& ShieldSpec = TargetASC->GetGameEffectSpecBasedOnTag(TAG_Item_Equipment_Weapon_OffHand);
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	// GET FULL ATTRIBUTES
	float TotalDamageMod = 1.f;
	FAggregatorEvaluateParameters EvalParams; TMap<EPrimaryAttribute, float> AttributeValues;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DStatics().StrengthDef, EvalParams, AttributeValues.Add(EPrimaryAttribute::Strength, 0.f));
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DStatics().DexterityDef, EvalParams, AttributeValues.Add(EPrimaryAttribute::Dexterity, 0.f));
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DStatics().ConstitutionDef, EvalParams, AttributeValues.Add(EPrimaryAttribute::Constitution, 0.f));
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DStatics().SorceryDef, EvalParams, AttributeValues.Add(EPrimaryAttribute::Sorcery, 0.f));
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DStatics().FaithDef, EvalParams, AttributeValues.Add(EPrimaryAttribute::Faith, 0.f));
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DStatics().CharismaDef, EvalParams, AttributeValues.Add(EPrimaryAttribute::Charisma, 0.f));
	for (const TPair<EPrimaryAttribute, float>& Pair : AttributeValues)
	{
		const EPrimaryAttribute Attr = Pair.Key;
		const float RawValue = FMath::Max(0.f, Pair.Value);
		const float ScaleFactor = DamageSpec.GetSetByCallerMagnitude(USinLibrary::GetTagFromAttribute(Attr), 0.f);
		if (ScaleFactor > 0.f)
		{
			const float Contribution = CalculateDamageBonus(SourceASC, Attr, RawValue, ScaleFactor);
			TotalDamageMod += Contribution;
		}
	}
	// GET FULL ATTRIBUTE float DamageMod = CalculateDamageBonus(SourceASC); 
	float StaminaDamage = 0.0f;
	float Avoidance = CalculateAvoidance(TargetASC); float ArP = CalculateAttackerArP(SourceASC);
	FPhysicalDamageProfile RawDamage = CalculateRawDamage(DamageSpec, TotalDamageMod);
	DamageReport.DamagePhysicalRaw = RawDamage.DamageValues;
	FPhysicalDefenseProfile RawDefenses = CalculateDefenses(TargetASC, Avoidance, ArP);
	// BOILER //
	//  BLOCKING LOGIC HERE // 
	if (Spec.GetDynamicAssetTags().HasTag(TAG_Substate_Block_Held) && IsAttackerInFront(SourceASC->GetAvatarActor(), TargetASC->GetAvatarActor(), 160.f))
	{
		FBlockResult Blocked = BlockFilter(TargetASC, ShieldSpec, RawDamage);
		RawDamage = Blocked.ReducedDamage;
		StaminaDamage = Blocked.StaminaCost;
	}
	//  BLOCKING LOGIC HERE // 
	float FinalDamage = RawDefenses.TotalMitigationFor(RawDamage);

	if (FinalDamage > 0.f)
	{
		DamageReport.FinalDamage = FinalDamage;
		// DELEGATES //
		SourceASC->SignalDamageReport.Broadcast(SourceASC, TargetASC,DamageReport);
		TargetASC->SignalDamageReport.Broadcast(SourceASC, TargetASC,DamageReport);
		// DELEGATES //
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DStatics().DamageProperty, EGameplayModOp::Additive, FinalDamage));
	}
	if (StaminaDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DStatics().StaminaProperty, EGameplayModOp::Additive, -StaminaDamage));
	}
}

// RAW PHYSICAL DAMAGE OF THE ATTACKER //
float USinPhysicalDamageCalculation::CalculateRawPhysicalDamage(USinASC* ASC, const FGameplayEffectSpec& DamageSpec, float& Pierce, float& Slash, float& Strike, float& Penetration) const
{
	float ComboModifier = DamageSpec.GetSetByCallerMagnitude(TAG_ComboModifier) ? DamageSpec.GetSetByCallerMagnitude(TAG_ComboModifier): 1.f;
	bool Dummy;
	float ModConstant = 5;
	float DamageMod;
	float RawStrength = ASC->GetGameplayAttributeValue(USinAttributePrimary::GetStrengthAttribute(), Dummy);
	float StrengthMod = (RawStrength / (RawStrength + ModConstant))* DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Major_Strength);
	float RawDexterity = ASC->GetGameplayAttributeValue(USinAttributePrimary::GetDexterityAttribute(), Dummy);
	float DexterityMod = (RawDexterity / (RawDexterity + ModConstant))* DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Major_Dexterity);
	float RawConstitution = ASC->GetGameplayAttributeValue(USinAttributePrimary::GetConstitutionAttribute(), Dummy);
	float ConstitutionMod = (RawConstitution / (RawConstitution + ModConstant)) * DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Major_Constitution);
	float RawSorcery = ASC->GetGameplayAttributeValue(USinAttributePrimary::GetSorceryAttribute(), Dummy);
	float SorceryMod = (RawSorcery / (RawSorcery + ModConstant)) * DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Major_Sorcery);
	DamageMod = 1.f + StrengthMod + DexterityMod+ ConstitutionMod+SorceryMod;
	Pierce = ComboModifier*DamageMod*DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Pierce);
	Slash = ComboModifier*DamageMod*DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Slash);
	Strike = ComboModifier*DamageMod*DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Strike);
	Penetration = DamageSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Penetration);
	return Pierce+Strike+Slash;
}

// MITIGATED DAMAGE BY SHIELD BEFORE PERSONAL DEFENSES
void USinPhysicalDamageCalculation::CalculateShieldMitigation(USinASC* ASC, const FGameplayEffectSpec& DefenseSpec, float& Pierce, float& Slash, float& Strike) const
{
	float FinalBlock = 0.f;
	float UniversalBlock = DefenseSpec.GetSetByCallerMagnitude(TAG_Damage_Block) ? DefenseSpec.GetSetByCallerMagnitude(TAG_Damage_Block) : 1.f;
	float PierceBlock = 0.f;
	float SlashBlock = 0.f;
	float StrikeBlock = 0.f;
	Pierce *= UniversalBlock;
	Slash *= UniversalBlock;
	Strike*= UniversalBlock;
}

// MITIGATED DAMAGE BASED ON THE VICTIMS DEFENSES //
float USinPhysicalDamageCalculation::CalculateMitigatedPhysicalDamage(USinASC* ASC, const FGameplayEffectSpec& DefenseSpec, float& Pierce, float& Slash, float& Strike, float& Penetration) const
{
	bool Dummy;
	float Avoidance = 1.f - ASC->GetGameplayAttributeValue(USinAttributeSecondary::GetAvoidanceAttribute(), Dummy);
	float Toughness = 1.f - ASC->GetGameplayAttributeValue(USinAttributeSecondary::GetToughnessAttribute(), Dummy);
	float PhysicalProtection;
	DefenseSpec.GetSetByCallerMagnitude(TAG_Gear) ?
		PhysicalProtection = DefenseSpec.GetSetByCallerMagnitude(TAG_Gear)
		: PhysicalProtection = 1.f;
	float MitigatedPierce;
	DefenseSpec.GetSetByCallerMagnitude(TAG_Gear) ?
		MitigatedPierce = DefenseSpec.GetSetByCallerMagnitude(TAG_Gear)
		: MitigatedPierce = 1.f; MitigatedPierce *= Pierce*Toughness*Avoidance*PhysicalProtection;
	float MitigatedSlash;
	DefenseSpec.GetSetByCallerMagnitude(TAG_Gear) ?
		MitigatedSlash = DefenseSpec.GetSetByCallerMagnitude(TAG_Gear)
		: MitigatedSlash = 1.f; MitigatedSlash *= Slash*Toughness * Avoidance * PhysicalProtection;
	float MitigatedStrike;
	DefenseSpec.GetSetByCallerMagnitude(TAG_Gear) ?
		MitigatedStrike = DefenseSpec.GetSetByCallerMagnitude(TAG_Gear)
		: MitigatedStrike = 1.f; MitigatedStrike *= Strike * Toughness * Avoidance * PhysicalProtection;
	return MitigatedStrike+MitigatedSlash+MitigatedPierce;
}

FPhysicalDefenseProfile USinPhysicalDamageCalculation::CalculateDefenses(USinASC* ASC, float DefenderAvoidance, float ArmorPenetration) const
{
	FPhysicalDefenseProfile Defenses;
	const float ArmorCoefficient = CalculateArmorCoefficient(ASC);
	const float ArmorRating = ASC->GetSetByCallerMagnitude(TAG_Resistance_Physical, TAG_Resistance_Physical_Mult);
	const float AdjustedArmor = ArmorRating * (1.0f - ArmorPenetration);
	Defenses.Armor = AdjustedArmor / (AdjustedArmor + ArmorCoefficient);

	Defenses.Slash = DefenderAvoidance* ASC->GetSetByCallerMagnitudeWithCap(TAG_Resistance_Physical_Slash, TAG_Resistance_Physical_Slash_Mult, TAG_Resistance_Physical_Slash_Cap);

	Defenses.Strike = DefenderAvoidance* ASC->GetSetByCallerMagnitudeWithCap(TAG_Resistance_Physical_Strike, TAG_Resistance_Physical_Strike_Mult, TAG_Resistance_Physical_Strike_Cap);

	Defenses.Pierce = DefenderAvoidance* ASC->GetSetByCallerMagnitudeWithCap(TAG_Resistance_Physical_Pierce, TAG_Resistance_Physical_Pierce_Mult, TAG_Resistance_Physical_Pierce_Cap);

	return Defenses;
}

FPhysicalDamageProfile USinPhysicalDamageCalculation::CalculateRawDamage(const FGameplayEffectSpec& WeaponSpec, float DamageMod) const
{
	FPhysicalDamageProfile Damage;
	// strength and dex bonuses, class and weapon mastery bonuses, buffs, etc
	float ComboModifier = WeaponSpec.GetSetByCallerMagnitude(TAG_ComboModifier) ? WeaponSpec.GetSetByCallerMagnitude(TAG_ComboModifier) : 1.f;

	Damage.SetValue(TAG_Attribute_Secondary_DamageType_Physical_Slash, WeaponSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Slash) * DamageMod * ComboModifier);
	//Damage.Slash = WeaponSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Slash)*DamageMod* ComboModifier*DefenderAvoidance;
	Damage.SetValue(TAG_Attribute_Secondary_DamageType_Physical_Strike, WeaponSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Strike) * DamageMod * ComboModifier);
	//Damage.Strike = WeaponSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Strike) * DamageMod * ComboModifier*DefenderAvoidance;
	Damage.SetValue(TAG_Attribute_Secondary_DamageType_Physical_Pierce, WeaponSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Pierce) * DamageMod * ComboModifier);
	//Damage.Pierce = WeaponSpec.GetSetByCallerMagnitude(TAG_Attribute_Secondary_DamageType_Physical_Pierce) * DamageMod * ComboModifier*DefenderAvoidance;
	return Damage;
}

float USinPhysicalDamageCalculation::CalculateAttackerArP(USinASC* AttackerASC) const
{
	return 0.0f;
}

float USinPhysicalDamageCalculation::CalculateArmorCoefficient(USinASC* VictimASC) const
{
	return 250.f;
}

float USinPhysicalDamageCalculation::CalculateShieldCoefficient(USinASC* VictimASC) const
{
	return 250.f;
}

float USinPhysicalDamageCalculation::CalculateAvoidance(USinASC* VictimASC) const
{
	return 1.0f;
}

float USinPhysicalDamageCalculation::CalculateDamageBonus(USinASC* AttackerASC, EPrimaryAttribute Attribute, float AttributeValue, float AttributeScaler) const
{
	// y = (x / (x + Taper)) * Ceiling
	float Taper = 25.f; float Ceiling = AttackerASC->GetAttributeScalingCeiling(Attribute);
	float Contribution = ((AttributeValue / (AttributeValue + Taper)) * Ceiling)*AttributeScaler;
	return Contribution;
}

float USinPhysicalDamageCalculation::CalculateShieldRatingBonus(USinASC* AttackerASC) const
{
	return 1.0f;
}

float USinPhysicalDamageCalculation::CalculateBlockStaminaFactor(USinASC* VictimASC, float RawBlockedDamage) const
{
	return RawBlockedDamage * 0.5f;
}

FBlockResult USinPhysicalDamageCalculation::BlockFilter(USinASC* ASC, const FGameplayEffectSpec& DefenseSpec, FPhysicalDamageProfile RawDamage) const
{
	// float OriginalSlash = RawDamage.Slash;
	float OriginalSlash = RawDamage.Slash;
	float OriginalStrike = RawDamage.Strike;
	float OriginalPierce = RawDamage.Pierce;

	float BlockRating = DefenseSpec.GetSetByCallerMagnitude(TAG_Block) ? DefenseSpec.GetSetByCallerMagnitude(TAG_Block) : 1.f;
	BlockRating *= CalculateShieldRatingBonus(ASC);
	float BlockCoefficient = CalculateShieldCoefficient(ASC); // Currently returns 250.f until we code a perk system and a timed block system
	float BlockPercent = BlockRating / (BlockRating + BlockCoefficient);
	float BlockSlash = DefenseSpec.GetSetByCallerMagnitude(TAG_Block_Slash) ? DefenseSpec.GetSetByCallerMagnitude(TAG_Block_Slash) : 1.f;
	float BlockStrike = DefenseSpec.GetSetByCallerMagnitude(TAG_Block_Strike) ? DefenseSpec.GetSetByCallerMagnitude(TAG_Block_Strike) : 1.f;
	float BlockPierce = DefenseSpec.GetSetByCallerMagnitude(TAG_Block_Pierce) ? DefenseSpec.GetSetByCallerMagnitude(TAG_Block_Pierce) : 1.f;

	// CALCULATE STAMINA COST?
	RawDamage.Slash *= FMath::Clamp(1.f - (BlockPercent + BlockSlash), 0.f, 1.f);
	RawDamage.Strike *= FMath::Clamp(1.f - (BlockPercent + BlockStrike), 0.f, 1.f);
	RawDamage.Pierce *= FMath::Clamp(1.f - (BlockPercent + BlockPierce), 0.f, 1.f);
	// Calculate how much was blocked
	float BlockedSlash = OriginalSlash - RawDamage.Slash;
	float BlockedStrike = OriginalStrike - RawDamage.Strike;
	float BlockedPierce = OriginalPierce - RawDamage.Pierce;
	float RawBlockedDamage = BlockedSlash + BlockedStrike + BlockedPierce;
	float StaminaCost = CalculateBlockStaminaFactor(ASC, RawBlockedDamage);
	FBlockResult Result;
	Result.ReducedDamage = RawDamage;
	Result.StaminaCost = StaminaCost;
	return Result;
}

bool USinPhysicalDamageCalculation::IsAttackerInFront(AActor* Attacker, AActor* Defender, float ViewAngleDegrees) const
{
	if (!Attacker || !Defender) return false;

	const FVector Forward = Defender->GetActorForwardVector().GetSafeNormal();
	const FVector ToAttacker = (Attacker->GetActorLocation() - Defender->GetActorLocation()).GetSafeNormal();
	const float Dot = FVector::DotProduct(Forward, ToAttacker);

	const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(ViewAngleDegrees * 0.5f));
	return Dot >= CosThreshold;
}