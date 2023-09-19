#include "pch.h"
#include "WideSpringParam.h"

void slw_extra_objects::paramMap_WideSpring(app::SetEd::CResClass rClass)
{
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "Length", "", 0, 30.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamBool(rClass, "IsWaitUp", "", 4, false);
    SLW_EXTRA_OBJECTS::AddParamBool(rClass, "IsOneTimeUp", "", 5, false);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "ImpulseVelocity", "", 8, 250.0f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 12, 0.25f, 0.0f, 1000000.0f, 1.0f);
    SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "UpSpeedMax", "", 16, 300.0f, 0.0f, 1000000.0f, 1.0f);
}