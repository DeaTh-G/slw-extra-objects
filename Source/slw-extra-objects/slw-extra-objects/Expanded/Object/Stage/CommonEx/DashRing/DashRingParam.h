#pragma once

namespace slw_extra_objects
{
    struct SDashRingParam
    {
        enum EType
        {
            eType_Dash,
            eType_Rainbow
        };

        float FirstSpeed{};
        float OutOfControl{};
        float KeepVelocityDistance{};
        EType Type{};
    };

    static void paramMap_DashRing(app::SetEd::CResClass rClass);
}