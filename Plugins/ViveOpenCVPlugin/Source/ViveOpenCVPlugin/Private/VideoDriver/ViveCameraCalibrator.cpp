#include "VideoDriver/ViveCameraCalibrator.h"
#include "VideoDriver/ViveVideoDriver.h"
#include "VideoSource/ViveVideoConfiguration.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveUtilitiesHelper.h"
#include "ViveLog.h"

UViveCameraCalibrator::UViveCameraCalibrator( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

void UViveCameraCalibrator::SetupPredefinedDictionary( cv::aruco::PREDEFINED_DICTIONARY_NAME InName )
{
    cv::vive_studios::SetupPredefinedDictionary( InName );
}

void UViveCameraCalibrator::Shutdown()
{
    cv::vive_studios::ResetPredefinedDictionary();
}

void UViveCameraCalibrator::Reset( const FViveVideoConfiguration* InVideoConfig )
{
    LastFrameTime = 0.0f;
    FramesCollected = 0;
    StepCountsCollected = 0;
    MustInitUndistortRemap = true;

    if ( CapturedPointSets.size() > 0 )
        CapturedPointSets.clear();

    if ( ObjectPoints.size() > 0 )
        ObjectPoints.clear();

    if ( InVideoConfig != nullptr ) {
        MinInterval      = InVideoConfig->CaptureDelay;
        FramesNeeded     = InVideoConfig->MaxCaptureFrames;
        StepCountsNeeded = InVideoConfig->MaxCaptureCount;
        PatternSize      = cv::Size( InVideoConfig->BoardSizeWidth, InVideoConfig->BoardSizeHeight );
        WinSize          = cv::Size( InVideoConfig->WindowsSize, InVideoConfig->WindowsSize );
        SquareSize       = InVideoConfig->BoardSquareSize;
        MarkerSize       = InVideoConfig->MarkerSize;
        CalibType        = InVideoConfig->CalibrationType == EViveCalibType::CT_Pose ? TEXT( "Pose" ) : TEXT( "Lens" );
        Resolution       = FIntPoint( (int32)InVideoConfig->ResolutionWidth, (int32)InVideoConfig->ResolutionHeight );
        SensorSize       = FVector2D( InVideoConfig->SensorWidth, InVideoConfig->SensorHeight );
        ZoomRange        = FVector2D( InVideoConfig->MinZoom, InVideoConfig->MaxZoom );
	    
        CalibrationFlags = 0;
        if ( InVideoConfig->FixPrincipalPointAtTheCenter )
            CalibrationFlags |= cv::CALIB_FIX_PRINCIPAL_POINT;
        if ( InVideoConfig->AssumeZeroTangentialDistortion )
            CalibrationFlags |= cv::CALIB_ZERO_TANGENT_DIST;
        if ( InVideoConfig->FixK1 )
            CalibrationFlags |= cv::CALIB_FIX_K1;
        if ( InVideoConfig->FixK2 )
            CalibrationFlags |= cv::CALIB_FIX_K2;
        if ( InVideoConfig->FixK3 )
            CalibrationFlags |= cv::CALIB_FIX_K3;

        ObjectPoints.reserve(1);
        for ( int32 col = 0; col < PatternSize.height; ++col ) {
            for ( int32 row = 0; row < PatternSize.width; ++row ) {
                ObjectPoints.push_back( cv::Point3f( 
                    float( row ) * SquareSize, 
                    float( col ) * SquareSize, 
                    0.0f ) );
            }
        }

        float gridWidth = SquareSize * (PatternSize.width - 1);
        ObjectPoints[PatternSize.width - 1].x = ObjectPoints[0].x + gridWidth; 
    }
}

bool UViveCameraCalibrator::IsFinished() const
{
    return FramesCollected == FramesNeeded;
}

bool UViveCameraCalibrator::IsStepFinished() const
{
    return StepCountsCollected == StepCountsNeeded;
}

float UViveCameraCalibrator::GetProgressRatio() const
{
    return float( FramesCollected ) / float( FramesNeeded );
}

const FViveVideoCameraProperties& UViveCameraCalibrator::GetVideoCameraProperties() const
{
    return CameraProperties;
}

bool UViveCameraCalibrator::ProcessFrame( cv::Mat& InOutFrame, const FString& InModeType, float InTimeNow, UViveVideoDriver* InDriver )
{
    if ( InModeType.Equals( TEXT( "Capturing" ) ) ) {
        cv::Mat grayFrame;
        cv::cvtColor( InOutFrame, grayFrame, cv::COLOR_RGB2GRAY );
	    
        bool blinkOutput = false;
        cv::Mat newCalibPoints;
        int32 chessboardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;
        bool found = cv::findChessboardCorners( grayFrame, PatternSize, newCalibPoints, chessboardFlags );
        if ( found ) {
            cv::cornerSubPix( grayFrame, newCalibPoints, WinSize, cv::Size( -1, -1 ), 
                cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001f) );
	    
            if ( StepCountsCollected <= StepCountsNeeded && InTimeNow >= (LastFrameTime + MinInterval) ) {
                if ( FramesCollected < FramesNeeded ) {
                    CapturedPointSets.push_back( newCalibPoints );
                    blinkOutput = true;
                    ++StepCountsCollected;
                    ++FramesCollected;
                    auto msg = FString::Printf( TEXT( "Captured frames: %d/%d" ), FramesCollected, FramesNeeded );
                    InDriver->SetDriverMessage( msg );
                    VIVELOG( Log, TEXT( "#### %s ####" ), *msg );
                }
                LastFrameTime = InTimeNow;
            }
	    
            cv::drawChessboardCorners( InOutFrame, PatternSize, newCalibPoints, found );
	    
            if ( blinkOutput )
                cv::bitwise_not( InOutFrame, InOutFrame );
        }

        if ( FramesCollected >= FramesNeeded ) {
            VIVELOG( Log, TEXT( "#### Begin calculate calibration. ####" ) );
            auto frameSize = InOutFrame.size();
            CalculateCalibration( frameSize );
            InDriver->SetDriverMessage( TEXT( "Finished calibration." ) );
            VIVELOG( Log, TEXT( "#### End calculate calibration. ####" ) );
        }
        else if ( StepCountsCollected >= StepCountsNeeded ) {
            InDriver->SetDriverMessage( TEXT( "Paused. Press 'Continue Button'." ) );
            InDriver->SetModeTypeByString( TEXT( "Detection" ) );
            StepCountsCollected = 0;
        }

        return true;
    }

    return false;
}

