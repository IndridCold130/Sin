// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/SinGPTs.h"

//Character Identity
UE_DEFINE_GAMEPLAY_TAG(TAG_Race, "Race");
UE_DEFINE_GAMEPLAY_TAG(TAG_Race_Human, "Race.Human");
UE_DEFINE_GAMEPLAY_TAG(TAG_Race_HalfOrc, "Race.HalfOrc");
UE_DEFINE_GAMEPLAY_TAG(TAG_Race_Elf, "Race.Elf");
UE_DEFINE_GAMEPLAY_TAG(TAG_Race_Goblin, "Race.Goblin");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gender, "Gender");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gender_Female, "Gender.Female");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gender_Male, "Gender.Male");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gender_Intersex_Female, "Gender.Intersex.Female");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gender_Intersex_Male, "Gender.Intersex.Male");

// Character State Tags
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Passive, "State.Passive");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Aggressive, "State.Aggressive");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Sex, "State.Sex");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_FFYL, "State.FFYL");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Unconscious, "State.Unconscious");
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Dead, "State.Dead");


/** Define and expose the gameplay tag "Movement.Mode.Walking" to other modules and code. */
// Character Substate Tags
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Dodge, "Substate.Dodge");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Dodge_Invulnerability, "Substate.Dodge.Invulnerability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Attack, "Substate.Attack");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_SpellCast, "Substate.SpellCast");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_SpellCast_EnterLoop, "Substate.SpellCast.EnterLoop");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_SpellCast_Finish, "Substate.SpellCast.Finish");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Block, "Substate.Block");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Block_Held, "Substate.Block.Held");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Block_DeflectWindow, "Substate.Block.DeflectWindow");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Block_Recoil, "Substate.Block.Recoil");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Knockback, "Substate.Knockback");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Sprint, "Substate.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Sit, "Substate.Sit");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Crouch, "Substate.Crouch");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Swim, "Substate.Swim");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_UseItem, "Substate.UseItem");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_EquipWeapon, "Substate.EquipWeapon");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_ComboQueue, "Substate.ComboQueue");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_ComboTrigger, "Substate.ComboTrigger");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_DodgeWindow, "Substate.DodgeWindow");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_PivotWindow, "Substate.PivotWindow");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Hold_MainHand, "Substate.Hold.MainHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Hold_OffHand, "Substate.Hold.OffHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Substate_Hold_RightHandStyle, "Substate.RightHandStyle");


