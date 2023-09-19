#pragma once

namespace slw_extra_objects
{
    struct SUpReelParam
    {
        float Length{};
        bool IsWaitUp{};
        bool IsOneTimeUp{};
        float ImpulseVelocity{};
        float OutOfControl{};
        float UpSpeedMax{};
    };

    static void paramMap_UpReel(app::SetEd::CResClass rClass)
    {
        slw_extra_objects::AddParamFloat(rClass, "Length", "", 0, 30.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamBool(rClass, "IsWaitUp", "", 4, false);
        slw_extra_objects::AddParamBool(rClass, "IsOneTimeUp", "", 5, false);
        slw_extra_objects::AddParamFloat(rClass, "ImpulseVelocity", "", 8, 250.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "OutOfControl", "", 12, 0.25f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "UpSpeedMax", "", 16, 300.0f, 0.0f, 1000000.0f, 1.0f);
    }
}