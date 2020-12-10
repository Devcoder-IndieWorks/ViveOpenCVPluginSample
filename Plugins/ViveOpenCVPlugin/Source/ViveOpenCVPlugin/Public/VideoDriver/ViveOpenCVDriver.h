// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "ViveOpenCV.h"
#include "ViveVideoDriver.h"
#include "ViveVideoFrame.h"
#include "ViveOpenCVDriver.generated.h"

UCLASS( Blueprintable, BlueprintType ) 
class VIVEOPENCVPLUGIN_API UViveOpenCVDriver : public UViveVideoDriver
{
    GENERATED_UCLASS_BODY()
public:
    void Initialize() override;
    void Shutdown() override;

    //-------------------------------------------------------------------------

    float GetCalibrationProgress() const override;
    void StartCalibration() override;
    void CancelCalibration() override;

    void StartEstimatePose() override;
    void StopEstimatePose() override;

    void ContinueCapturing() override;

    //-------------------------------------------------------------------------

    FIntPoint GetResolution() const override;
    FVector2D GetFieldOfView() const override;

    //-------------------------------------------------------------------------

    FViveVideoFrame* GetFrame() override;
    bool IsNewFrameAvailable() const override;

    //-------------------------------------------------------------------------

    void OpenVideoSource( const FViveVideoConfiguration& InVideoConfig ) override;

    //-------------------------------------------------------------------------

    void SavePoseCalibToFile() override;
    void SaveLensCalibToFile() override;

private:
    void SetFrameResolution( const FIntPoint& InNewResolution ) override;

    //-------------------------------------------------------------------------

    FRunnable* CreateWorker();
    void StoreWorkerFrame();

    void OnVideoConfigSwitch();
    void OnCalibrationFinished();

    void OnCameraPropertiesChange();
    void OnCameraPropertiesChange( const FIntPoint& InNewResolution );
    void NotifyVideoPropertiesChange();
    void NotifyCalibrationStatusChange();

private:
    class FWorkerRunnable : public FRunnable
    {
    public:
        FWorkerRunnable( UViveOpenCVDriver* InDriver );

        bool Init() override;
        uint32 Run() override;
        void Stop() override;

    private:
        UViveOpenCVDriver* Driver;
        FThreadSafeBool Continue;
        cv::Mat_<cv::Vec3b> CapturedFrame;
    };

private:
    UPROPERTY( Transient )
    class UViveCameraCalibrator* CameraCalibrator;

    //-------------------------------------------------------------------------

    FCriticalSection FrameLock;
    FCriticalSection VideoSourceLock;
    FCriticalSection CalibrationLock;

    FThreadSafeBool NewFrameReady;

    //-------------------------------------------------------------------------

    FViveVideoFrame* WorkerFrame;
    FViveVideoFrame* AvailableFrame;
    FViveVideoFrame* PublishedFrame;
    FViveVideoFrame FrameInstances[3];

    FIntPoint FrameResolution;

    //-------------------------------------------------------------------------

    bool SwitchToNextVideoSource;
    FViveVideoConfiguration NextVideoConfig;

    //-------------------------------------------------------------------------

    TUniquePtr<FRunnable> Worker;
    TUniquePtr<FRunnableThread> WorkerThread;
};
