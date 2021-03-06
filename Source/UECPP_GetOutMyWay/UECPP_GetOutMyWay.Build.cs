// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UECPP_GetOutMyWay : ModuleRules
{
	public UECPP_GetOutMyWay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara", "OnlineSubsystem", "OnlineSubsystemSteam","Steamworks"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		//인텔리센스오류 수정
		PublicIncludePaths.Add(ModuleDirectory);
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
