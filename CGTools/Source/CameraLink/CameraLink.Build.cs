// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class CameraLink : ModuleRules
{
    public CameraLink(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
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
                "CoreUObject",
                "Engine",
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "LevelEditor",
                "ContentBrowser",
                "AssetTools",
                "AssetRegistry",
                "UnrealEd",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "LevelSequence",
                "MessageLog",
                "Sockets",
                "Networking",
                "MovieSceneTracks",
                "SequencerScripting",
                "PythonScriptPlugin",
                "CinematicCamera",
                "Json",
                "MovieScene",
                "RenderCore",
                "RHI",
                "EditorScriptingUtilities",
                "Projects"
				// ... add private dependencies that you statically link with here ...	
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
