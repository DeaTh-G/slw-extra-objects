#pragma once
#include "Patches\Functions.h"

namespace app
{
    struct SJumpPanelParam
    {
        enum EType
        {
            eType_Normal,
            eType_Upwards
        };

        float m_FirstSpeed{};
        float m_KeepVelocityDistance{};
        float m_OutOfControl{};
        float m_Pitch{};
        CSetObjectID m_TargetID{};
        EType m_Type{};
    };

    static void paramMap_JumpPanel(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "KeepVelocityDistance", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "Pitch", "", 8, 45.0f, 0.0f, 10000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamTarget(rClass, "Target", "", 12);
        SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 16, nullptr, 0, 0);
    }
}