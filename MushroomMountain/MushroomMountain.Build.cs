// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MushroomMountain : ModuleRules
{
    public MushroomMountain(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "EnhancedInput",
            "GameplayTags",   // ✅ GameplayTags 모듈
            "UMG",            // ✅ UMG 모듈
            "Slate",          // ✅ Slate 모듈
            "SlateCore"       // ✅ SlateCore 모듈
        });
    }
}


