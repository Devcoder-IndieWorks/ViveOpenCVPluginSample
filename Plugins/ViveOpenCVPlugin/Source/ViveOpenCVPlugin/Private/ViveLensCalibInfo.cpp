#include "ViveLensCalibInfo.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveLog.h"

void FViveLensCalibInfo::ReadFromFile( const FString& InFilename )
{
    VIVELOG( Log, TEXT( "### Lens Calibration Info File: %s ###" ), *InFilename );

    FString fileData;
    FFileHelper::LoadFileToString( fileData, *InFilename );

    TArray<FString> lines;
    fileData.ParseIntoArray( lines, TEXT( "\r\n" ), true );

    int32 index = 0;
    {
        TArray<FString> infoDatas;
        lines[index++].ParseIntoArray( infoDatas, TEXT( ":" ), false );
        if ( ensure( infoDatas[0].Equals( TEXT( "MinFocalLength" ) ) ) )
            MinFocalLength = FCString::Atof( *infoDatas[1] );
    }
    {
        TArray<FString> infoDatas;
        lines[index++].ParseIntoArray( infoDatas, TEXT( ":" ), false );
        if ( ensure( infoDatas[0].Equals( TEXT( "MaxFocalLength" ) ) ) )
            MaxFocalLength = FCString::Atof( *infoDatas[1] );
    }

    int32 numCalibData = 0;
    {
        TArray<FString> infoDatas;
        lines[index++].ParseIntoArray( infoDatas, TEXT( ":" ), false );
        if ( ensure( infoDatas[0].Equals( TEXT( "ResultNum" ) ) ) )
            numCalibData = FCString::Atoi( *infoDatas[1] );
    }

    for ( int32 i = 0; i < numCalibData; ++i ) {
        auto calibData = CreateCalibData( 
            [ zoomLevel = lines[index] ]{
                TArray<FString> infoDatas;
                zoomLevel.ParseIntoArray( infoDatas, TEXT( ":" ), false );
                if ( ensure( infoDatas[0].Equals( TEXT( "ZoomLevel" ) ) ) )
                    return FCString::Atof( *infoDatas[1] );

                return 0.0f;
            }, 
            [ principalPoint = lines[index + 1] ]{
                TArray<FString> infoDatas;
                principalPoint.ParseIntoArray( infoDatas, TEXT( ":" ), false );
                if ( ensure( infoDatas[0].Equals( TEXT( "PrincipalPoint" ) ) ) ) {
                    TArray<FString> values;
                    infoDatas[1].ParseIntoArray( values, TEXT( "," ), false );
                    auto x = FCString::Atof( *values[0] );
                    auto y = FCString::Atof( *values[1] );
                    return MakeTuple( x, y );
                }

                return MakeTuple( 0.0f, 0.0f );
            }, 
            [ distCoeffs = lines[index + 2] ]{
                TArray<FString> infoDatas;
                distCoeffs.ParseIntoArray( infoDatas, TEXT( ":" ), false );
                if ( ensure( infoDatas[0].Equals( TEXT( "DistortionCoefficients(K1, K2, P1, P2, K3)" ) ) ) ) {
                    TArray<FString> values;
                    infoDatas[1].ParseIntoArray( values, TEXT( "," ), false );
                    auto k1 = FCString::Atof( *values[0] );
                    auto k2 = FCString::Atof( *values[1] );
                    auto p1 = FCString::Atof( *values[2] );
                    auto p2 = FCString::Atof( *values[3] );
                    auto k3 = FCString::Atof( *values[4] );
                    return MakeTuple( k1, k2, p1, p2, k3 );
                }

                return MakeTuple( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
            } );
        CalibDataList.Add( calibData );
        index += 3;
    }
}

FViveLensCalibInfo::CalibData FViveLensCalibInfo::CreateCalibData(TFunctionRef<float()> InFuncZoomLevel, TFunctionRef<TTuple<float, float>()> InFuncPrincipalPoint, 
    TFunctionRef<TTuple<float, float, float, float, float>()> InFuncDistCoeffs )
{
    auto zoomLevel      = InFuncZoomLevel();
    auto principalPoint = InFuncPrincipalPoint();
    auto distCoeffs     = InFuncDistCoeffs();

    FViveLensCalibInfo::CalibData calibData;
    calibData.ZoomLevel = zoomLevel;
    calibData.PrincipalPointX = principalPoint.Get<0>();
    calibData.PrincipalPointY = principalPoint.Get<1>();
    calibData.K1 = distCoeffs.Get<0>();
    calibData.K2 = distCoeffs.Get<1>();
    calibData.P1 = distCoeffs.Get<2>();
    calibData.P2 = distCoeffs.Get<3>();
    calibData.K3 = distCoeffs.Get<4>();

    return calibData;
}

FViveLensCalibInfo::CalibData FViveLensCalibInfo::SeekCalibDataByFocalLength( float InFocalLength ) const
{
    if ( CalibDataList.Num() == 1 )
        return CalibDataList[0];

    InFocalLength = FMath::Clamp( InFocalLength, MinFocalLength, MaxFocalLength );

    float zoomLevel = (InFocalLength - MinFocalLength) / (MaxFocalLength - MinFocalLength);
    VIVELOG( Log, TEXT( "Zoom Level: %f" ), zoomLevel );
    auto num = CalibDataList.Num() - 1;
    auto currentValue = zoomLevel * (float)num;
    auto prevIndex = FMath::FloorToInt( currentValue );
    auto nextIndex = FMath::CeilToInt( currentValue );
    VIVELOG( Log, TEXT( "Prev Index: %d, Next Index: %d" ), prevIndex, nextIndex );
    FViveLensCalibInfo::CalibData prevCalibData = CalibDataList[ prevIndex ];
    FViveLensCalibInfo::CalibData nextCalibData = CalibDataList[ nextIndex ];

    float ratio = (zoomLevel - prevCalibData.ZoomLevel) / (nextCalibData.ZoomLevel - prevCalibData.ZoomLevel);
    VIVELOG( Log, TEXT( "Zoom Level Ratio: %f" ), ratio );
    FViveLensCalibInfo::CalibData resultCalibData;
    resultCalibData.ZoomLevel = zoomLevel;
    resultCalibData.PrincipalPointX = FMath::Lerp( prevCalibData.PrincipalPointX, nextCalibData.PrincipalPointX, ratio );
    resultCalibData.PrincipalPointY = FMath::Lerp( prevCalibData.PrincipalPointY, nextCalibData.PrincipalPointY, ratio );
    resultCalibData.K1 = FMath::Lerp( prevCalibData.K1, nextCalibData.K1, ratio );
    resultCalibData.K2 = FMath::Lerp( prevCalibData.K2, nextCalibData.K2, ratio );
    resultCalibData.P1 = FMath::Lerp( prevCalibData.P1, nextCalibData.P1, ratio );
    resultCalibData.P2 = FMath::Lerp( prevCalibData.P2, nextCalibData.P2, ratio );
    resultCalibData.K3 = FMath::Lerp( prevCalibData.K3, nextCalibData.K3, ratio );

    return resultCalibData;
}
