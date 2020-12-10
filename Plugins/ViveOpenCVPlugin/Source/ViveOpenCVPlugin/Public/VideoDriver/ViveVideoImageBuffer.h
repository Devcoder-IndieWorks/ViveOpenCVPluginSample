// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "ViveOpenCV.h"
#include "ViveCondition.h"

class VIVEOPENCVPLUGIN_API FViveVideoImageBuffer
{
public:
    FViveVideoImageBuffer( bool InShowLog = false );
    ~FViveVideoImageBuffer();

    void InFrameImage( const cv::Mat_<cv::Vec3b>& InFrame );
    bool OutFrameImage( cv::Mat_<cv::Vec3b>& OutFrame );

private:
    TQueue<cv::Mat_<cv::Vec3b>> ImageDatas;

    TSharedPtr<FViveCondition> ProductCondition;
    TSharedPtr<FViveCondition> ConsumeCondition;
    FCriticalSection CriticalSection;

    bool ShowLog;
};
