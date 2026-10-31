using UnrealBuildTool;

public class Edrath : ModuleRules
{
    public Edrath(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
        bUseUnity = true;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            "GameplayTags", "Niagara", "UMG", "AIModule", "NavigationSystem"
        });
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate", "SlateCore", "RenderCore", "RHI"
        });
    }
}
