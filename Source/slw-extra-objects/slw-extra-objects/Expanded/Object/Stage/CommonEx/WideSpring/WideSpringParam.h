#pragma once

namespace slw_extra_objects
{
    struct SWideSpringParam
    {
        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};
        bool IsEventOn{};
    };

    static void paramMap_WideSpring(app::SetEd::CResClass rClass);
}