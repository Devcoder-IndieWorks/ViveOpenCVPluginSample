#include "VideoSource/ViveVideoCameraCapture.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveLog.h"

UViveVideoCameraCapture::UViveVideoCameraCapture( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

FIntPoint UViveVideoCameraCapture::GetResolution() const
{
    FIntPoint cameraResolution;
    cameraResolution.X = FPlatformMath::RoundToInt( Capture.get( cv::CAP_PROP_FRAME_WIDTH ) );
    cameraResolution.Y = FPlatformMath::RoundToInt( Capture.get( cv::CAP_PROP_FRAME_HEIGHT ) );
    return cameraResolution;
}

bool UViveVideoCameraCapture::Connect( const FViveVideoConfiguration& InConfiguration )
{
    Super::Connect( InConfiguration );

    auto cameraId = CurrentConfiguration.CameraId;
    Capture.open( cameraId);

    if ( !IsConnected() ) {
        VIVELOG( Error, TEXT( "Failed to open camera: [%d]." ), cameraId );
        return false;
    }

    VIVELOG( Log, TEXT( "Connected to camera: [%d]." ), cameraId );
    if ( FMath::Min( CurrentConfiguration.WindowFrameWidth, CurrentConfiguration.WindosFrameHeight ) > 0 ) {
        Capture.set( cv::CAP_PROP_FRAME_WIDTH, CurrentConfiguration.WindowFrameWidth );
        Capture.set( cv::CAP_PROP_FRAME_HEIGHT, CurrentConfiguration.WindosFrameHeight );
    }
    Capture.set( cv::CAP_PROP_AUTOFOCUS, 0 );

    return true;
}

bool UViveVideoCameraCapture::IsConnected() const
{
    return Capture.isOpened();
}

void UViveVideoCameraCapture::Disconnect()
{
    if ( IsConnected() )
        Capture.release();
}

bool UViveVideoCameraCapture::GetNextFrame( cv::Mat_<cv::Vec3b>& OutFrame )
{
    return Capture.read( OutFrame );
}

float UViveVideoCameraCapture::GetFrequency() const
{
    return Capture.get( cv::CAP_PROP_FPS );
}

FString UViveVideoCameraCapture::GetSourceName() const
{
    return FString::Printf( TEXT( "Camera %d" ), CurrentConfiguration.CameraId );
}

FString UViveVideoCameraCapture::GetIdentifier() const
{
    return FString::Printf( TEXT( "RealCamera_%d" ), CurrentConfiguration.CameraId );
}
