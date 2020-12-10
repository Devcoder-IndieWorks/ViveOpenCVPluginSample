// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "UObject/ObjectMacros.h"
#include "ViveVideoConfiguration.generated.h"

UENUM()
enum EViveCalibType
{
    CT_Pose UMETA(DisplayName="Camera Pose"),
    CT_Lens UMETA(DisplayName="Camera Lens"),
    CT_Max  UMETA(Hidden)
};

USTRUCT( BlueprintType )
struct VIVEOPENCVPLUGIN_API FViveVideoConfiguration
{
    GENERATED_BODY()

    //-------------------------------------------------------------------------

    FViveVideoConfiguration() = default;

    void SetupIdentifierAndDisplayName( const FString& InIdentifier, const FString& InSourceName, 
        const FString& InVariant );

    //-------------------------------------------------------------------------

    UPROPERTY( BlueprintReadOnly, Category=Configuration )
    FString Identifier;
    UPROPERTY( BlueprintReadOnly, Category=Configuration )
    FText DisplayName;

    //-------------------------------------------------------------------------

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    int32 CameraId;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    TEnumAsByte<EViveCalibType> CalibrationType;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    int32 BoardSizeWidth;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    int32 BoardSizeHeight;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float BoardSquareSize;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float MarkerSize;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float CaptureDelay;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    int32 WindowsSize;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float ResolutionWidth;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float ResolutionHeight;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float WindowFrameWidth;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float WindosFrameHeight;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float SensorWidth;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float SensorHeight;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float MinZoom;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    float MaxZoom;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    int32 MaxCaptureFrames;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    int32 MaxCaptureCount;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    bool AssumeZeroTangentialDistortion;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    bool FixPrincipalPointAtTheCenter;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    bool FixK1;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    bool FixK2;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category=Configuration )
    bool FixK3;
};
