#include "pch.h"
#include "DashRingParam.h"

void slw_extra_objects::paramMap_DashRing(app::SetEd::CResClass rClass)
{
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "KeepVelocityDistance", "", 8, 50.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 12, nullptr, 0, 0);
}