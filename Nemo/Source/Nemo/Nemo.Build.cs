// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Nemo : ModuleRules
{
	public Nemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"AIModule",								// AI 관련 클래스들이 들어있는 모듈
			"NavigationSystem",						// NavMesh 관련 기능 모듈 (AI가 길 찾을 때 필요)
			"UMG"						
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
