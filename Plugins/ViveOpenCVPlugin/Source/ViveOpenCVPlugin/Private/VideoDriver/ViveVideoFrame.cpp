#include "VideoDriver/ViveVideoFrame.h"
#include "ViveStudiosUtilsPCH.h"

void FViveVideoFrame::SetResolution( FIntPoint InResolution )
{
    auto imageSize = GetImageSize( InResolution );
    Image.Init( FColor::MakeRandomColor(), imageSize );
}

int32 FViveVideoFrame::GetImageSize( FIntPoint InResolution ) const
{
    return InResolution.X * InResolution.Y;
}

uint8* FViveVideoFrame::GetRawData() const
{
    return (uint8*)Image.GetData();
}

FColor* FViveVideoFrame::GetPixelData()
{
    return Image.GetData();
}
