using UnrealBuildTool;
using System.Collections.Generic;

public class EdrathTarget : TargetRules
{
    public EdrathTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.Add("Edrath");
    }
}
