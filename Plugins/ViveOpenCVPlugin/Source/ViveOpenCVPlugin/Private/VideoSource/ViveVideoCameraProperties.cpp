#include "VideoSource/ViveVideoCameraProperties.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveUtilitiesHelper.h"
#include "ViveDeveloperSettings.h"
#include "ViveLog.h"

void FViveVideoCameraProperties::PerformCalibMatrixValues( const FIntPoint& InResolution, const FIntPoint& InFrameSize, const FVector2D& InSensorSize )
{
    cv::Mat mat;
    RecalculateMatrix( InResolution, InFrameSize, mat );

    double fovx, fovy;
    cv::calibrationMatrixValues( mat, cv::Size( InResolution.X, InResolution.Y ), InSensorSize.X, InSensorSize.Y, 
        fovx, fovy, FocalLength, PrincipalPoint, AspectRatio );

    FOV.X = (float)fovx;
    FOV.Y = (float)fovy;

    VIVELOG( Log, TEXT( "============================================================================" ) );
    VIVELOG( Log, TEXT( "#### Result aspect ratio: %f ####" ), (float)AspectRatio );
    VIVELOG( Log, TEXT( "#### Result focal length(MM): %f #### " ), (float)FocalLength );
    VIVELOG( Log, TEXT( "#### Result principal point(MM): %f X %f ####" ), (float)PrincipalPoint.x, (float)PrincipalPoint.y );
    VIVELOG( Log, TEXT( "#### Result field of view: X(%f) Y(%f) ####" ), FOV.X, FOV.Y );
    VIVELOG( Log, TEXT( "============================================================================" ) );
}

void FViveVideoCameraProperties::RecalculateMatrix( const FIntPoint& InResolution, const FIntPoint& InFrameSize, cv::Mat& OutMatrix )
{
    OutMatrix = CameraMatrix.clone();
    OutMatrix.at<double>(0, 0) = (double)InResolution.X * (CameraMatrix.at<double>(0, 0) / (double)InFrameSize.X);
    OutMatrix.at<double>(1, 1) = (double)InResolution.Y * (CameraMatrix.at<double>(1, 1) / (double)InFrameSize.Y);
    OutMatrix.at<double>(0, 2) = (double)InResolution.X * (CameraMatrix.at<double>(0, 2) / (double)InFrameSize.X);
    OutMatrix.at<double>(1, 2) = (double)InResolution.Y * (CameraMatrix.at<double>(1, 2) / (double)InFrameSize.Y);

    VIVELOG( Log, TEXT( "#### Original matrix: %f, %f, %f, %f ####" ), 
        CameraMatrix.at<double>(0,0), CameraMatrix.at<double>(1, 1), CameraMatrix.at<double>(0, 2), CameraMatrix.at<double>(1, 2) );
    VIVELOG( Log, TEXT( "#### Output matrix: %f, %f, %f, %f ####" ), 
        OutMatrix.at<double>(0,0), OutMatrix.at<double>(1, 1), OutMatrix.at<double>(0, 2), OutMatrix.at<double>(1, 2) );
}

void FViveVideoCameraProperties::StoreLensCalibInfo( float InZoomMin, float InZoomMax, const FVector2D& InSensorSize )
{
    auto minFocal = (double)InZoomMin;
    auto maxFocal = (double)InZoomMax;

    auto focalLength = FocalLength < minFocal ? minFocal : FocalLength > maxFocal ? maxFocal : FocalLength;
    focalLength = std::round( focalLength );

    auto zoomLevel = (focalLength - minFocal) / (maxFocal - minFocal);

    VIVELOG( Log, TEXT( "============================================================================" ) );
    VIVELOG( Log, TEXT( "#### Store Lens Calibration Info ####" ) );
    VIVELOG( Log, TEXT( "#### Focal length(MM): %f ####" ), focalLength );
    VIVELOG( Log, TEXT( "#### Zoom level: %f ####" ), zoomLevel );

    LensCalibInfo lensCalibInfo;
    lensCalibInfo.ZoomLevel = zoomLevel;
    lensCalibInfo.PrincipalPoint = cv::Point2d( PrincipalPoint.x / (double)InSensorSize.X, PrincipalPoint.y / (double)InSensorSize.Y );
    lensCalibInfo.K1 = DistortionCoefficients.at<double>(0);
    lensCalibInfo.K2 = DistortionCoefficients.at<double>(1);
    lensCalibInfo.P1 = DistortionCoefficients.at<double>(2);
    lensCalibInfo.P2 = DistortionCoefficients.at<double>(3);
    lensCalibInfo.K3 = DistortionCoefficients.cols >= 5 ? DistortionCoefficients.at<double>(4) : 0.0;
    LensCalibInfos.Add( lensCalibInfo );

    VIVELOG( Log, TEXT( "#### Principal point: %f X %f ####" ), lensCalibInfo.PrincipalPoint.x, lensCalibInfo.PrincipalPoint.y );
    VIVELOG( Log, TEXT( "#### Distortion Coeffic: K1(%f) K2(%f) P1(%f) P2(%f) K3(%f) ####" ), 
        lensCalibInfo.K1, lensCalibInfo.K2, lensCalibInfo.P1, lensCalibInfo.P2, lensCalibInfo.K3 );
    VIVELOG( Log, TEXT( "============================================================================" ) );
}

