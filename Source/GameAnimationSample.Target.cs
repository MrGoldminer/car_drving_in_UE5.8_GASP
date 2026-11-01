using UnrealBuildTool;

public class GameAnimationSampleTarget : TargetRules
{
	public GameAnimationSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GameAnimationSample");
	}
}
