#include "ViveOpenCVPluginModule.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveCameraCalibInfo.h"
#include "ViveLensCalibInfo.h"
#include "ViveCameraCalibrationHelper.h"
#include "ViveDelayFrame.h"
#include "ViveCameraTrackerHelper.h"
#include "ViveDeveloperSettings.h"
#include "ViveLog.h"

class FViveOpenCVPluginModule : public IViveOpenCVPluginModule
{
public:
    virtual void StartupModule() override
    {
        VIVELOG( Log, TEXT( "============================= Start Vive OpenCV Plugin =============================" ) );

        auto baseDir = IPluginManager::Get().FindPlugin( TEXT( "ViveOpenCVPlugin" ) )->GetBaseDir();

        auto libraryPath = FPaths::Combine( baseDir, TEXT( "ThirdParty/OpenCV/x64/vc16/bin/opencv_world" ) );
        FString opencvVersion = TEXT( "430" );
#if UE_BUILD_DEBUG
        libraryPath = FString::Printf( TEXT( "%s%s%s.dll" ), *libraryPath, *opencvVersion, TEXT( "d" ) );
#else
        libraryPath = FString::Printf( TEXT( "%s%s.dll" ), *libraryPath, *opencvVersion );
#endif
        OpenCVLibHandle = FPlatformProcess::GetDllHandle( *libraryPath );
        CVIVELOG( OpenCVLibHandle == nullptr, Error, TEXT( "Failed to load opencv library!" ) );

        auto pluginShaderDir = FPaths::Combine( baseDir, TEXT( "Shaders" ) );
        AddShaderSourceDirectoryMapping( TEXT( "/ViveOpenCVPluginShaders" ), pluginShaderDir );

        CameraCalibInfo = MakeShareable<FViveCameraCalibInfo>( new FViveCameraCalibInfo() );
        UViveCameraCalibrationHelper::SetCameraCalibInfo( CameraCalibInfo.Get() );

        LensCalibInfo = MakeShareable<FViveLensCalibInfo>( new FViveLensCalibInfo );
        LensCalibInfo->IsSetPixelData = false;
        UViveCameraCalibrationHelper::SetLensCalibInfo( LensCalibInfo.Get() );

        DelayFrame = MakeShareable<FViveDelayFrame>( new FViveDelayFrame() );
        DelayFrame->SetDelayFrame( 1 );
        UViveCameraTrackerHelper::SetDelayFrame( DelayFrame.Get() );
    }

    virtual void ShutdownModule() override
    {
        VIVELOG( Log, TEXT( "============================= Shutdown Vive OpenCV Plugin =============================" ) );

        if ( OpenCVLibHandle != nullptr ) {
            FPlatformProcess::FreeDllHandle( OpenCVLibHandle );
            OpenCVLibHandle = nullptr;
        }

        UViveCameraCalibrationHelper::SetCameraCalibInfo( nullptr );
        UViveCameraCalibrationHelper::SetLensCalibInfo( nullptr );
        UViveCameraTrackerHelper::SetDelayFrame( nullptr );

        CameraCalibInfo.Reset();
        LensCalibInfo.Reset();
        DelayFrame.Reset();
    }

private:
    TSharedPtr<FViveCameraCalibInfo> CameraCalibInfo;
    TSharedPtr<FViveLensCalibInfo> LensCalibInfo;
    TSharedPtr<FViveDelayFrame> DelayFrame;
    void* OpenCVLibHandle;
};

IMPLEMENT_MODULE( FViveOpenCVPluginModule, ViveOpenCVPlugin );
