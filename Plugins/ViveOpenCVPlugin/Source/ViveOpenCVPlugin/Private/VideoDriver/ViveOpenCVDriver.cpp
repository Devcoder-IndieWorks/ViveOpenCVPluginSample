#include "VideoDriver/ViveOpenCVDriver.h"
#include "VideoDriver/ViveCameraCalibrator.h"
#include "VideoSource/ViveVideoSource.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveLog.h"

UViveOpenCVDriver::UViveOpenCVDriver( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    NewFrameReady = false;
    FrameResolution = FIntPoint( 1920, 1080 );
}

void UViveOpenCVDriver::Initialize()
{
    Super::Initialize();

    CameraCalibrator = NewObject<UViveCameraCalibrator>( this, UViveCameraCalibrator::StaticClass() );
    CameraCalibrator->SetupPredefinedDictionary( cv::aruco::DICT_6X6_250 );

    NewFrameReady = false;
    WorkerFrame = &FrameInstances[0];
    AvailableFrame = &FrameInstances[1];
    PublishedFrame = &FrameInstances[2];

    auto newWorker = CreateWorker();
    if ( ensure( newWorker != nullptr ) ) {
        Worker.Reset( newWorker );
        auto threadName = FString::Printf( TEXT( "%s_Thread" ), *(GetName()) );
        WorkerThread.Reset( FRunnableThread::Create( newWorker, *threadName, 0, TPri_Normal ) );
    }
}

FRunnable* UViveOpenCVDriver::CreateWorker()
{
    return new FWorkerRunnable( this );
}

void UViveOpenCVDriver::Shutdown()
{
    if ( Worker.IsValid() ) {
        Worker->Stop();
        if ( WorkerThread.IsValid() )
            WorkerThread->WaitForCompletion();

        WorkerThread.Reset( nullptr );
        Worker.Reset( nullptr );
    }

    CameraCalibrator->Shutdown();

    Super::Shutdown();
}

bool UViveOpenCVDriver::IsNewFrameAvailable() const
{
    return NewFrameReady;
}

FViveVideoFrame* UViveOpenCVDriver::GetFrame()
{
    if ( NewFrameReady ) {
        FScopeLock lock( &FrameLock );

        std::swap( AvailableFrame, PublishedFrame );
        NewFrameReady.AtomicSet( false );
    }

    return PublishedFrame;
}

void UViveOpenCVDriver::StoreWorkerFrame()
{
    FScopeLock lock( &FrameLock );

    std::swap( WorkerFrame, AvailableFrame );
    NewFrameReady.AtomicSet( true );
}

void UViveOpenCVDriver::SetFrameResolution( const FIntPoint& InNewResolution )
{
    VIVELOG( Log, TEXT( "#### New resolution Width:[%d] Height:[%d] ####" ), InNewResolution.X, InNewResolution.Y );
    Super::SetFrameResolution( InNewResolution );

    FrameResolution = InNewResolution;
    for ( int32 idx = 0; idx < 3; ++idx )
        FrameInstances[idx].SetResolution( InNewResolution );
}

void UViveOpenCVDriver::OpenVideoSource( const FViveVideoConfiguration& InVideoConfig )
{
    FScopeLock lock( &VideoSourceLock );
    NextVideoConfig = InVideoConfig;
    SwitchToNextVideoSource = true;
}

void UViveOpenCVDriver::SavePoseCalibToFile()
{
    if ( (ModeType == EMode::MODE_Calibrated) && !IsEstimatePoseProgress() ) {
        CameraCalibrator->SavePoseCalibToFile( VideoSourceInstance->CalibPoseOutputFileName );
    }
}

void UViveOpenCVDriver::SaveLensCalibToFile()
{
    if ( (ModeType == EMode::MODE_Calibrated) && !IsCalibrationInProgress() )
        CameraCalibrator->SaveLensCalibToFile( VideoSourceInstance->CalibLensOutputFileName );
}

FIntPoint UViveOpenCVDriver::GetResolution() const
{
    return FrameResolution;
}

FVector2D UViveOpenCVDriver::GetFieldOfView() const
{
    if ( CameraCalibrator != nullptr ) {
        auto fov = CameraCalibrator->GetVideoCameraProperties().FOV;
        if ( !FMath::IsNearlyZero( fov.X ) )
            return fov;
    }

    return FVector2D( 50.0f, 50.0f );
}

void UViveOpenCVDriver::OnVideoConfigSwitch()
{
    if ( IsCalibrationInProgress() )
        CancelCalibration();

    NewFrameReady.AtomicSet( false );
    OnCameraPropertiesChange();
}

