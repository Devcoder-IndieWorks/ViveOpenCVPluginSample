#include "ViveDelayFrame.h"
#include "ViveStudiosUtilsPCH.h"

bool FViveDelayFrame::Completed()
{
    if ( DelayFrame == CountFrame ) {
        CountFrame = 0;
        return true;
    }

    ++CountFrame;
    return false;
}

void FViveDelayFrame::SetDelayFrame( uint32 InDelayFrame )
{
    DelayFrame = InDelayFrame;
}
