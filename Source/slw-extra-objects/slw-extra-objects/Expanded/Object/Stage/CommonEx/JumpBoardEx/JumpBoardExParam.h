#pragma once

namespace slw_extra_objects
{
    struct SJumpBoardExParam
    {
        enum EType : char
        {
            eType_15Small,
            eType_30Small,
            eType_30Medium,
            eType_30Large
        };

        float ImpulseSpeedOnSpindash{};
        float ImpulseSpeedOnNormal{};
        float OutOfControl{};
        bool IsStand{};
        EType Type{};
    };

    static void paramMap_JumpBoardEx(app::SetEd::CResClass rClass);
}