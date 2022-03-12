#pragma once
#include "Patches.h"

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
        TestingGrounds::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "KeepVelocityDistance", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "Pitch", "", 8, 45.0f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamTarget(rClass, "Target", "", 12);
        TestingGrounds::AddParamEnum(rClass, "Type", "", 16, nullptr, 0, 0);
    }
}