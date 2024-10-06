// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InspectorPlayFi : ModuleRules
{
	public InspectorPlayFi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Blutility",
				"EditorScriptingUtilities",
				"AssetTools"
			}
			);
	}
}
