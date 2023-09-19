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

    static void paramMap_UpReel(app::SetEd::CResClass rClass);
}