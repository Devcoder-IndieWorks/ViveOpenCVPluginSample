#include "ViveDeveloperSettings.h"
#include "ViveStudiosUtilsPCH.h"

UViveDeveloperSettings::UViveDeveloperSettings( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    OutputMapDirName = TEXT( "DistortionCorrectionMaps/" );
    OutputCalibDirName = TEXT( "CameraCalibration/" );
    SaveDistortionCorrectionMap = true;
    ShowLog = true;
}
