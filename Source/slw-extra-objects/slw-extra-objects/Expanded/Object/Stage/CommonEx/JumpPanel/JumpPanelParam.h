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

    static void paramMap_JumpPanel(app::SetEd::CResClass rClass)
    {
        slw_extra_objects::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "KeepVelocityDistance", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "OutOfControl", "", 8, 0.25f, 0.0f, 10000.0f, 1.0f);
        slw_extra_objects::AddParamFloat(rClass, "Pitch", "", 12, 45.0f, 0.0f, 10000.0f, 1.0f);
        slw_extra_objects::AddParamTarget(rClass, "Target", "", 16);
        slw_extra_objects::AddParamEnum(rClass, "Type", "", 20, nullptr, 0, 0);
    }
}