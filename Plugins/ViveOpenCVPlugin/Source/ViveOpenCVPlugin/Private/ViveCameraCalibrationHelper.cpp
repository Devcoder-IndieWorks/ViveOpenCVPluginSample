#include "ViveCameraCalibrationHelper.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveCameraCalibInfo.h"
#include "ViveLensCalibInfo.h"
#include "ViveDeveloperSettings.h"
#include "ViveDistortionCorrectionMapShader.h"
#include "ViveUtilitiesHelper.h"
#include "Kismet/KismetmathLibrary.h"
#include "TextureResource.h"
#include "RHIUtilities.h"
#include "RHIStaticStates.h"
#include "PixelShaderUtils.h"
#include "RenderTargetPool.h"
#include "ImagePixelData.h"
#include "ViveLog.h"

FViveCameraCalibInfo* UViveCameraCalibrationHelper::CameraCalibInfo = nullptr;
FViveLensCalibInfo* UViveCameraCalibrationHelper::LensCalibInfo = nullptr;

bool UViveCameraCalibrationHelper::GetCalibratedViewTransform( FTransform& OutTransform )
{
    if ( ensure( CameraCalibInfo != nullptr ) ) {
        auto quat = FVector4( 0.0f, 0.0f, 1.0f, PI );
        auto additionRotation = UKismetMathLibrary::MakeTransform( FVector( 0.0f, 0.0f, 0.0f ), quat.ToOrientationRotator(), FVector( 1.0f, 1.0f, 1.0f ) );
        auto calibratedTransform = CameraCalibInfo->GetTransform();
        OutTransform = additionRotation * calibratedTransform;
        VIVELOG( Log, TEXT( "#### Rotation: [%s] ####" ), *(OutTransform.Rotator().ToString()) );
        VIVELOG( Log, TEXT( "#### Location: [%s] ####" ), *(OutTransform.GetLocation().ToString()) );

        return true;
    }

    return false;
}

bool UViveCameraCalibrationHelper::GetCameraFocalLength( float& OutFocalLength )
{
    if ( ensure( CameraCalibInfo != nullptr ) ) {
        OutFocalLength = CameraCalibInfo->GetFocalLength();
        return true;
    }

    return false;
}

bool UViveCameraCalibrationHelper::GetCameraFovX( float& OutFovX )
{
    if ( ensure( CameraCalibInfo != nullptr ) ) {
        OutFovX = CameraCalibInfo->GetFovX();
        return true;
    }

    return false;
}

bool UViveCameraCalibrationHelper::GetCameraFovY( float& OutFovY )
{
    if ( ensure( CameraCalibInfo != nullptr ) ) {
        OutFovY = CameraCalibInfo->GetFovY();
        return true;
    }

    return false;
}

bool UViveCameraCalibrationHelper::GetCameraAspectRatio( float& OutAspectRatio )
{
    if ( ensure( CameraCalibInfo != nullptr ) ) {
        OutAspectRatio = CameraCalibInfo->GetAspectRatio();
        return true;
    }

    return false;
}