void UViveOpenCVDriver::OnCalibrationFinished()
{
    if ( ModeType != EMode::MODE_Calibrated ) {
        ModeType = EMode::MODE_Calibrated;
        CalibrationInProgress = false;
        OnCameraPropertiesChange();
        NotifyCalibrationStatusChange();
    }
}

void UViveOpenCVDriver::OnCameraPropertiesChange()
{
    if ( VideoSourceInstance != nullptr ) {
        OnCameraPropertiesChange( VideoSourceInstance->GetResolution() );
        return;
    }
    VIVELOG( Warning, TEXT( "#### VideoSourceInstance is nullptr. ####" ) );
}

void UViveOpenCVDriver::OnCameraPropertiesChange( const FIntPoint& InNewResolution )
{
    if ( VideoSourceInstance != nullptr )
        SetFrameResolution( InNewResolution );

    NotifyVideoPropertiesChange();
}

void UViveOpenCVDriver::NotifyVideoPropertiesChange()
{
    if ( !IsPendingKill() ) {
        AsyncTask( ENamedThreads::GameThread, [this]{
            VIVELOG( Log, TEXT( "#### NotifyVideoPropertiesChange ####" ) );
            if ( OnVideoPropertiesChagne.IsBound() )
                OnVideoPropertiesChagne.Broadcast( this );
        } );
    }
}

void UViveOpenCVDriver::NotifyCalibrationStatusChange()
{
    if ( !IsPendingKill() ) {
        AsyncTask( ENamedThreads::GameThread, [this]{
            VIVELOG( Log, TEXT( "#### NotifyCalibrationStatusChange ####" ) );
            if ( OnCalibrationStatusChange.IsBound() )
                OnCalibrationStatusChange.Broadcast( this );
        } );
    }
}

float UViveOpenCVDriver::GetCalibrationProgress() const
{
    if ( ensure( CameraCalibrator != nullptr ) )
        return CameraCalibrator->GetProgressRatio();
    return 0.0f;
}

void UViveOpenCVDriver::StartCalibration()
{
    SetDriverMessage( TEXT( "Start calibration." ) );

    FScopeLock lock( &CalibrationLock );
    if ( ensure( CameraCalibrator != nullptr ) )
        CameraCalibrator->Reset( &NextVideoConfig );

    ModeType = EMode::MODE_Capturing;
    CalibrationInProgress = true;
    NotifyCalibrationStatusChange();
}

void UViveOpenCVDriver::CancelCalibration()
{
    SetDriverMessage( TEXT( "Cancel calibration." ) );

    FScopeLock lock( &CalibrationLock );
    if ( ensure( CameraCalibrator != nullptr ) )
        CameraCalibrator->Reset( nullptr );

    ModeType = EMode::MODE_Detection;
    CalibrationInProgress = false;
    NotifyCalibrationStatusChange();
}

void UViveOpenCVDriver::StartEstimatePose()
{
    if ( ModeType == EMode::MODE_Calibrated ) {
        SetDriverMessage( TEXT( "Start estimate pose." ) );

        FScopeLock lock( &CalibrationLock );
        EstimatePoseInProgress = true;
        NotifyCalibrationStatusChange();
    }
}

void UViveOpenCVDriver::StopEstimatePose()
{
    if ( ModeType == EMode::MODE_Calibrated ) {
        SetDriverMessage( TEXT( "Stop estimate pose." ) );

        FScopeLock lock( &CalibrationLock );
        EstimatePoseInProgress = false;
        NotifyCalibrationStatusChange();
    }
}

void UViveOpenCVDriver::ContinueCapturing()
{
    if ( ModeType == EMode::MODE_Detection ) {
        FScopeLock lock( &CalibrationLock );
        ModeType = EMode::MODE_Capturing;
    }
}

//-----------------------------------------------------------------------------

UViveOpenCVDriver::FWorkerRunnable::FWorkerRunnable( UViveOpenCVDriver* InDriver )
    : Driver( InDriver )
{
    CapturedFrame.create( 1920, 1080 );
    CapturedFrame.setTo( cv::Scalar( 0, 0, 255 ) );
}

bool UViveOpenCVDriver::FWorkerRunnable::Init()
{
    VIVELOG( Log, TEXT( "#### Initialize worker. ####" ) );
    Continue.AtomicSet( true );
    return true;
}

