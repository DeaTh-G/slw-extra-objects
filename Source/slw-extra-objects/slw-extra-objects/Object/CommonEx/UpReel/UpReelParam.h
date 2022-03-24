#pragma once
#include "Patches\Functions.h"

namespace app
{
    struct SUpReelParam
    {
        float m_Length{};
    };

    static void paramMap_UpReel(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamFloat(rClass, "Length", "", 0, 30.0f, 0.0f, 1000000.0f, 1.0f);
    }
}