bool UViveCameraCalibrationHelper::LoadCameraCalibInfo( const FString& InFilename )
{
    auto settings = FViveUtilitiesHelper::GetSettings();
    const auto backupOutputPath = FViveUtilitiesHelper::GenerateVideoCameraCalibOutputPath( settings->OutputCalibDirName );
    FString filePath;
    if ( FViveUtilitiesHelper::ValidateFilePath( filePath, backupOutputPath, InFilename, TEXT( "txt" ), false ) ) {
        VIVELOG( Log, TEXT( "#### Load camera calibration infomation from Save data. ####" ) );
        CameraCalibInfo->ReadFromFile( filePath );
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void UViveCameraCalibrationHelper::GenerateDistortionCorrectionMap( float InFocalLength )
{
    LensCalibInfo->IsSetPixelData = false;

    auto settings = FViveUtilitiesHelper::GetSettings();
    if ( settings->OutputMapResolution.X <= 3 || settings->OutputMapResolution.Y <= 3 ) {
        VIVELOG( Error, TEXT( "Cannot generate distortion correction map, the map resolution member is <= 3 pixels on the X or Y axis." ) );
        return;
    }

    static const FString backupOutputMapPath = FViveUtilitiesHelper::GenerateDistortionCorrectionMapOutputPath( settings->OutputMapDirName );
    FString outputMapPath;
    if ( settings->SaveDistortionCorrectionMap ) {
        outputMapPath = settings->OutputMapFullPath;
        if ( !FViveUtilitiesHelper::ValidateFilePath( outputMapPath, backupOutputMapPath, TEXT( "DistortionCorrectionMap" ), TEXT( "exr" ) ) ) {
            VIVELOG( Error, TEXT( "Cannot generate distortion correction map, unable to create folder path: \"%s\"." ), *outputMapPath );
            return;
        }
    }

    ENQUEUE_RENDER_COMMAND( GenerateDistortionCorrectionMap )(
        [outputMapPath, settings, InFocalLength]( FRHICommandListImmediate& RHICmdList ){
            GenerateDistortionCorrectionMapRenderThread( &RHICmdList, settings, InFocalLength, outputMapPath );
        }
    );
}

void UViveCameraCalibrationHelper::GenerateDistortionCorrectionMapRenderThread( FRHICommandListImmediate* InRHICmdList, const UViveDeveloperSettings* InSettings, 
    float InFocalLength, const FString& InCorrectionFilePath )
{
    auto rect   = FIntRect( 0, 0, InSettings->OutputMapResolution.X, InSettings->OutputMapResolution.Y );
    auto width  = InSettings->OutputMapResolution.X;
    auto height = InSettings->OutputMapResolution.Y;
    VIVELOG( Log, TEXT( "Queuing render command to generate distortion correction map of size: (%d, %d)." ), 
        width, height );

    FTexture2DRHIRef distortionCorrectionRT;
    FRHIResourceCreateInfo createInfo;
    FTexture2DRHIRef dummyTexRef;

    RHICreateTargetableShaderResource2D( width, height, EPixelFormat::PF_FloatRGBA, 1, 
        TexCreate_Transient, TexCreate_RenderTargetable, false, createInfo, 
        distortionCorrectionRT, dummyTexRef );

    auto calibData = LensCalibInfo->SeekCalibDataByFocalLength( InFocalLength );
    FVector2D normalizedPrincipalPoint = FVector2D( calibData.PrincipalPointX, calibData.PrincipalPointY );

    FString log = TEXT( "Submitting the following parameters to distortion correction map generation shader:\n{" );
    log += FString::Printf( TEXT( "\n\tNormalized principal point: (%f, %f),\n\tDistortion Coefficients: [K1: %f, K2: %f, P1: %f, P2: %f, K3: %f]\n}" ),
        normalizedPrincipalPoint.X, normalizedPrincipalPoint.Y, calibData.K1, calibData.K2, calibData.P1, calibData.P2, calibData.K3 );
    VIVELOG( Log, TEXT( "%s" ), *log );

    FRHIRenderPassInfo distortionCorrectionRPInfo( distortionCorrectionRT, ERenderTargetActions::DontLoad_Store );
    InRHICmdList->BeginRenderPass( distortionCorrectionRPInfo, TEXT( "GenerateDistortionCorrectionMapPass" ) );
    {
        const auto renderFeatureLevel = GMaxRHIFeatureLevel;
        const auto globalShaderMap = GetGlobalShaderMap( renderFeatureLevel );

        TShaderMapRef<FViveDistortionCorrectionMapShaderVS> vertexShader( globalShaderMap );
        TShaderMapRef<FViveDistortionCorrectionMapShaderPS> pixelShader( globalShaderMap );

        FGraphicsPipelineStateInitializer graphicsPSOInit;
        graphicsPSOInit.BlendState        = TStaticBlendState<>::GetRHI();
        graphicsPSOInit.RasterizerState   = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
        graphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
        graphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
        graphicsPSOInit.BoundShaderState.VertexShaderRHI      = vertexShader.GetVertexShader();
        graphicsPSOInit.BoundShaderState.PixelShaderRHI       = pixelShader.GetPixelShader();
        graphicsPSOInit.PrimitiveType = PT_TriangleList;

        InRHICmdList->SetViewport( 0.0f, 0.0f, 0.0f, width, height, 1.0f );
        SetGraphicsPipelineState( *InRHICmdList, graphicsPSOInit );
        pixelShader->SetParameters( *InRHICmdList, pixelShader.GetPixelShader(), normalizedPrincipalPoint, 
            calibData.K1, calibData.K2, calibData.P1, calibData.P2, calibData.K3 );

        FPixelShaderUtils::DrawFullscreenQuad( *InRHICmdList, 1 );
    }
    InRHICmdList->EndRenderPass();

    auto texture2D = distortionCorrectionRT->GetTexture2D();
    TArray<FFloat16Color> pixels;
    InRHICmdList->ReadSurfaceFloatData( texture2D, rect, pixels, (ECubeFace)0, 0, 0 );

    LensCalibInfo->PixelData = pixels;
    LensCalibInfo->IsSetPixelData = true;

    if ( InSettings->SaveDistortionCorrectionMap ) {
        auto pixelData = MakeUnique<TImagePixelData<FFloat16Color>>( rect.Size() );
        pixelData->Pixels = pixels;
        check( pixelData->IsDataWellFormed() );

        auto success = FViveUtilitiesHelper::WriteTexture16( InCorrectionFilePath, width, height, MoveTemp( pixelData ) );
        CVIVELOG( success, Log, TEXT( "Wrote distortion correction map to path: \"%s\"." ), *InCorrectionFilePath );
    }
}

bool UViveCameraCalibrationHelper::GetDistortionCorrectionMap( UTexture2D*& OutTexture )
{
    if ( LensCalibInfo->IsSetPixelData ) {
        LensCalibInfo->IsSetPixelData = false;
        auto settings = FViveUtilitiesHelper::GetSettings();
        return FViveUtilitiesHelper::CreateTexture2D( LensCalibInfo->PixelData.GetData(), 
            settings->OutputMapResolution.X, settings->OutputMapResolution.Y, false, true, OutTexture, EPixelFormat::PF_FloatRGBA );
    }

    return false;
}

bool UViveCameraCalibrationHelper::LoadLensCalibInfo( const FString& InFilename )
{
    auto settings = FViveUtilitiesHelper::GetSettings();
    const auto backupOutputPath = FViveUtilitiesHelper::GenerateVideoCameraCalibOutputPath( settings->OutputCalibDirName );
    FString filePath;
    if ( FViveUtilitiesHelper::ValidateFilePath( filePath, backupOutputPath, InFilename, TEXT( "txt" ), false ) ) {
        VIVELOG( Log, TEXT( "#### Load lens calibration infomation from Save data. ####" ) );
        LensCalibInfo->ReadFromFile( filePath );
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

bool UViveCameraCalibrationHelper::WriteTransformToFile( const FRotator& InRotation, const FVector& InLocation, const FString& InFilename )
{
    auto settings = FViveUtilitiesHelper::GetSettings();
    const auto backupOutputPath = FViveUtilitiesHelper::GenerateVideoCameraCalibOutputPath( settings->OutputCalibDirName );
    FString outputPath;
    if ( FViveUtilitiesHelper::ValidateFilePath( outputPath, backupOutputPath, InFilename, TEXT( "txt" ) ) ) {
        auto fileContent = FString::Printf( TEXT( "[BEGIN SAVE VIVESTUDIOSUTILS DATA]\n\rRotation: [%s]\n\rLocation: [%s]\n\r[END SAVE VIVESTUDIOSUTILS DATA]" ), 
            *(InRotation.ToString()), *(InLocation.ToString()) );
        auto result = FViveUtilitiesHelper::WriteTextFile( outputPath, fileContent );
        CVIVELOG( result, Log, TEXT( "#### Success Saved File. Camera Transform Data:\n\r%s ####" ), *fileContent );
        return result;
    }

    return false;
}

//-----------------------------------------------------------------------------

void UViveCameraCalibrationHelper::SetCameraCalibInfo( FViveCameraCalibInfo* InCameraCalibInfo )
{
    CameraCalibInfo = InCameraCalibInfo;
}

void UViveCameraCalibrationHelper::SetLensCalibInfo( FViveLensCalibInfo* InLensCalibInfo )
{
    LensCalibInfo = InLensCalibInfo;
}
