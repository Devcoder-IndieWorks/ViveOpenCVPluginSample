// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "CineCameraActor.h"
#include "ViveCineCameraBase.generated.h"

UCLASS()
class VIVEOPENCVPLUGIN_API AViveCineCameraBase : public ACineCameraActor
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="ViveCineCameraBase|Methods" )
    bool ComputeOffsetTransformFromTracker();
    UFUNCTION( BlueprintCallable, Category="ViveCineCameraBase|Methods" )
    bool ComputeOffsetTransformFromExternal();

    UFUNCTION( BlueprintCallable, Category="ViveCineCameraBase|Methods" )
    void WriteOffsetTransform( const FString& InFilename );

protected:
    virtual void BeginPlay() override;
    virtual void Tick( float InDeltaSeconds ) override;

private:
    void SetupTransform();
    void SetupViewTarget();
    void SetupTracker();

private:
    int32 TrackerId;

    FVector OffsetLocation;
    FRotator OffsetRotation;

    //-------------------------------------------------------------------------

    UPROPERTY( EditAnywhere, Category="ViveCineCameraBase|ExternalInputParams", meta=(AllowPrivateAccess="true") )
    FVector ExternalLocation;
    UPROPERTY( EditAnywhere, Category="ViveCineCameraBase|ExternalInputParams", meta=(AllowPrivateAccess="true") )
    FRotator ExternalRotation;
};
