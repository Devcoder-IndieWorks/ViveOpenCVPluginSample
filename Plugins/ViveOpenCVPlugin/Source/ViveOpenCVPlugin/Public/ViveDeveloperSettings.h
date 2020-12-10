// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "Engine/DeveloperSettings.h"
#include "ViveDeveloperSettings.generated.h"

UCLASS( config=ViveStudiosUtils, defaultconfig, meta=(DisplayName="ViveStudios OpenCV Plugin") ) 
class VIVEOPENCVPLUGIN_API UViveDeveloperSettings : public UDeveloperSettings
{
    GENERATED_UCLASS_BODY()
public:
    UPROPERTY( config, EditAnywhere, Category="Configuration" )
    FIntPoint OutputMapResolution;

    UPROPERTY( config, EditAnywhere, Category="Configuration" )
    FString OutputMapDirName;

    UPROPERTY( config, EditAnywhere, Category="Configuration" )
    FString OutputCalibDirName;

    UPROPERTY( config, EditAnywhere, Category="Configuration" )
    FString OutputMapFullPath;

    UPROPERTY( config, EditAnywhere, Category="Configuration" )
    bool SaveDistortionCorrectionMap;

    //-------------------------------------------------------------------------

    UPROPERTY( config, EditAnywhere, Category="Settings" )
    bool ShowLog;
};
