#include "VideoSource/ViveVideoSource.h"
#include "ViveStudiosUtilsPCH.h"

UViveVideoSource::UViveVideoSource( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

void UViveVideoSource::SetupConfigurations()
{
    for ( auto& config : Configurations ) {
        config.SetupIdentifierAndDisplayName( GetIdentifier(), GetSourceName(), 
            FString::Printf( TEXT( "%s:%dx%d" ), 
                config.CalibrationType == EViveCalibType::CT_Pose ? TEXT( "Pose" ) : TEXT( "Lens" ), 
                (int32)config.ResolutionWidth, (int32)config.ResolutionHeight ) );
    }
}

const FViveVideoConfiguration& UViveVideoSource::GetCurrentConfiguration() const
{
    return CurrentConfiguration;
}

FIntPoint UViveVideoSource::GetResolution() const
{
    unimplemented();
    return FIntPoint(0, 0);
}

bool UViveVideoSource::Connect( const FViveVideoConfiguration& InConfiguration )
{
    CurrentConfiguration = InConfiguration;
    return false;
}

bool UViveVideoSource::IsConnected() const
{
    return false;
}

void UViveVideoSource::Disconnect()
{
    unimplemented();
}

bool UViveVideoSource::GetNextFrame( cv::Mat_<cv::Vec3b>& OutFrame )
{
    unimplemented();
    return false;
}

float UViveVideoSource::GetFrequency() const
{
    unimplemented();
    return 0.0f;
}

FString UViveVideoSource::GetSourceName() const
{
    unimplemented();
    return TEXT( "INVALID" );
}

FString UViveVideoSource::GetIdentifier() const
{
    unimplemented();
    return TEXT( "INVALID" );
}
