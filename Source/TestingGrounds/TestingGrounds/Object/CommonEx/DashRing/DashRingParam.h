#pragma once
#include "Patches.h"

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

    static void paramMap_DashRing(app::SetEd::CResClass rClass)
    {
        TestingGrounds::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "KeepVelocityDistance", "", 8, 50.0f, 0.0f, 1000000.0f, 1.0f);
        TestingGrounds::AddParamEnum(rClass, "Type", "", 12, nullptr, 0, 0);
    }
}