#include "ViveUtilitiesHelper.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveDeveloperSettings.h"
#include "ViveLog.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "ImageWriteTask.h"
#include "ImageWriteQueue.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"

bool FViveUtilitiesHelper::CreateTexture2D( void* InPixelData, int32 InWidth, int32 InHeight, bool InsRGB, bool InIsLUT, 
    UTexture2D*& OutTexture, EPixelFormat InPixelFormat )
{
    int32 stride = 0;
    int32 size = 0;

    switch( InPixelFormat ) {
    case EPixelFormat::PF_B8G8R8A8:
    case EPixelFormat::PF_R8G8B8A8:
    case EPixelFormat::PF_A8R8G8B8:
        size = 1;
        stride = 4;
        break;

    case EPixelFormat::PF_FloatRGBA:
    case EPixelFormat::PF_A16B16G16R16:
        size = 2;
        stride = 4;
        break;
    default:
        VIVELOG( Error, TEXT( "Non-implemented pixel format: \"%s\"." ), GetPixelFormatString( InPixelFormat ) );
        return false;
    }

    OutTexture = UTexture2D::CreateTransient( InWidth, InHeight, InPixelFormat );
    if ( OutTexture == nullptr ) {
        VIVELOG( Error, TEXT( "Unable to create transient texture" ) );
        return false;
    }

    OutTexture->SRGB = InsRGB;
    VIVELOG( Log, TEXT( "Created transient UTexture2D with the following parameters:\n{\n\rResolution: (%d, %d), \n\rPixel Format: \"%s\", \n\rsRGB: %d\n}" ), 
        InWidth, InHeight, GetPixelFormatString(InPixelFormat), InsRGB ? 1 : 0 );

    if ( InIsLUT ) {
        OutTexture->Filter = TextureFilter::TF_Nearest;
        OutTexture->CompressionSettings = TextureCompressionSettings::TC_HDR;
        OutTexture->LODGroup = TextureGroup::TEXTUREGROUP_16BitData;
    }

    OutTexture->PlatformData->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    OutTexture->PlatformData->Mips[0].SizeX = InWidth;
    OutTexture->PlatformData->Mips[0].SizeY = InHeight;
    OutTexture->PlatformData->Mips[0].BulkData.Realloc( InWidth * InHeight * stride * size );
    OutTexture->PlatformData->Mips[0].BulkData.Unlock();

    uint8* textureData = (uint8*)OutTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    if ( textureData == nullptr ) {
        VIVELOG( Error, TEXT( "BulkData.Lock() returned nullptr." ) );
        return false;
    }
    FMemory::Memcpy( textureData, InPixelData, InWidth * InHeight * stride * size );
    OutTexture->PlatformData->Mips[0].BulkData.Unlock();

    OutTexture->UpdateResource();
    OutTexture->RefreshSamplerStates();

    return true;
}

//-----------------------------------------------------------------------------

FString FViveUtilitiesHelper::GenerateVideoCameraCalibOutputPath( const FString& InSubFolder )
{
    return FPaths::ConvertRelativePathToFull( FPaths::Combine( FPaths::ProjectSavedDir(), InSubFolder ) );
}

//-----------------------------------------------------------------------------

FString FViveUtilitiesHelper::GenerateDistortionCorrectionMapOutputPath( const FString& InSubFolder )
{
    return FPaths::ConvertRelativePathToFull( FPaths::Combine( FPaths::ProjectContentDir(), InSubFolder ) );
}

