#include "VideoDriver/ViveVideoImageBuffer.h"
#include "ViveStudiosUtilsPCH.h"
#include "ViveLog.h"

FViveVideoImageBuffer::FViveVideoImageBuffer( bool InShowLog )
{
    ShowLog = InShowLog;

    ProductCondition = MakeShareable( new FViveCondition() );
    ConsumeCondition = MakeShareable( new FViveCondition() );
    ProductCondition->Init();
    ConsumeCondition->Init();
}

FViveVideoImageBuffer::~FViveVideoImageBuffer()
{
    if ( ProductCondition.IsValid() ) {
        ProductCondition->Destroy();
        ProductCondition.Reset();
    }

    if ( ConsumeCondition.IsValid() ) {
        ConsumeCondition->Destroy();
        ConsumeCondition.Reset();
    }
}

void FViveVideoImageBuffer::InFrameImage( const cv::Mat_<cv::Vec3b>& InFrame )
{
    if ( !ImageDatas.IsEmpty() )
        ConsumeCondition->WaitForSync();

    FScopeLock scopeLock( &CriticalSection );
    ImageDatas.Enqueue( InFrame );
    ProductCondition->Signal();
    CVIVELOG( ShowLog, Log, TEXT( "#### Process Productor. ####" ) );
}

bool FViveVideoImageBuffer::OutFrameImage( cv::Mat_<cv::Vec3b>& OutFrame )
{
    if ( ImageDatas.IsEmpty() )
        ProductCondition->WaitForSync();

    FScopeLock scopeLock( &CriticalSection );
    bool result = ImageDatas.Dequeue( OutFrame );
    ConsumeCondition->Signal();
    CVIVELOG( ShowLog, Log, TEXT( "#### Process Consumer. ####" ) );
    return result;
}
