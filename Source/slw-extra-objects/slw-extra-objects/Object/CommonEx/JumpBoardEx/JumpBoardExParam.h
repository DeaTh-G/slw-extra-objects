#pragma once
#include "Patches\Functions.h"

namespace app
{

    struct SJumpBoardExParam
    {
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
        SLW_EXTRA_OBJECTS::AddParamEnum(rClass, "Type", "", 0, nullptr, 0, 0);
    }
}