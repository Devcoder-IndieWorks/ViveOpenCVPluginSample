// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ViveCameraTrackerHelper.generated.h"

UCLASS() 
class VIVEOPENCVPLUGIN_API UViveCameraTrackerHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static void ConvertToUnrealLocation( const FVector& InLocation, FVector& OutLocation );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static void ConvertToUnrealRotation( const FRotator& InRotation, FRotator& OutRotation );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static FVector ComputeLocationOffset( const FVector& InLocationA, const FVector& InLocationB );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static FVector CombineLocationOffset( const FVector& InLocation, const FVector& InOffset );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static FRotator ComputeRotationOffset( const FRotator& InRotationA, const FRotator& InRotationB );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static FRotator CombineRotationOffset( const FRotator& InRotation, const FRotator& InOffset );

    UFUNCTION( BlueprintCallable, Category="ViveOpenCVPlugin|CameraTrackerHelper" )
    static bool CompletedDelayFrame();

    static void SetDelayFrame( class FViveDelayFrame* InDelayFrame );

private:
    static class FViveDelayFrame* DelayFrame;
};
