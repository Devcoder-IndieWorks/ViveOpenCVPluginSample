#include "VideoDriver/ViveVideoDriver.h"
#include "ViveStudiosUtilsPCH.h"
#include "VideoDriver/ViveVideoFrame.h"
#include "VideoSource/ViveVideoSource.h"
#include "ViveLog.h"

UViveVideoDriver::FViveVideoDriverInstanceChange UViveVideoDriver::OnVideoDriverInstanceChange;
UViveVideoDriver* UViveVideoDriver::GlobalDriver = nullptr;

UViveVideoDriver::UViveVideoDriver( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    IsActive = false;
    CalibrationInProgress = false;
    EstimatePoseInProgress = false;
    ModeType = EMode::MODE_Detection;
}

void UViveVideoDriver::Initialize()
{
    if ( ensure( AvailableVideoSource ) ) {
        IsActive = true;
        VideoSourceInstance = NewObject<UViveVideoSource>( this, AvailableVideoSource );
        VideoSourceInstance->SetupConfigurations();

        UViveVideoDriver::GlobalDriver = this;
        UViveVideoDriver::OnVideoDriverInstanceChange.Broadcast( UViveVideoDriver::GlobalDriver );
    }
}

void UViveVideoDriver::Shutdown()
{
    IsActive = false;
    UViveVideoDriver::GlobalDriver = nullptr;
    UViveVideoDriver::OnVideoDriverInstanceChange.Broadcast( UViveVideoDriver::GlobalDriver );
}

bool UViveVideoDriver::OpenVideoSourceDefault()
{
    VIVELOG( Log, TEXT( "#### Default video configuration name: [%s] ####" ), *DefaultVideoConfigName );
    if ( ensure( OpenVideoSourceByName( DefaultVideoConfigName ) ) )
        return true;

    return false;
}

bool UViveVideoDriver::OpenVideoSourceByName( const FString& InVideoConfigName )
{
    if ( VideoSourceInstance != nullptr ) {
        for ( auto& config : VideoSourceInstance->Configurations ) {
            VIVELOG( Log, TEXT( "####    Video configuration name: [%s] ####" ), *(config.Identifier) );
            if ( config.Identifier == InVideoConfigName ) {
                DefaultVideoConfigName = InVideoConfigName;
                OpenVideoSource( config );
                return true;
            }
        }
    }

    return false;
}

void UViveVideoDriver::OpenVideoSource( const FViveVideoConfiguration& InVideoConfig )
{
    unimplemented();
}

bool UViveVideoDriver::IsConnected() const
{
    return (VideoSourceInstance != nullptr) ? VideoSourceInstance->IsConnected() : false;
}

void UViveVideoDriver::Tick()
{
    if ( ensure( IsActive ) )
        WriteFrameToTexture();
}

void UViveVideoDriver::WriteFrameToTexture()
{
    // The general code for updating UE's dynamic texture.
    ENQUEUE_RENDER_COMMAND( UpdateTextureRenderCommand )(
        [this]( FRHICommandListImmediate& InRHICmdList ){
            if (  TextureUpdateParam.Texture2DResource != nullptr 
               && TextureUpdateParam.Texture2DResource->GetCurrentFirstMip() <= 0 
               && TextureUpdateParam.Texture2DResource->GetTexture2DRHI().IsValid() ){
                // 새로운 Capture frame image가 있다면 texure에 다시 그린다.
                auto driver = TextureUpdateParam.Driver;
                if ( driver != nullptr && driver->IsNewFrameAvailable() ) {
                    auto newFrame = driver->GetFrame();
                    if ( ensure( newFrame != nullptr ) ) {
                        RHIUpdateTexture2D( TextureUpdateParam.Texture2DResource->GetTexture2DRHI(), 0, 
                            TextureUpdateParam.RegionDefinition, sizeof(FColor) * TextureUpdateParam.RegionDefinition.Width, 
                            newFrame->GetRawData() );
                    }
                }
            }
        }
    );
}

bool UViveVideoDriver::IsNewFrameAvailable() const
{
    unimplemented();
    return false;
}

FViveVideoFrame* UViveVideoDriver::GetFrame()
{
    unimplemented();
    return nullptr;
}

UTexture2D* UViveVideoDriver::GetOutputTexture() const
{
    return OutputTexture;
}

