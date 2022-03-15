#pragma once
#include "Patches\Functions.h"

namespace app
{

    struct SJumpBoardExParam
    {
        float m_ImpulseSpeedOnSpindash;
        float m_ImpulseSpeedOnNormal;
        float m_OutOfControl;

        enum EType
        {
            eType_15Small,
            eType_30Small,
            eType_30Medium,
            eType_30Large
        };

        EType m_Type{};
    };

    static void paramMap_JumpBoardEx(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "ImpulseSpeedOnSpindash", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "ImpulseSpeedOnNormal", "", 4, 200.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "OutOfControl", "", 8, 200.0f, 0.0f, 1000000.0f, 1.0f);
        SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 12, nullptr, 0, 0);
    }
}