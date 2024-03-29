// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "VideoDriver/ViveOpenCV.h"
#include "CoreMinimal.h"

class VIVEOPENCVPLUGIN_API FViveVideoCameraProperties final
{
public:
    FViveVideoCameraProperties() = default;

    void PerformCalibMatrixValues( const FIntPoint& InResolution, const FIntPoint& InFrameSize, const FVector2D& InSensorSize );
    void StoreLensCalibInfo( float InZoomMin, float InZoomMax, const FVector2D& InSensorSize );

public:
    // Estimate lens.
    struct LensCalibInfo
    {
        double ZoomLevel;
        cv::Point2d PrincipalPoint;
        double K1;
        double K2;
        double P1;
        double P2;
        double K3;
    };
public:
    static bool SavePoseCalibToFile( const FString& InFilename, const FIntPoint& InResolution, cv::Vec3d InRot, cv::Vec3d InTrans, 
        FVector2D InFOV, double InFocalLength, double InAspectRatio );
    static bool SaveLensCalibToFile( const FString& InFilename, const FVector2D& InZoomRange, TArray<LensCalibInfo> InLensCalibInfo );

private:
    void RecalculateMatrix( const FIntPoint& InResolution, const FIntPoint& InFrameSize, cv::Mat& OutMatrix );

public:
    /*
     * Camera intrinsic matrix in form:
       f_x,     0,  center_x;
         0,   f_y,  center_y;
         0,     0,         1;
    **/
    cv::Mat_<double> CameraMatrix;
    cv::Mat_<double> DistortionCoefficients;

    //-------------------------------------------------------------------------

    // Estimate pose.
    cv::Vec3d RotVec;
    cv::Vec3d TranVec;

    // Estimate lens.
    TArray<LensCalibInfo> LensCalibInfos;

    //-------------------------------------------------------------------------

    double FocalLength;
    double AspectRatio;
    cv::Point2d PrincipalPoint;
    FVector2D FOV;
};
