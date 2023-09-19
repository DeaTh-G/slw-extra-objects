#include "pch.h"
#include "JumpPanelParam.h"

void slw_extra_objects::paramMap_JumpPanel(app::SetEd::CResClass rClass)
{
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "KeepVelocityDistance", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 8, 0.25f, 0.0f, 10000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "Pitch", "", 12, 45.0f, 0.0f, 10000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamTarget(rClass, "Target", "", 16);
    SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 20, nullptr, 0, 0);
}