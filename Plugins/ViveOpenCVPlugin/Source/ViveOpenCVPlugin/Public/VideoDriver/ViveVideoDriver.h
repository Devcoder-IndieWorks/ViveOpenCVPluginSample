// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "UObject/Object.h"
#include "VideoSource/ViveVideoConfiguration.h"
#include "ViveVideoDriver.generated.h"

UCLASS( Abstract, Blueprintable, BlueprintType ) 
class VIVEOPENCVPLUGIN_API UViveVideoDriver : public UObject
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void Initialize();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void Tick();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void Shutdown();

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    bool OpenVideoSourceByName( const FString& InVideoConfigName );
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    bool OpenVideoSourceDefault();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void OpenVideoSource( const FViveVideoConfiguration& InVideoConfig );
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    bool IsConnected() const;

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    UTexture2D* GetOutputTexture() const;
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    class UViveVideoSource* GetVideoSource() const;

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual bool IsCalibrated() const;
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual bool IsCalibrationInProgress() const;
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual float GetCalibrationProgress() const;
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void StartCalibration();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void CancelCalibration();

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void StartEstimatePose();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void StopEstimatePose();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual bool IsEstimatePoseProgress() const;

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void ContinueCapturing();

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual FIntPoint GetResolution() const;
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual FVector2D GetFieldOfView() const;
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    void GetCameraParameters( FIntPoint& OutCameraResolution, FVector2D& OutFieldOfView ) const;

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void SavePoseCalibToFile();
    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    virtual void SaveLensCalibToFile();

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="Driver|Functions" )
    FString GetDriverMessage() const;

public:
    virtual struct FViveVideoFrame* GetFrame();
    virtual bool IsNewFrameAvailable() const;

    //-------------------------------------------------------------------------

    FString ModeTypeToString() const;
    void SetModeTypeByString( const FString& InModeType );

    //-------------------------------------------------------------------------

    void SetDriverMessage( const FString& InMessage );

    //-------------------------------------------------------------------------

    static UViveVideoDriver* GetGlobalDriver();

protected:
    virtual void SetFrameResolution( const FIntPoint& InNewResolution );

private:
    void WriteFrameToTexture();

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FViveVideoPropertiesChange, UViveVideoDriver*, Driver );
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FViveCalibrationStatusChange, UViveVideoDriver*, Driver );
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FViveVideoDriverInstanceChange, UViveVideoDriver*, Driver );

    UPROPERTY( BlueprintAssignable )
    FViveVideoPropertiesChange OnVideoPropertiesChagne;
    UPROPERTY( BlueprintAssignable )
    FViveCalibrationStatusChange OnCalibrationStatusChange;

    static FViveVideoDriverInstanceChange OnVideoDriverInstanceChange;

protected:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="Driver|Settings" )
    TSubclassOf<class UViveVideoSource> AvailableVideoSource;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="Driver|Settings" )
    FString DefaultVideoConfigName;

    //-------------------------------------------------------------------------

    UPROPERTY( Transient, BlueprintReadOnly, Category="Driver|FieldData" )
    UTexture2D* OutputTexture;

    //-------------------------------------------------------------------------

    UPROPERTY( Transient )
    class UViveVideoSource* VideoSourceInstance;

protected:
    enum class EMode {
        MODE_Detection,
        MODE_Capturing,
        MODE_Calibrated
    };

    bool IsActive;
    bool CalibrationInProgress;
    bool EstimatePoseInProgress;
    EMode ModeType;

    //-------------------------------------------------------------------------

    FString Message;

private:
    struct FTextureUpdateParameters
    {
        class FTexture2DResource* Texture2DResource;
        FUpdateTextureRegion2D RegionDefinition;
        UViveVideoDriver* Driver;
    };
    FTextureUpdateParameters TextureUpdateParam;

    //-------------------------------------------------------------------------

    static UViveVideoDriver* GlobalDriver;
};
