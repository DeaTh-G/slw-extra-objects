#include "pch.h"
#include "JumpBoardExParam.h"

void slw_extra_objects::paramMap_JumpBoardEx(app::SetEd::CResClass rClass)
{
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "ImpulseSpeedOnSpindash", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "ImpulseSpeedOnNormal", "", 4, 200.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 8, 200.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamBool(rClass, "IsStand", "", 12, true);
    SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 13, nullptr, 0, 0);
}