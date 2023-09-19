#pragma once

namespace slw_extra_objects
{
    class ObjWideSpring : public app::CSetObjectListener
    {
    protected:
        inline static const size_t ms_AnimationCount = 1;
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 6.0f;
        inline static const float ms_CollisionHeight = 12.0f;
        inline static const csl::math::Vector3 ms_CollisionOffset = { 0.0f, 3.5f, 0.0f };
        inline static const csl::math::Quaternion ms_CollisionRotation = { Eigen::AngleAxisf(TO_RAD(90.0f), csl::math::Vector3::UnitZ()) };
        inline static const char* ms_pAnimationName = "HIT";
        inline static const char* ms_pSoundName = "obj_spring";

    public:
        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};

    public:
        ObjWideSpring();
        void AddCallback(app::GameDocument& in_rDocument) override;
        bool ProcessMessage(app::fnd::Message& in_rMessage) override;
        bool ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage);
        bool ProcMsgPLGetHomingTargetInfo(app::xgame::MsgPLGetHomingTargetInfo& in_rMessage);
        bool ProcMsgNotifyObjectEvent(app::xgame::MsgNotifyObjectEvent& in_rMessage);
        csl::math::Vector3 GetDirectionVector();
    };
}