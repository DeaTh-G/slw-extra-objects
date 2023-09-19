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

    static void paramMap_JumpBoardEx(app::SetEd::CResClass rClass)
    {
        slw_extra_objects::AddParamFloat(rClass, "ImpulseSpeedOnSpindash", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "ImpulseSpeedOnNormal", "", 4, 200.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "OutOfControl", "", 8, 200.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamBool(rClass, "IsStand", "", 12, true);
        slw_extra_objects::AddParamEnum(rClass, "Type", "", 13, nullptr, 0, 0);
    }
}