bool FViveVideoCameraProperties::SavePoseCalibToFile( const FString& InFilename, const FIntPoint& InResolution, cv::Vec3d InRot, cv::Vec3d InTrans,
    FVector2D InFOV, double InFocalLength, double InAspectRatio )
{
    // Declaration const variables.
    const cv::Mat_<double> REBASE_CV_TO_UE = 
        (cv::Mat_<double>(3, 3) << 
            0.0, 1.0, 0.0, 
            1.0, 0.0, 0.0, 
            0.0, 0.0, 1.0
        );
    const cv::Mat_<double> REBASE_UE_TO_CV = REBASE_CV_TO_UE.t();

    // Convert OpenCV to Unreal.
    cv::Mat_<double> rotMat;
    cv::Rodrigues( InRot, rotMat );
    cv::Mat_<double> invRot( rotMat.t() );

    cv::Mat_<double> locMat(3, 1);
    locMat.at<double>(0, 0) = InTrans[0];
    locMat.at<double>(1, 0) = InTrans[1];
    locMat.at<double>(2, 0) = InTrans[2];

    cv::Mat_<double> negInvRot = (-1.0 * invRot);
    cv::Mat locCVToUE = REBASE_CV_TO_UE * (negInvRot * locMat);
    cv::Mat_<double> rotCVToUE = REBASE_CV_TO_UE * invRot * REBASE_UE_TO_CV;
    VIVELOG( Log, TEXT( "CV to UE Rotation:\n\t[%f, %f, %f]\n\t[%f, %f, %f]\n\t[%f, %f, %f]" ), 
        rotCVToUE.at<double>(0, 0), rotCVToUE.at<double>(0, 1), rotCVToUE.at<double>(0, 2), 
        rotCVToUE.at<double>(1, 0), rotCVToUE.at<double>(1, 1), rotCVToUE.at<double>(1, 2), 
        rotCVToUE.at<double>(2, 0), rotCVToUE.at<double>(2, 1), rotCVToUE.at<double>(2, 2) );
    VIVELOG( Log, TEXT( "CV to UE Location:\n\t[%f, %f, %f]" ), 
        locCVToUE.at<double>(0, 0), locCVToUE.at<double>(1, 0), locCVToUE.at<double>(2, 0) );

    // Save the calibration data.
    auto settings = FViveUtilitiesHelper::GetSettings();
    const auto backupOutputPath = FViveUtilitiesHelper::GenerateVideoCameraCalibOutputPath( settings->OutputCalibDirName );
    FString outputPath;
    if ( FViveUtilitiesHelper::ValidateFilePath( outputPath, backupOutputPath, InFilename, TEXT( "txt" ), false ) ) {
        VIVELOG( Log, TEXT( "#### Save camera pose calibration data. ####" ) );

        auto calibData = FString::Printf( TEXT( "FocalLength:%f\n\rFovX:%f\n\rFovY:%f\n\rAspectRatio:%f\n\r"
            "Location:%f,%f,%f\n\rRotation:%f,%f,%f,%f,%f,%f,%f,%f,%f\n\rResolution:%d,%d\n\r" ), 
            InFocalLength, InFOV.X, InFOV.Y, InAspectRatio, 
            locCVToUE.at<double>(0, 0), locCVToUE.at<double>(1, 0), locCVToUE.at<double>(2, 0), 
            rotCVToUE.at<double>(0, 0), rotCVToUE.at<double>(0, 1), rotCVToUE.at<double>(0, 2), 
            rotCVToUE.at<double>(1, 0), rotCVToUE.at<double>(1, 1), rotCVToUE.at<double>(1, 2), 
            rotCVToUE.at<double>(2, 0), rotCVToUE.at<double>(2, 1), rotCVToUE.at<double>(2, 2), 
            InResolution.X, InResolution.Y );

        auto successed = FViveUtilitiesHelper::WriteTextFile( outputPath, calibData );

        CVIVELOG( successed, Log, TEXT( "#### Success save file. camera pose calibration data ####\n\r%s" ), *calibData );
        return successed;
    }

    return false;
}

void FViveVideoCameraProperties::SaveLensCalibToFile( const FString& InFilename, const FVector2D& InZoomRange )
{
    // Save the calibration data.
    auto settings = FViveUtilitiesHelper::GetSettings();
    const auto backupOutputPath = FViveUtilitiesHelper::GenerateVideoCameraCalibOutputPath( settings->OutputCalibDirName );
    FString outputPath;
    if ( FViveUtilitiesHelper::ValidateFilePath( outputPath, backupOutputPath, InFilename, TEXT( "txt" ), false ) ) {
        VIVELOG( Log, TEXT( "#### Save camera lens calibration data. ####" ) );

        auto calibData = FString::Printf( TEXT( "MinFocalLength:%f\n\rMaxFocalLength:%f\n\rResultNum:%d\n\r" ), 
            InZoomRange.X, InZoomRange.Y, LensCalibInfos.Num() );
        for ( auto& info : LensCalibInfos ) {
            auto infoContent = FString::Printf( TEXT( "ZoomLevel:%f\n\rPrincipalPoint:%f,%f\n\r"
                "DistortionCoefficients(K1, K2, P1, P2, K3):%f,%f,%f,%f,%f\n\r" ), 
                info.ZoomLevel, info.PrincipalPoint.x, info.PrincipalPoint.y, 
                info.K1, info.K2, info.P1, info.P2, info.K3 );

            calibData += infoContent;
        }

        auto successed = FViveUtilitiesHelper::WriteTextFile( outputPath, calibData );

        CVIVELOG( successed, Log, TEXT( "#### Success save file. camera lens calibration data ####\n\r%s" ), *calibData );
    }
}
