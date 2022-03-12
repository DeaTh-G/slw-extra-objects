#pragma once
#include "Patches\Functions.h"

namespace app
{

    struct SJumpBoardExParam
    {
        enum EType
        {
            eType_SmallSmall,
            eType_LargeSmall,
            eType_LargeMedium,
            eType_LargeLarge
        };

        EType m_Type{};
    };

    static void paramMap_JumpBoardEx(SetEd::CResClass rClass)
    {
        SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 0, nullptr, 0, 0);
    }
}