// ITEM GAMEPLAYTAGS
UE_DEFINE_GAMEPLAY_TAG(TAG_Item, "Item");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Currency, "Item.Currency");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment, "Item.Equipment");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Weapon, "Item.Equipment.Weapon");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Weapon_MainHand, "Item.Equipment.Weapon.MainHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Weapon_OneHand, "Item.Equipment.Weapon.OneHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Weapon_OffHand, "Item.Equipment.Weapon.OffHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Head, "Item.Equipment.Head");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Chest, "Item.Equipment.Chest");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Hands, "Item.Equipment.Hands");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Legs, "Item.Equipment.Legs");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Feet, "Item.Equipment.Feet");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Jewellery, "Item.Equipment.Jewellery");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Jewellery_Finger, "Item.Equipment.Jewellery.Finger");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Jewellery_Neck, "Item.Equipment.Jewellery.Neck");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Undergarments, "Item.Equipment.Undergarments");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Undergarments_Panties, "Item.Equipment.Undergarments.Panties");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Undergarments_Top, "Item.Equipment.Undergarments.Top");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Undergarments_Hosiery, "Item.Equipment.Undergarments.Hosiery");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equipment_Utility, "Item.Equipment.Utility");
// ATTRIBUTES REDUX
UE_DEFINE_GAMEPLAY_TAG(TAG_ComboModifier, "ComboModifier");
UE_DEFINE_GAMEPLAY_TAG(TAG_Mult, "Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Buff, "Buff");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class, "Class");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Barbarian, "Class.Barbarian");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Cleric, "Class.Cleric");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Druid, "Class.Druid");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Fighter, "Class.Fighter");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Ranger, "Class.Ranger");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Rogue, "Class.Rogue");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Paladin, "Class.Paladin");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Sorcerer, "Class.Sorcerer");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Warlock, "Class.Warlock");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage, "Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage_Block, "Damage.Block");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage_Block_Stamina, "Damage.Block.Stamina");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage_Block_Timed, "Damage.Block.Timed");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage_Block_OH, "Damage.Block.OH");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block, "Block");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Physical, "Block.Physical");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Slash, "Block.Slash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Strike, "Block.Strike");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Pierce, "Block.Pierce");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Fire, "Block.Fire");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Cold, "Block.Cold");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Lightning, "Block.Lightning");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Arcane, "Block.Arcane");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Dark, "Block.Dark");
UE_DEFINE_GAMEPLAY_TAG(TAG_Block_Holy, ".Block.Holy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Aura, "Source.Aura");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Buff, "Source.Buff");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Curse, "Source.Curse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Spell, "Source.Spell");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Gear, "Source.Gear");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Perk, "Source.Perk");
UE_DEFINE_GAMEPLAY_TAG(TAG_Source_Potion, "Source.Potion");
UE_DEFINE_GAMEPLAY_TAG(TAG_Curse, "Curse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gear, "Gear");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gear_Mult, "Gear.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Perk, "Perk");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health, "Health");
UE_DEFINE_GAMEPLAY_TAG(TAG_Racial, "Racial");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Damage, "Health.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Full, "Health.Full");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Max, "Health.Max");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Regen, "Health.Regen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Regen_Gear, "Health.Regen.Gear");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Regen_Perk, "Health.Regen.Perk");
UE_DEFINE_GAMEPLAY_TAG(TAG_Health_Regen_Buff, "Health.Regen.Buff");
UE_DEFINE_GAMEPLAY_TAG(TAG_Stamina, "Stamina");
UE_DEFINE_GAMEPLAY_TAG(TAG_Stamina_Full, "Stamina.Full");
UE_DEFINE_GAMEPLAY_TAG(TAG_Stamina_Max, "Stamina.Max");
UE_DEFINE_GAMEPLAY_TAG(TAG_Stamina_Regen, "Stamina.Regen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Stamina_Damage, "Stamina.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Mana, "Mana");
UE_DEFINE_GAMEPLAY_TAG(TAG_Mana_Max, "Mana.Max");
UE_DEFINE_GAMEPLAY_TAG(TAG_Mana_Full, "Mana.Full");
UE_DEFINE_GAMEPLAY_TAG(TAG_Mana_Regen, "Mana.Regen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Mana_Damage, "Mana.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Rage, "Rage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Rage_Depleted, "Rage.Depleted");
UE_DEFINE_GAMEPLAY_TAG(TAG_Rage_Gain, "Rage.Gain");
// MAJOR ATTRIBUTES
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major, "Attribute.Major");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Health, "Attribute.Major.Health");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_HealthMult, "Attribute.Major.HealthMult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_HealthMax, "Attribute.Major.HealthMax");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Stamina, "Attribute.Major.Stamina");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_StaminaMult, "Attribute.Major.StaminaMult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_StaminaMax, "Attribute.Major.StaminaMax");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Energy, "Attribute.Major.Energy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_EnergyMax, "Attribute.Major.EnergyMax");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_EnergyMult, "Attribute.Major.EnergyMult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Strength, "Attribute.Major.Strength");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Strength_Scaling, "Attribute.Major.Strength.Scaling");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Dexterity, "Attribute.Major.Dexterity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Dexterity_Scaling, "Attribute.Major.Dexterity.Scaling");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Constitution, "Attribute.Major.Constitution");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Constitution_Scaling, "Attribute.Major.Constitution.Scaling");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Sorcery, "Attribute.Major.Sorcery");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Sorcery_Scaling, "Attribute.Major.Sorcery.Scaling");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Faith, "Attribute.Major.Faith");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Faith_Scaling, "Attribute.Major.Faith.Scaling");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Charisma, "Attribute.Major.Charisma");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Major_Charisma_Scaling, "Attribute.Major.Charisma.Scaling");

// SECONDARY ATTRIBUTES

UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary, "Attribute.Secondary");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_HealthRegen, "Attribute.Secondary.HealthRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_EnergyRegen, "Attribute.Secondary.EnergyRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_StaminaRegen, "Attribute.Secondary.StaminaRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_Toughness, "Attribute.Secondary.Toughness");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_Avoidance, "Attribute.Secondary.Avoidance");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_Resilience, "Attribute.Secondary.Resilience");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_Maneuverability, "Attribute.Secondary.Maneuverability");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance, "Resistance");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical, "Resistance.Physical");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Mult, "Resistance.Physical.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Cap, "Resistance.Physical.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Status, "Resistance.Status");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Arcane, "Resistance.Arcane");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element, "Resistance.Element");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Slash, "Resistance.Physical.Slash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Slash_Mult, "Resistance.Physical.Slash.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Slash_Cap, "Resistance.Physical.Slash.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Strike, "Resistance.Physical.Strike");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Strike_Mult, "Resistance.Physical.Strike.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Strike_Cap, "Resistance.Physical.Strike.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Pierce, "Resistance.Physical.Pierce");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Pierce_Mult, "Resistance.Physical.Pierce.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Physical_Pierce_Cap, "Resistance.Physical.Pierce.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Fire, "Resistance.Element.Fire");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Fire_Mult, "Resistance.Element.Fire.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Fire_Cap, "Resistance.Element.Fire.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Cold, "Resistance.Element.Cold");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Cold_Mult, "Resistance.Element.Cold.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Cold_Cap, "Resistance.Element.Cold.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Lightning, "Resistance.Element.Lightning");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Lightning_Mult, "Resistance.Element.Lightning.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Element_Lightning_Cap, "Resistance.Element.Lightning.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Dark, "Resistance.Dark");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Dark_Mult, "Resistance.Dark.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Dark_Cap, "Resistance.Dark.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Holy, "Resistance.Holy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Holy_Mult, "Resistance.Holy.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Holy_Cap, "Resistance.Holy.Cap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Status_Bleed, "Resistance.Status.Bleed");
UE_DEFINE_GAMEPLAY_TAG(TAG_Resistance_Status_Poison, "Resistance.Status.Poison");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical, "Attribute.Secondary.DamageType.Physical");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Slash, "Attribute.Secondary.DamageType.Physical.Slash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Slash_Mult, "Attribute.Secondary.DamageType.Physical.Slash.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Strike, "Attribute.Secondary.DamageType.Physical.Strike");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Strike_Mult, "Attribute.Secondary.DamageType.Physical.Strike.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Pierce, "Attribute.Secondary.DamageType.Physical.Pierce");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Pierce_Mult, "Attribute.Secondary.DamageType.Physical.Pierce.Mult");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Physical_Penetration, "Attribute.Secondary.DamageType.Physical.Penetration");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Magic, "Attribute.Secondary.DamageType.Magic");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Magic_Fire, "Attribute.Secondary.DamageType.Magic.Fire");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Magic_Cold, "Attribute.Secondary.DamageType.Magic.Cold");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Magic_Dark, "Attribute.Secondary.DamageType.Magic.Dark");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Magic_Lightning, "Attribute.Secondary.DamageType.Magic.Lightning");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Magic_Holy, "Attribute.Secondary.DamageType.Magic.Holy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Internal, "Attribute.Secondary.DamageType.Internal");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Internal_Bleed, "Attribute.Secondary.DamageType.Internal.Bleed");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Internal_Poison, "Attribute.Secondary.DamageType.Internal.Poison");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_DamageType_Internal_Disease, "Attribute.Secondary.DamageType.Internal.Disease");
// speed increasing stats
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_AttackSpeed, "Attribute.Secondary.AttackSpeed");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_Acumen, "Attribute.Secondary.Acumen");
// util speed stats
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_RunSpeed, "Attribute.Secondary.RunSpeed");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Secondary_SwimSpeed, "Attribute.Secondary.SwimSpeed");

// PROGRESSION ATTRIBUTES

UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Progression, "Attribute.Progression");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Progression_Level, "Attribute.Progression.Level");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Progression_Experience, "Attribute.Progression.Experience");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Progression_SpentAttributePoints, "Attribute.Progression.SpentAttributePoints");
UE_DEFINE_GAMEPLAY_TAG(TAG_Attribute_Progression_UnspentAttributePoints, "Attribute.Progression.UnspentAttributePoints");

// Material Types

UE_DEFINE_GAMEPLAY_TAG(TAG_Material, "Material");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal, "Material.Metal");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Mineral, "Material.Mineral");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Wood, "Material.Wood");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Bronze, "Material.Metal.Bronze");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Iron, "Material.Metal.Iron");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Iron_Cold, "Material.Metal.Iron.Cold");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Iron_Rusted, "Material.Metal.Iron.Rusted");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Silver, "Material.Metal.Silver");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Steel, "Material.Metal.Steel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Damascus, "Material.Metal.Damascus");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Blacksteel, "Material.Metal.Blacksteel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Material_Metal_Gold, "Material.Metal.Gold");

// FACTION TAGS

UE_DEFINE_GAMEPLAY_TAG(TAG_Faction_Undead, "Faction.Undead");
UE_DEFINE_GAMEPLAY_TAG(TAG_Faction_Player, "Faction.Player");

// INPUT TAGS

UE_DEFINE_GAMEPLAY_TAG(TAG_Input, "Input");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_SwitchMainHand, "Input.SwitchMainHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_SwitchOffHand, "Input.SwitchOffHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Ability_Block, "Input.Ability.Block");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Move, "Input.Move");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Dodge, "Input.Dodge");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Dodge_OnRelease, "Input.Dodge.OnRelease");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Sprint, "Input.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Action, "Input.Action");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Action_Secondary, "Input.Action.Secondary");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Action_Special, "Input.Action.Special");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_ActionCombo, "Input.ActionCombo");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_ActionSecondary, "Input.ActionSecondary");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_ActionSpecial, "Input.ActionSpecial");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Jump, "Input.Jump");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_DrawWeapon, "Input.DrawWeapon");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Crouch, "Input.Crouch");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Guard, "Input.Guard");

// MENU INPUT
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Escape, "Menu.Escape");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Level, "Menu.Level");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Loot, "Menu.Loot");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Quickloot, "Menu.Quickloot");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Carousel, "Menu.Carousel");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Carousel_Character, "Menu.Carousel.Character");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Carousel_Inventory, "Menu.Carousel.Inventory");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Carousel_Journal, "Menu.Carousel.Journal");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Carousel_Spellbook, "Menu.Carousel.Spellbook");
UE_DEFINE_GAMEPLAY_TAG(TAG_Menu_Carousel_Perks, "Menu.Carousel.Perks");
// GAMEPLAY CUES

UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_HitPhysical, "GameplayCue.HitPhysical");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_FireShield, "GameplayCue.FireShield");
UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayCue_Firebolt, "GameplayCue.Firebolt");