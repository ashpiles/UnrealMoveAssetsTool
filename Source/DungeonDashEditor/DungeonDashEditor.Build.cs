using UnrealBuildTool;

public class DungeonDashEditor : ModuleRules
{
    public DungeonDashEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "DeveloperSettings", 
                "Blutility"
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
    }
}