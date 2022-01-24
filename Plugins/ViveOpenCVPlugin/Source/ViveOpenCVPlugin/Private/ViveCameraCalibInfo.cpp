#include "ViveCameraCalibInfo.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveLog.h"

void FViveCameraCalibInfo::ReadFromFile( const FString& InFilename )
{
    VIVELOG( Log, TEXT( "### Calibration Info File: %s ###" ), *InFilename );

    FString fileData;
    FFileHelper::LoadFileToString( fileData, *InFilename );

    TArray<FString> lines;
    fileData.ParseIntoArray( lines, TEXT( "\n\r" ), true );

    for ( auto line : lines ) {
        TArray<FString> infoData;
        line.ParseIntoArray( infoData, TEXT( ":"), false );
        if ( infoData[0].Equals( TEXT( "FocalLength" ) ) ) {
            auto setData = [this]( const FString& InData ){
                FocalLength = FCString::Atof( *InData );
            };
            setData( infoData[1] );
        }
        else if ( infoData[0].Equals( TEXT( "FovX" ) ) ) {
            auto setData = [this]( const FString& InData ) {
                FovX = FCString::Atof( *InData );
            };
            setData( infoData[1] );
        }
        else if ( infoData[0].Equals( TEXT( "FovY" ) ) ) {
            auto setData = [this]( const FString& InData ) {
                FovY = FCString::Atof( *InData );
            };
            setData( infoData[1] );
        }
        else if ( infoData[0].Equals( TEXT( "AspectRatio" ) ) ) {
            auto setData = [this]( const FString& InData ) {
                AspectRatio = FCString::Atof( *InData );
            };
            setData( infoData[1] );
        }
        else if ( infoData[0].Equals( TEXT( "Resolution" ) ) ) {
            auto setData = [this]( const FString& InData ){
                TArray<FString> datas;
                InData.ParseIntoArray( datas, TEXT( "," ), false );
                if ( ensure( datas.Num() >= 2 ) ) {
                    ResolutionWidth  = FCString::Atoi( *datas[0] );
                    ResolutionHeight = FCString::Atoi( *datas[1] );
                }
            };
            setData( infoData[1] );
        }
        else if ( infoData[0].Equals( TEXT( "Location" ) ) ) {
            auto setData = [this]( const FString& InData ) {
                TArray<FString> datas;
                InData.ParseIntoArray( datas, TEXT( "," ), false );
                if ( ensure( datas.Num() >= 3 ) ) {
                    Position.X = (FCString::Atof( *datas[0] ) * 0.1f);
                    Position.Y = (FCString::Atof( *datas[1] ) * 0.1f);
                    Position.Z = (FCString::Atof( *datas[2] ) * 0.1f);
                    Position.W = 1.0f;
                }
            };
            setData( infoData[1] );
        }
        else if ( infoData[0].Equals( TEXT( "Rotation" ) ) ) {
            auto setData = [this]( const FString& InData ){
                TArray<FString> datas;
                InData.ParseIntoArray( datas, TEXT( "," ), false );
                if ( ensure( datas.Num() >= 9 ) ) {
                    PlaneX.W = 0.0f;
                    PlaneX.X = FCString::Atof( *datas[0 + (0 * 3)] );
                    PlaneX.Y = FCString::Atof( *datas[0 + (1 * 3)] );
                    PlaneX.Z = FCString::Atof( *datas[0 + (2 * 3)] );

                    PlaneY.W = 0.0f;
                    PlaneY.X = FCString::Atof( *datas[1 + (0 * 3)] );
                    PlaneY.Y = FCString::Atof( *datas[1 + (1 * 3)] );
                    PlaneY.Z = FCString::Atof( *datas[1 + (2 * 3)] );

                    PlaneZ.W = 0.0f;
                    PlaneZ.X = FCString::Atof( *datas[2 + (0 * 3)] );
                    PlaneZ.Y = FCString::Atof( *datas[2 + (1 * 3)] );
                    PlaneZ.Z = FCString::Atof( *datas[2 + (2 * 3)] );
                }
            };
            setData( infoData[1] );
        }
    }
}

float FViveCameraCalibInfo::GetFocalLength() const
{
    return FocalLength;
}

float FViveCameraCalibInfo::GetFovX() const
{
    return FovX;
}

float FViveCameraCalibInfo::GetFovY() const
{
    return FovY;
}

float FViveCameraCalibInfo::GetAspectRatio() const
{
    return AspectRatio;
}

FTransform FViveCameraCalibInfo::GetTransform() const
{
    FMatrix viewMatrix( PlaneX, PlaneY, PlaneZ, Position );
    return FTransform( viewMatrix );
}

int32 FViveCameraCalibInfo::GetResolutionWidth() const
{
    return ResolutionWidth;
}

int32 FViveCameraCalibInfo::GetResolutionHeight() const
{
    return ResolutionHeight;
}
