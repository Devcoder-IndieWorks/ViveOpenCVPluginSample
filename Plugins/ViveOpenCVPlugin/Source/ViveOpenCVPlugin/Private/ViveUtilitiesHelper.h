// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "ImagePixelData.h"

class FViveUtilitiesHelper
{
public:
    static bool CreateTexture2D( void* InPixelData, int32 InWidth, int32 InHeight, bool InsRGB, bool InIsLUT, 
        class UTexture2D*& OutTexture, EPixelFormat InPixelFormat );

    //-------------------------------------------------------------------------

    static FString GenerateVideoCameraCalibOutputPath( const FString& InSubFolder );

    //-------------------------------------------------------------------------

    static FString GenerateDistortionCorrectionMapOutputPath( const FString& InSubFolder );
    static FString GenerateIndexedFilePath( const FString& InFilePathWithoutExtension, const FString& InExtension );
    static bool ValidateFilePath( FString& InOutFilePath, const FString& InAbsoluteBackupFolderPath, 
        const FString& InBackupFilename, const FString& InBackupExtension, bool InUseIndexedFile = true );

    //-------------------------------------------------------------------------

    static bool WriteTextFile( const FString& InAbsolutePath, const FString& InFileContent );

    static bool WriteTexture16( const FString& InAbsoluteTexturePath, int32 InWidth, int32 InHeight, 
        TUniquePtr<TImagePixelData<FFloat16Color>> InData );

    //-------------------------------------------------------------------------

    static const class UViveDeveloperSettings* GetSettings();
};
