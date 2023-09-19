#pragma once
#include "Patches\Functions.h"

namespace app
{
    struct SWideSpringParam
    {
        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};
        bool IsEventOn{};
    };

    static void paramMap_WideSpring(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "KeepVelocityDistance", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 8, 0.25f, 0.0f, 10000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamBool(rClass, "IsEventOn", "", 12, false);
    }
}