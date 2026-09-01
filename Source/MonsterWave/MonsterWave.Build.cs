// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MonsterWave : ModuleRules
{
	public MonsterWave(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "UMG", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
