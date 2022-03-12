#pragma once
#include "Patches\Functions.h"

namespace app
{
    struct SDashRingParam
    {
        enum EType
        {
            eType_Dash,
            eType_Rainbow
        };

        float m_FirstSpeed{};
        float m_OutOfControl{};
        float m_KeepVelocityDistance{};
        EType m_Type{};
    };

    static void paramMap_DashRing(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "KeepVelocityDistance", "", 8, 50.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 12, nullptr, 0, 0);
    }
}