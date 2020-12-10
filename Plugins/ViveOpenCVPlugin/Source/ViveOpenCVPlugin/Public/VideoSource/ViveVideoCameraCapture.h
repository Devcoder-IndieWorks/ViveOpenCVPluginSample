// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "ViveVideoSource.h"
#include "ViveVideoCameraCapture.generated.h"

UCLASS( Blueprintable, BlueprintType ) 
class VIVEOPENCVPLUGIN_API UViveVideoCameraCapture final : public UViveVideoSource
{
    GENERATED_UCLASS_BODY()
public:
    FIntPoint GetResolution() const;

    //-------------------------------------------------------------------------

    bool Connect( const FViveVideoConfiguration& InConfiguration );
    bool IsConnected() const;
    void Disconnect();

    //-------------------------------------------------------------------------

    bool GetNextFrame( cv::Mat_<cv::Vec3b>& OutFrame );

    //-------------------------------------------------------------------------

    float GetFrequency() const;

    //-------------------------------------------------------------------------

    FString GetSourceName() const;
    FString GetIdentifier() const;

private:
    cv::VideoCapture Capture;
};
