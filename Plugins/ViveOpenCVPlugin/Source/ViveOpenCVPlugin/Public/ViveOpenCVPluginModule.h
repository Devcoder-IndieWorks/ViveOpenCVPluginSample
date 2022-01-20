// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "Modules/ModuleInterface.h"
#include "Stats/Stats.h"

class IViveOpenCVPluginModule : public IModuleInterface
{
};

// LoadingPhase가 PostConfigInit이면 Stats System에 등록되지 않는다.
DECLARE_STATS_GROUP( TEXT( "ViveOpenCV" ), STATGROUP_ViveOpenCV, STATCAT_Advanced );
