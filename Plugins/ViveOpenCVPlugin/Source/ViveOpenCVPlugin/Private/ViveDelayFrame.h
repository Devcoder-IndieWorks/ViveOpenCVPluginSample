// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class FViveDelayFrame
{
public:
    FViveDelayFrame() = default;

    bool Completed();

    void SetDelayFrame( uint32 InDelayFrame );

private:
    uint32 DelayFrame;
    uint32 CountFrame;
};
