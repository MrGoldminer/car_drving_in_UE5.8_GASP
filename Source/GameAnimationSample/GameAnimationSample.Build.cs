using UnrealBuildTool;

public class GameAnimationSample : ModuleRules
{
	public GameAnimationSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"PhysicsCore",
				"Mover",
				"MoverExamples"
			});
	}
}
