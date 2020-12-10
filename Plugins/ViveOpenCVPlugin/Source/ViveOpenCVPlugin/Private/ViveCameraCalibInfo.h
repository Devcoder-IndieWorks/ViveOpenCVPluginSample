// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class FViveCameraCalibInfo
{
public:
    FViveCameraCalibInfo() = default;

    void ReadFromFile( const FString& InFilename );

    //-------------------------------------------------------------------------

    float GetFocalLength() const;
    float GetFovX() const;
    float GetFovY() const;
    float GetAspectRatio() const;
    FTransform GetTransform() const;
    int32 GetResolutionWidth() const;
    int32 GetResolutionHeight() const;

private:
    float FocalLength;
    float FovX;
    float FovY;
    float AspectRatio;
    int32 ResolutionWidth;
    int32 ResolutionHeight;

    FPlane PlaneX;
    FPlane PlaneY;
    FPlane PlaneZ;
    FVector4 Position;
};
