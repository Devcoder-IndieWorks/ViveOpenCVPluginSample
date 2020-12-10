// Copyright Vive-Studios. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class ViveOpenCVPlugin : ModuleRules
{
	public ViveOpenCVPlugin( ReadOnlyTargetRules Target )
	    : base( Target )
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = false;
		
		PublicIncludePaths.AddRange(
		    new string[] {
				"Runtime/ImageWriteQueue/Public"
			} );
			
		PrivateIncludePaths.AddRange(
		    new string[] {
				"ViveOpenCVPlugin/Private"
			} );
			
		PrivateDependencyModuleNames.AddRange(
		    new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"EngineSettings",
				"RenderCore",
				"RHI",
				"Projects",
				"CinematicCamera",
				"ImageWrapper", 
				"ImageWriteQueue",
				"SteamVR"
			} );
			
		if ( Target.bBuildEditor )
			PrivateDependencyModuleNames.Add( "UnrealEd" );
		
		LoadOpenCV( Target );
		
		Console.WriteLine( "Include headers from directories:" );
		PublicIncludePaths.ForEach( m => Console.WriteLine( " " + m ) );
		
		Console.WriteLine( "Libraries - Static:" );
		PublicAdditionalLibraries.ForEach( m => Console.WriteLine( " " + m ) );
		
		Console.WriteLine( "Libraries - Dynamic:" );
		PublicDelayLoadDLLs.ForEach( m => Console.WriteLine( " " + m ) );
	}
	
	//-------------------------------------------------------------------------
		
	protected void LoadOpenCV( ReadOnlyTargetRules Target )
	{
		var opencvDir = Path.Combine( ThirdPartyPath, "OpenCV" );
		
		// Include opencv headers
		PublicIncludePaths.Add( Path.Combine( opencvDir, "include" ) );
		if ( Target.Platform == UnrealTargetPlatform.Win64 ) {
			bEnableExceptions = true;
			Console.WriteLine( "VIVE: OpenCV for Win64" );
			
			var suffix = OpenCVVersion;
			if ( IsDebug( Target ) ) {
				Console.WriteLine( "VIVE: Debug" );
				suffix += "d";
			}
			else {
				Console.WriteLine( "VIVE: Not debug" );
			}
			
			// Static linking
			var libDir = Path.Combine( opencvDir, "x64", "vc16", "lib" );
			PublicAdditionalLibraries.AddRange(
			    OpenCVModules.ConvertAll( m => Path.Combine( libDir, m + suffix + ".lib" ) )
			);
			
			// Dynamic libraries
			// Delay load the DLL, so we can load it from the right place first.
			PublicDelayLoadDLLs.AddRange(
			    OpenCVModules.ConvertAll( m => m + suffix + ".dll" )
			);
			
			// Add a Runtime Dependency so the DLLs will be packaged correctly.
			var dllDir = Path.Combine( opencvDir, "x64", "vc16", "bin" );
			OpenCVModules
			.ConvertAll( m => Path.Combine( dllDir, m + suffix + ".dll" ) )
			.ForEach(
			    m => {
					RuntimeDependencies.Add( m );
					Console.WriteLine( "Runtime dependencies:" );
					Console.WriteLine( " " + m );
				}
			);
		}
		else {
			Console.WriteLine( "VIVE: No prebuild binaries for OpenCV on platform" + Target.Platform );
		}
	}
	
	protected bool IsDebug( ReadOnlyTargetRules Target )
	{
		return Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT;
	}
	
	protected string PluginRootDirectory
	{
		get {
			return Path.GetFullPath( Path.Combine( ModuleDirectory, "../../" ) );
		}
	}
	
	protected string ThirdPartyPath
	{
		get {
			return Path.Combine( PluginRootDirectory, "ThirdParty/" );
		}
	}
	
	protected string BinariesDir
	{
		get {
			return Path.Combine( PluginRootDirectory, "Binaries" );
		}
	}
	
	//-------------------------------------------------------------------------
	
	protected List<string> OpenCVModules = new List<string>(){ "opencv_world" };
	protected string OpenCVVersion = "430";
}