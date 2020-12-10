#include "VideoDriver/ViveCondition.h"
#include "ViveStudiosUtilsPCH.h"

FViveCondition::~FViveCondition()
{
    Destroy();
}

void FViveCondition::Init()
{
    if ( ensure( Condition == nullptr ) )
        Condition = FGenericPlatformProcess::GetSynchEventFromPool( false );
}

void FViveCondition::Destroy()
{
    if ( Condition != nullptr ) {
        Signal();
        FGenericPlatformProcess::ReturnSynchEventToPool( Condition );
        Condition = nullptr;
    }
}

void FViveCondition::WaitForSync() const
{
    if ( ensure( Condition != nullptr ) )
        Condition->Wait();
}

void FViveCondition::Signal()
{
    if ( ensure( Condition != nullptr ) )
        Condition->Trigger();
}
