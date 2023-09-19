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

    static void paramMap_DashRing(app::SetEd::CResClass rClass)
    {
        slw_extra_objects::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "KeepVelocityDistance", "", 8, 50.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamEnum(rClass, "Type", "", 12, nullptr, 0, 0);
    }
}