UViveVideoSource* UViveVideoDriver::GetVideoSource() const
{
    return VideoSourceInstance;
}

bool UViveVideoDriver::IsCalibrated() const
{
    return (ModeType == EMode::MODE_Calibrated);
}

bool UViveVideoDriver::IsCalibrationInProgress() const
{
    return CalibrationInProgress;
}

float UViveVideoDriver::GetCalibrationProgress() const
{
    return 0.0f;
}

void UViveVideoDriver::StartCalibration()
{
    unimplemented();
}

void UViveVideoDriver::CancelCalibration()
{
    unimplemented();
}

void UViveVideoDriver::StartEstimatePose()
{
    unimplemented();
}

void UViveVideoDriver::StopEstimatePose()
{
    unimplemented();
}

bool UViveVideoDriver::IsEstimatePoseProgress() const
{
    return EstimatePoseInProgress;
}

void UViveVideoDriver::ContinueCapturing()
{
    unimplemented();
}

FIntPoint UViveVideoDriver::GetResolution() const
{
    unimplemented();
    return FIntPoint( 0, 0 );
}

FVector2D UViveVideoDriver::GetFieldOfView() const
{
    unimplemented();
    return FVector2D::ZeroVector;
}

void UViveVideoDriver::SavePoseCalibToFile()
{
    unimplemented();
}

void UViveVideoDriver::SaveLensCalibToFile()
{
    unimplemented();
}

void UViveVideoDriver::GetCameraParameters( FIntPoint& OutCameraResolution, FVector2D& OutFieldOfView ) const
{
    OutCameraResolution = GetResolution();
    OutFieldOfView = GetFieldOfView();
}

void UViveVideoDriver::SetFrameResolution( const FIntPoint& InNewResolution )
{
    if ( ensure( InNewResolution.GetMin() > 0 ) ) {
        VIVELOG( Log, TEXT( "#### Create output UTexture2D. Resolution Width:[%d] Height:[%d] ####" ), InNewResolution.X, InNewResolution.Y );
        OutputTexture = UTexture2D::CreateTransient( InNewResolution.X, InNewResolution.Y );
        OutputTexture->UpdateResource();

        FUpdateTextureRegion2D wholeTextureRegion;
        // Offset in source image data: we map the video data 1:1 to the texture
        wholeTextureRegion.SrcX = 0;
        wholeTextureRegion.SrcY = 0;

        // Offset in texture: we map video data 1:1 to the texture
        wholeTextureRegion.DestX = 0;
        wholeTextureRegion.DestY = 0;

        // Size of the updated region equals to the size of video image
        wholeTextureRegion.Width = InNewResolution.X;
        wholeTextureRegion.Height = InNewResolution.Y;

        TextureUpdateParam.Texture2DResource = (FTexture2DResource*)OutputTexture->Resource;
        TextureUpdateParam.RegionDefinition = wholeTextureRegion;
        TextureUpdateParam.Driver = this;
    }
}

FString UViveVideoDriver::ModeTypeToString() const
{
    switch( ModeType ) {
    case EMode::MODE_Detection:
        return TEXT( "Detection" );
    case EMode::MODE_Capturing:
        return TEXT( "Capturing" );
    case EMode::MODE_Calibrated:
        return TEXT( "Calibrated" );
    }
    return TEXT( "INVALID" );
}

void UViveVideoDriver::SetModeTypeByString( const FString& InModeType )
{
    if ( InModeType.Equals( TEXT( "Detection" ) ) )
        ModeType = EMode::MODE_Detection;
    else if ( InModeType.Equals( TEXT( "Capturing" ) ) )
        ModeType = EMode::MODE_Capturing;
    else if ( InModeType.Equals( TEXT( "Calibrated" ) ) )
        ModeType = EMode::MODE_Calibrated;
    else
        VIVELOG( Error, TEXT( "Invalid mode type." ) );
}

FString UViveVideoDriver::GetDriverMessage() const
{
    return Message;
}

void UViveVideoDriver::SetDriverMessage( const FString& InMessage )
{
    Message = InMessage;
}

UViveVideoDriver* UViveVideoDriver::GetGlobalDriver()
{
    return UViveVideoDriver::GlobalDriver;
}
