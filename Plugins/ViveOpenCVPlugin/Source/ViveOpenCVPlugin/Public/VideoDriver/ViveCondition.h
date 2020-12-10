// Copyright UserHabit io. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class VIVEOPENCVPLUGIN_API FViveCondition
{
public:
    FViveCondition() = default;
    ~FViveCondition();

    void Init();
    void Destroy();

    void WaitForSync() const;
    void Signal();

private:
    class FEvent* Condition;
};
