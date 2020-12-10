// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ViveCameraCalibrationHelper.generated.h"

UCLASS()
class VIVEOPENCVPLUGIN_API UViveCameraCalibrationHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool GetCalibratedViewTransform( FTransform& OutTransform );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool GetCameraFocalLength( float& OutFocalLength );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool GetCameraFovX( float& OutFovX );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool GetCameraFovY( float& OutFovY );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool GetCameraAspectRatio( float& OutAspectRatio );

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static void GenerateDistortionCorrectionMap( float InFocalLength );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool GetDistortionCorrectionMap( UTexture2D*& OutTexture );

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraCalibrator" )
    static bool WriteTransformToFile( const FRotator& InRotation, const FVector& InLocation, 
        const FString& InFilename );

public:
    static void SetCameraCalibInfo( class FViveCameraCalibInfo* InCameraCalibInfo );
    static void SetLensCalibInfo( class FViveLensCalibInfo* InLensCalibInfo );

private:
    static void GenerateDistortionCorrectionMapRenderThread( class FRHICommandListImmediate* InRHICmdList, 
        const class UViveDeveloperSettings* InSettings, float InFocalLength, int32 InSrcResolutionX, int32 InSrcResolutionY, 
        const FString& InCorrectionFilePath );

private:
    static class FViveCameraCalibInfo* CameraCalibInfo;
    static class FViveLensCalibInfo* LensCalibInfo;
};
