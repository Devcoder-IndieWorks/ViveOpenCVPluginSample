// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "VideoDriver/ViveOpenCV.h"
#include "UObject/Object.h"
#include "ViveVideoConfiguration.h"
#include "ViveVideoSource.generated.h"

UCLASS( Abstract, Blueprintable, BlueprintType ) 
class VIVEOPENCVPLUGIN_API UViveVideoSource : public UObject
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="VideoSource|Functions" )
    void SetupConfigurations();

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="VideoSource|Functions" )
    virtual FIntPoint GetResolution() const;
    UFUNCTION( BlueprintCallable, Category="VideoSource|Functions" )
    virtual bool IsConnected() const;
    UFUNCTION( BlueprintCallable, Category="VideoSource|Functions" )
    virtual FString GetSourceName() const;
    UFUNCTION( BlueprintCallable, Category="VideoSource|Functions" )
    virtual float GetFrequency() const;

    UFUNCTION( BlueprintCallable, Category="VideoSource|Functions" )
    const FViveVideoConfiguration& GetCurrentConfiguration() const;

public:
    virtual bool Connect( const FViveVideoConfiguration& InConfiguration );
    virtual void Disconnect();

    //-------------------------------------------------------------------------

    virtual bool GetNextFrame( cv::Mat_<cv::Vec3b>& OutFrame );

    //-------------------------------------------------------------------------

    virtual FString GetIdentifier() const;

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="VideoSource|Settings" )
    FString CalibPoseOutputFileName;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="VideoSource|Settings" )
    FString CalibLensOutputFileName;

    //-------------------------------------------------------------------------

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="VideoSource|Settings" )
    TArray<FViveVideoConfiguration> Configurations;

protected:
    UPROPERTY( Transient, BlueprintReadOnly, Category=VideoSource )
    FViveVideoConfiguration CurrentConfiguration;
};
