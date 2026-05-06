// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Sin : ModuleRules
{
	public Sin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NetCore", "UE5Coro", "EasyMultiSave",
		"GameplayTags", 
		"GameplayAbilities", 
		"GameplayTasks",
		"EnhancedInput",
		"AnimGraphRuntime",
		"Niagara"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		CppStandard = CppStandardVersion.Cpp20;

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
