// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

struct VIVEOPENCVPLUGIN_API FViveVideoFrame
{
    TArray<FColor> Image;

    //-------------------------------------------------------------------------

    FViveVideoFrame() = default;

    void SetResolution( FIntPoint InResolution );
    int32 GetImageSize( FIntPoint InResolution ) const;
    uint8* GetRawData() const;
    FColor* GetPixelData();
};
