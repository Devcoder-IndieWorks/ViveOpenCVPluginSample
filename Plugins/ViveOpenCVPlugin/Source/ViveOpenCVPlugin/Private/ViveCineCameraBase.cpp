#include "ViveCineCameraBase.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveCameraCalibrationHelper.h"
#include "ViveCameraTrackerHelper.h"
#include "ViveLog.h"
#include "Kismet/GameplayStatics.h"
#include "SteamVRFunctionLibrary.h"

AViveCineCameraBase::AViveCineCameraBase( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    TrackerId = -1;
}

void AViveCineCameraBase::BeginPlay()
{
    Super::BeginPlay();

    SetupTransform();
    SetupTracker();
    SetupViewTarget();
}

void AViveCineCameraBase::SetupTransform()
{
    FTransform newTransform;
    if ( UViveCameraCalibrationHelper::GetCalibratedViewTransform( newTransform ) ) {
        SetActorTransform( newTransform );
        VIVELOG( Log, TEXT( "Setup new transform." ) );
    }
}

void AViveCineCameraBase::SetupTracker()
{
    TArray<int32> trackedDevicedIds;
    USteamVRFunctionLibrary::GetValidTrackedDeviceIds( ESteamVRTrackedDeviceType::Other, trackedDevicedIds );
    for ( auto deviceId : trackedDevicedIds ) {
        TrackerId = deviceId;
        break;
    }
}

void AViveCineCameraBase::SetupViewTarget()
{
    auto playerController = UGameplayStatics::GetPlayerController( this, 0 );
    if ( ensure( playerController != nullptr ) )
        playerController->SetViewTargetWithBlend( this, 0.0f );
}

void AViveCineCameraBase::Tick( float InDeltaSeconds )
{
    Super::Tick( InDeltaSeconds );

    if ( TrackerId > -1 ) {
        FVector position;
        FRotator orientation;
        auto successed = USteamVRFunctionLibrary::GetTrackedDevicePositionAndOrientation( TrackerId, position, orientation );
        if ( successed ) {
            FVector unrealLocation;
            FRotator unrealRotation;
            UViveCameraTrackerHelper::ConvertToUnrealLocation( position, unrealLocation );
            UViveCameraTrackerHelper::ConvertToUnrealRotation( orientation, unrealRotation );
            auto newLocation = UViveCameraTrackerHelper::CombineLocationOffset( unrealLocation, OffsetLocation );
            auto newRotation = UViveCameraTrackerHelper::CombineRotationOffset( unrealRotation, OffsetRotation );

            VIVELOG( Log, TEXT( "New location: [%s], rotation: [%s]." ), *(newLocation.ToString()), *(newRotation.ToString()) );

            SetActorLocation( newLocation );
            SetActorRotation( newRotation );
        }
    }
}

bool AViveCineCameraBase::ComputeOffsetTransformFromTracker()
{
    if ( TrackerId > - 1 ) {
        FVector position;
        FRotator orientation;
        auto successed = USteamVRFunctionLibrary::GetTrackedDevicePositionAndOrientation( TrackerId, position, orientation );
        if ( successed ) {
            FVector unrealLocation;
            FRotator unrealRotation;
            UViveCameraTrackerHelper::ConvertToUnrealLocation( position, unrealLocation );
            UViveCameraTrackerHelper::ConvertToUnrealRotation( orientation, unrealRotation );
            OffsetLocation = UViveCameraTrackerHelper::ComputeLocationOffset( GetActorLocation(), unrealLocation );
            OffsetRotation = UViveCameraTrackerHelper::ComputeRotationOffset( GetActorRotation(), unrealRotation );
        }

        return successed;
    }

    return false;
}

bool AViveCineCameraBase::ComputeOffsetTransformFromExternal()
{
    if ( !ExternalLocation.IsZero() && !ExternalRotation.IsZero() ) {
        OffsetLocation = UViveCameraTrackerHelper::ComputeLocationOffset( GetActorLocation(), ExternalLocation );
        OffsetRotation = UViveCameraTrackerHelper::ComputeRotationOffset( GetActorRotation(), ExternalRotation );
        return true;
    }

    return false;
}

void AViveCineCameraBase::WriteOffsetTransform( const FString& InFilename )
{
    if ( !UViveCameraCalibrationHelper::WriteTransformToFile( OffsetRotation, OffsetLocation, InFilename ) )
        VIVELOG( Error, TEXT( "Failed. save file. File name: [%s]" ), *InFilename );
}