bool UViveCameraCalibrator::RectifyMap( cv::Mat& InOutFrame, const FString& InModeType )
{
    if ( InModeType.Equals( TEXT( "Calibrated" ) ) ) {
        auto tmpFrame = InOutFrame.clone();
        if ( MustInitUndistortRemap ) {
            auto frameSize = InOutFrame.size();
            cv::initUndistortRectifyMap( CameraProperties.CameraMatrix, CameraProperties.DistortionCoefficients, cv::Mat::eye( 3, 3, CV_64F ), 
                cv::getOptimalNewCameraMatrix( CameraProperties.CameraMatrix, CameraProperties.DistortionCoefficients, frameSize, 1.0 ), 
                frameSize, CV_32FC1, Map1, Map2 );

            MustInitUndistortRemap = false;
            VIVELOG( Log, TEXT( "#### Initialize undistort rectify map. ####" ) );
        }

        cv::remap( tmpFrame, InOutFrame, Map1, Map2, cv::INTER_LINEAR );
        return true;
    }

    return false;
}

void UViveCameraCalibrator::CalculateCalibration( cv::Size InFrameSize )
{
    std::vector<std::vector<cv::Point3f>> capturedPoints(1);
    capturedPoints[0] = ObjectPoints;
    capturedPoints.resize( CapturedPointSets.size(), capturedPoints[0] );

    // 카메라 컬리브레이션 값을 계산 함.
    cv::setIdentity( CameraProperties.CameraMatrix );
    CameraProperties.DistortionCoefficients.setTo(0, 0);
    auto calibrationError = cv::calibrateCamera( capturedPoints, CapturedPointSets, InFrameSize, 
        CameraProperties.CameraMatrix, CameraProperties.DistortionCoefficients, cv::noArray(), 
        cv::noArray(), CalibrationFlags | cv::CALIB_USE_LU | cv::CALIB_FIX_ASPECT_RATIO );
    VIVELOG( Log, TEXT( "#### Calibration finished, error: %f ####" ), calibrationError );

    CameraProperties.PerformCalibMatrixValues( Resolution, FIntPoint( InFrameSize.width, InFrameSize.height ), SensorSize );

    if ( CalibType.Equals( TEXT( "Lens" ) ) )
        CameraProperties.StoreLensCalibInfo( ZoomRange.X, ZoomRange.Y, SensorSize );
}

void UViveCameraCalibrator::EstimatePoseSingleMarker( cv::Mat& InOutFrame )
{
    cv::vive_studios::EstimagePoseSingleMarker( MarkerSize, CameraProperties.CameraMatrix, CameraProperties.DistortionCoefficients, true, true, 
        InOutFrame, CameraProperties.RotVec, CameraProperties.TranVec );
}

void UViveCameraCalibrator::SavePoseCalibToFile( const FString& InFilename )
{
    CameraProperties.SavePoseCalibToFile( InFilename, Resolution );
}

void UViveCameraCalibrator::SaveLensCalibToFile( const FString& InFilename )
{
    CameraProperties.SaveLensCalibToFile( InFilename, ZoomRange );
}
