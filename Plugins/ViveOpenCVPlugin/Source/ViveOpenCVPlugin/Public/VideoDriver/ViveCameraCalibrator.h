// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "VideoSource/ViveVideoCameraProperties.h"
#include "UObject/Object.h"
#include "ViveCameraCalibrator.generated.h"

UCLASS()
class VIVEOPENCVPLUGIN_API UViveCameraCalibrator : public UObject
{
    GENERATED_UCLASS_BODY()
public:
    void Shutdown();
    void Reset( const struct FViveVideoConfiguration* InVideoConfig );
    bool ProcessFrame( cv::Mat& InOutFrame, const FString& InModeType, float InTimeNow, 
        class UViveVideoDriver* InDriver );
    bool RectifyMap( cv::Mat& InOutFrame, const FString& InModeType );
    void SetupPredefinedDictionary( cv::aruco::PREDEFINED_DICTIONARY_NAME InName );
    void EstimatePoseSingleMarker( cv::Mat& InOutFrame );

    //-------------------------------------------------------------------------

    void SavePoseCalibToFile( const FString& InFilename );
    void SaveLensCalibToFile( const FString& InFilename );

    //-------------------------------------------------------------------------

    bool IsFinished() const;
    bool IsStepFinished() const;
    float GetProgressRatio() const;
    const FViveVideoCameraProperties& GetVideoCameraProperties() const;

private:
    void CalculateCalibration( cv::Size InFrameSize );

private:
    float MinInterval;
    float LastFrameTime;

    //-------------------------------------------------------------------------

    int32 FramesNeeded;
    int32 StepCountsNeeded;
    int32 CalibrationFlags;
    cv::Size PatternSize;
    cv::Size WinSize;
    float SquareSize;
    float MarkerSize;

    //-------------------------------------------------------------------------

    bool MustInitUndistortRemap;
    cv::Mat Map1;
    cv::Mat Map2;

    //-------------------------------------------------------------------------

    std::vector<cv::Mat> CapturedPointSets;
    std::vector<cv::Point3f> ObjectPoints;
    int32 FramesCollected;
    int32 StepCountsCollected;

    //-------------------------------------------------------------------------

    FIntPoint Resolution;
    FVector2D SensorSize;
    FVector2D ZoomRange;

    //-------------------------------------------------------------------------

    FString CalibType;

    //-------------------------------------------------------------------------

    FViveVideoCameraProperties CameraProperties;
};