bool FViveUtilitiesHelper::ValidateFilePath( FString& InOutFilePath, const FString& InAbsoluteBackupFolderPath, 
    const FString& InBackupFilename, const FString& InBackupExtension, bool InUseIndexedFile )
{
    if ( InOutFilePath.IsEmpty() || !FPaths::ValidatePath( InOutFilePath ) ) {
        FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree( *InAbsoluteBackupFolderPath );
        InOutFilePath = FPaths::Combine( InAbsoluteBackupFolderPath, InBackupFilename );
        if ( InUseIndexedFile )
            InOutFilePath = FViveUtilitiesHelper::GenerateIndexedFilePath( InOutFilePath, InBackupExtension );
        return true;
    }

    if ( FPaths::IsRelative( InOutFilePath ) )
        InOutFilePath = FPaths::ConvertRelativePathToFull( InOutFilePath );

    if ( FPaths::IsDrive( InOutFilePath ) ) {
        InOutFilePath = FPaths::Combine( InOutFilePath, InBackupFilename );
        if ( InUseIndexedFile )
            InOutFilePath = FViveUtilitiesHelper::GenerateIndexedFilePath( InOutFilePath, InBackupFilename );
        return true;
    }

    auto folder = FPaths::GetPath( InOutFilePath );
    if ( folder.IsEmpty() )
        folder = InAbsoluteBackupFolderPath;

    auto filename = FPaths::GetBaseFilename( InOutFilePath );
    auto extension = FPaths::GetExtension( InOutFilePath );

    FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree( *folder );
    if ( extension.IsEmpty() ) {
        if ( FPaths::FileExists( folder ) ) {
            VIVELOG( Error, TEXT( "Cannot create path to file: \"%s\", the folder: \"%s\" is a file." ), *InOutFilePath, *folder );
            return false;
        }

        InOutFilePath = FPaths::Combine( folder, InBackupFilename );
        if ( InUseIndexedFile )
            InOutFilePath = FViveUtilitiesHelper::GenerateIndexedFilePath( InOutFilePath, InBackupExtension );
        return true;
    }

    InOutFilePath = FPaths::Combine( folder, filename );
    if ( InUseIndexedFile )
        InOutFilePath = FViveUtilitiesHelper::GenerateIndexedFilePath( InOutFilePath, extension );
    return true;
}

FString FViveUtilitiesHelper::GenerateIndexedFilePath( const FString& InFilePathWithoutExtension, const FString& InExtension )
{
    int32 index = 0;
    while( FPaths::FileExists( FString::Printf( TEXT( "%s-%d.%s" ), *InFilePathWithoutExtension, index, *InExtension ) ) )
        index++;

    return FString::Printf( TEXT( "%s-%d.%s" ), *InFilePathWithoutExtension, index, *InExtension );
}

//-----------------------------------------------------------------------------

bool FViveUtilitiesHelper::WriteTextFile( const FString& InAbsolutePath, const FString& InFileContent )
{
    return FFileHelper::SaveStringToFile( InFileContent, *InAbsolutePath );
}

bool FViveUtilitiesHelper::WriteTexture16( const FString& InAbsoluteTexturePath, int32 InWidth, int32 InHeight, 
    TUniquePtr<TImagePixelData<FFloat16Color>> InData )
{
    auto imageWriteQueueModule = FModuleManager::Get().GetModulePtr<IImageWriteQueueModule>( "ImageWriteQueue" );
    if ( imageWriteQueueModule == nullptr ) {
        VIVELOG( Error, TEXT( "Unable to retrieve ImageWriteQueue." ) );
        return false;
    }

    auto imageTask = MakeUnique<FImageWriteTask>();
    imageTask->Format = EImageFormat::EXR;
    imageTask->Filename = InAbsoluteTexturePath;
    imageTask->bOverwriteFile = true;
    imageTask->CompressionQuality = (int32)EImageCompressionQuality::Uncompressed;
    imageTask->PixelData = MoveTemp( InData );

    TFuture<bool> dispatchedTask = imageWriteQueueModule->GetWriteQueue().Enqueue( MoveTemp( imageTask ) );
    if ( dispatchedTask.IsValid() )
        dispatchedTask.Wait();

    return true;
}

//-----------------------------------------------------------------------------

const UViveDeveloperSettings* FViveUtilitiesHelper::GetSettings()
{
    return GetDefault<UViveDeveloperSettings>();
}
