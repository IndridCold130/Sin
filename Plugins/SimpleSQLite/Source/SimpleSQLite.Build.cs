// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SimpleSQLite : ModuleRules
{
	public SimpleSQLite(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "SQLiteSupport" });


		PublicIncludePaths.AddRange(
			 new string[] {
				// ... add public include paths required here ...
				ModuleDirectory,
			 }
			 );
	}
}
