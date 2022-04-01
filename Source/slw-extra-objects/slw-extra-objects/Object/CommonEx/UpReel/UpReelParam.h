#pragma once
#include "Patches\Functions.h"

namespace app
{
    struct SUpReelParam
    {
        float m_Length{};
        bool m_IsWaitUp{};
        bool m_IsOneTimeUp{};
        float m_ImpulseVelocity{};
        float m_OutOfControl{};
        float m_UpSpeedMax{};
    };

    static void paramMap_UpReel(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "Length", "", 0, 30.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamBool(rClass, "IsWaitUp", "", 4, false);
        SLW_EXTRA_OBJECTS::AddParamBool(rClass, "IsOneTimeUp", "", 5, false);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "ImpulseVelocity", "", 8, 250.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 12, 0.25f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "UpSpeedMax", "", 16, 300.0f, 0.0f, 1000000.0f, 1.0f);
    }
}