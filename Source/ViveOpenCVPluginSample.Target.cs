// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ViveOpenCVPluginSampleTarget : TargetRules
{
	public ViveOpenCVPluginSampleTarget( TargetInfo Target ) : base( Target )
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ViveOpenCVPluginSample" } );
	}
}