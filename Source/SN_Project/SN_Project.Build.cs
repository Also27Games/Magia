// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SN_Project : ModuleRules
{
	public SN_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem" });
	}
}
