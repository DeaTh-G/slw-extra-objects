#pragma once

namespace slw_extra_objects
{
    class ObjUpReel : public app::CSetObjectListener, public app::TTinyFsm<ObjUpReel>
    {
    protected:
        inline static const size_t ms_ModelCount = 3;
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 6.0f;

    public:
        float Length{};
        float ImpulseVelocity{};
        float OutOfControl{};
        float UpSpeedMax{};
        bool IsOneTimeUp{};
        int PlayerNo{};
        float PositionOffset{};
        float Time{};

    public:
        ObjUpReel();
        void AddCallback(app::GameDocument& in_rDocument) override;
        bool ProcessMessage(app::fnd::Message& in_rMessage) override;
        bool ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage);
        bool ProcMsgPLGetHomingTargetInfo(app::xgame::MsgPLGetHomingTargetInfo& in_rMessage);
        bool ProcMsgGetExternalMovePosition(app::xgame::MsgGetExternalMovePosition& in_rMessage);
        bool ProcMsgNotifyObjectEvent(app::xgame::MsgNotifyObjectEvent& in_rMessage);
        void Update(const app::fnd::SUpdateInfo& in_rUpdateInfo) override;
        void ChangeState(TiFsmState_t in_state);
        TiFsmState_t StateIdle(const TiFsmEvent_t& in_rEvent);
        TiFsmState_t StateWaitUp(const TiFsmEvent_t& in_rEvent);
        TiFsmState_t StateUp(const TiFsmEvent_t& in_rEvent);
        TiFsmState_t StateDown(const TiFsmEvent_t& in_rEvent);
        csl::math::Vector3 GetForwardDirectionVector(float in_up, float in_forward);
        void MovePlayerWithHandle(csl::math::Matrix34* in_pTransform);
        void SetWireLength(float in_Length);
        void SetHandleDistance(float in_Distance);
    };
}