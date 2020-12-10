// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class FViveLensCalibInfo
{
public:
    struct CalibData
    {
        float ZoomLevel;
        float PrincipalPointX;
        float PrincipalPointY;
        float K1;
        float K2;
        float P1;
        float P2;
        float K3;
    };

public:
    FViveLensCalibInfo() = default;

    void ReadFromFile( const FString& InFilename );

    //-------------------------------------------------------------------------

    CalibData SeekCalibDataByFocalLength( float InFocalLength ) const;

private:
    CalibData CreateCalibData( TFunctionRef<float()> InFuncZoomLevel, TFunctionRef<TTuple<float, float>()> InFuncPrincipalPoint, 
        TFunctionRef<TTuple<float, float, float, float, float>()> InFuncDistCoeffs );

public:
    bool IsSetPixelData;
    TArray<FFloat16Color> PixelData;

private:
    float MinFocalLength;
    float MaxFocalLength;
    TArray<CalibData> CalibDataList;
};