uint32 UViveOpenCVDriver::FWorkerRunnable::Run()
{
    Driver->SetDriverMessage( TEXT( "Running OpenCV. Press 'Calibrate Button'." ) );
    VIVELOG( Log, TEXT( "#### Working thread start. ####" ) );

    UViveVideoSource* currentVideoSource = nullptr;

    while( Continue ) {
        bool newOpenVideoSource = false;
        FViveVideoConfiguration newVideoConfig;

        // Step 1: Initialize.
        if ( Driver->SwitchToNextVideoSource ) {
            FScopeLock lock( &Driver->VideoSourceLock );
            if ( currentVideoSource != nullptr )
                currentVideoSource->Disconnect();

            newVideoConfig = Driver->NextVideoConfig;
            VIVELOG( Log, TEXT( "#### Switching video source to [%s] ####" ), *(newVideoConfig.Identifier) );

            currentVideoSource = Driver->VideoSourceInstance;
            Driver->SwitchToNextVideoSource = false;
            newOpenVideoSource = true;
        }

        if ( newOpenVideoSource ) {
            currentVideoSource->Connect( newVideoConfig );
            Driver->OnVideoConfigSwitch();
        }

        // Step 2: Process calibration.
        if ( currentVideoSource != nullptr && currentVideoSource->IsConnected() ) {
            check( currentVideoSource->GetNextFrame( CapturedFrame ) );

            // 캡처된 프레임 이미지 크기가 설정된 프레임 크기와 같지 않으면, Driver에 캡처된 이미지 사이즈로
            // 해상도를 다시 설정 한다.
            auto frameSize = CapturedFrame.size();
            if ( frameSize.width != Driver->FrameResolution.X || frameSize.height != Driver->FrameResolution.Y ) {
                FIntPoint newResolution( frameSize.width, frameSize.height );
                VIVELOG( Error, TEXT( "#### Source returned frame of size %dX%d but %dX%d was expected from source's resolution ####" ), 
                    newResolution.X, newResolution.Y, Driver->FrameResolution.X, Driver->FrameResolution.Y );

                Driver->OnCameraPropertiesChange( newResolution );
                continue;
            }

            // Calibration 계산을 시작 했을때만 Calibration 관련 작업을 진행 한다. 
            if ( Driver->IsCalibrationInProgress() ) {
                auto worldReference = Driver->GetWorld();

                if ( worldReference != nullptr ) {
                    auto calibrator = Driver->CameraCalibrator;
                    FScopeLock lock( &Driver->CalibrationLock );
                    calibrator->ProcessFrame( CapturedFrame, Driver->ModeTypeToString(), worldReference->RealTimeSeconds, Driver );

                    if ( calibrator->IsFinished() )
                        Driver->OnCalibrationFinished();
                }

                CVIVELOG( worldReference == nullptr, Error, TEXT( "World reference is nullptr, cannot measure time for calibration." ) );
            }

            // Marker 위치에서 실제 카메라 위치를 계산 한다.
            if ( Driver->IsEstimatePoseProgress() ) {
                auto calibrator = Driver->CameraCalibrator;
                FScopeLock lock( &Driver->CalibrationLock );
                calibrator->EstimatePoseSingleMarker( CapturedFrame );
            }

            // 프레임 이미지 왜곡 처리(camera calibration 처리가 종료되면 그때 처리 됨)
            {
                auto calibrator = Driver->CameraCalibrator;
                if ( calibrator->IsFinished() ) {
                    FScopeLock lock( &Driver->CalibrationLock );
                    calibrator->RectifyMap( CapturedFrame, Driver->ModeTypeToString() );
                }
            }

            // 화면에 프레임 이미지를 출력 하기 위해 OpenCV 프레임 이미지를 Unreal 이미지로 변환 한다.
            ParallelFor( CapturedFrame.rows, [&]( int32 idx ){
                auto destPixel = (Driver->WorkerFrame->GetPixelData() + (idx * CapturedFrame.cols));
                for ( int32 pixelCol = 0; pixelCol < CapturedFrame.cols; ++pixelCol ) {
                    cv::Vec3b& srcPixel = CapturedFrame.at<cv::Vec3b>( idx, pixelCol );
                    destPixel->R = srcPixel.val[2];
                    destPixel->G = srcPixel.val[1];
                    destPixel->B = srcPixel.val[0];
                    destPixel++;
                }
            } );
            Driver->StoreWorkerFrame();
        }
        else {
            FPlatformProcess::Sleep( 0.25f );
        }
    }

    // Step 3: Destroy.
    {
        FScopeLock lock( &Driver->VideoSourceLock );
        if ( currentVideoSource != nullptr && currentVideoSource->IsConnected() )
            currentVideoSource->Disconnect();

        Driver->VideoSourceInstance = nullptr;
        Driver->OnVideoConfigSwitch();
    }

    VIVELOG( Log, TEXT( "#### Worker thread ends. ####" ) );
    return 0;
}

void UViveOpenCVDriver::FWorkerRunnable::Stop()
{
    VIVELOG( Log, TEXT( "#### Stop worker. ####" ) );
    Continue.AtomicSet( false );
}
