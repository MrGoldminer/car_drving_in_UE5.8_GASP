using UnrealBuildTool;

public class GameAnimationSampleEditorTarget : TargetRules
{
	public GameAnimationSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GameAnimationSample");
	}
}
