// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MoveAssets : ModuleRules
{
	public MoveAssets(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "DeveloperSettings", 
                "Blutility",
                "SlateCore"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "DeveloperSettings",
                "ContentBrowser",
                "ContentBrowserData",
                "Slate",
                "SlateCore", 
                "AssetRegistry",
                "AssetTools",
                "UMGEditor",
                "UnrealEd",
                "UMG", 
                "EditorWidgets", 

			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
