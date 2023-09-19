#pragma once

namespace slw_extra_objects
{
    struct SJumpPanelParam
    {
        enum EType : size_t
        {
            eType_Normal,
            eType_Upwards
        };

        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};
        float Pitch{};
        app::CSetObjectID TargetID{};
        EType Type{};
    };

    static void paramMap_JumpPanel(app::SetEd::CResClass rClass);
}