#include "ViveCameraTrackerHelper.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveDelayFrame.h"
#include "Kismet/KismetmathLibrary.h"

FViveDelayFrame* UViveCameraTrackerHelper::DelayFrame = nullptr;

void UViveCameraTrackerHelper::ConvertToUnrealLocation( const FVector& InLocation, FVector& OutLocation )
{
    // FRotator 생성자 순서 Pitch(Y Axis), Yaw(Z Axis), Roll(X Axis)로 되어져 있음.
    auto rotated = FRotator( 90.0f, 90.0f, 0.0f ).RotateVector( InLocation );
    OutLocation = rotated;
}

void UViveCameraTrackerHelper::ConvertToUnrealRotation( const FRotator& InRotation, FRotator& OutRotation )
{
    auto rotated = UKismetMathLibrary::ComposeRotators( FRotator( 90.0f, 0.0f, 0.0f ), InRotation );
    OutRotation = rotated;
}

FVector UViveCameraTrackerHelper::ComputeLocationOffset( const FVector& InLocationA, const FVector& InLocationB )
{
    return InLocationA - InLocationB;
}

FVector UViveCameraTrackerHelper::CombineLocationOffset( const FVector& InLocation, const FVector& InOffset )
{
    return InLocation + InOffset;
}

FRotator UViveCameraTrackerHelper::ComputeRotationOffset( const FRotator& InRotationA, const FRotator& InRotationB )
{
    return InRotationA - InRotationB;
}

FRotator UViveCameraTrackerHelper::CombineRotationOffset( const FRotator& InRotation, const FRotator& InOffset )
{
    return InRotation + InOffset;
}

bool UViveCameraTrackerHelper::CompletedDelayFrame()
{
    if ( ensure( DelayFrame != nullptr ) )
        return DelayFrame->Completed();

    return false;
}

void UViveCameraTrackerHelper::SetDelayFrame( FViveDelayFrame* InDelayFrame )
{
    DelayFrame = InDelayFrame;
}
