#pragma once

namespace slw_extra_objects
{
    class ObjJumpBoardEx : public app::CSetObjectListener
    {
    private:
        inline static const size_t ms_TextureAnimationCount = 3;
        inline static const size_t ms_ShapeCount = 1;
        inline static const size_t ms_PhysicsShapeCount = 1;
        inline static const csl::math::Vector3 ms_CollisionSizes[] = { { 2.48f, 0.2f, 3.0f }, { 2.48f, 0.42f, 3.5f }, { 5.64f, 0.46f, 13.65f }, { 15.4f, 0.46f, 30.2f } };
        inline static const csl::math::Vector3 ms_CollisionOffsets[] = { { 0.0f, 2.672f, 7.585f }, { 0.0f, 4.622f, 7.397f }, { 0.0f, 7.176f, 11.148f }, { 0.0f, 16.403f, 24.368f } };
        inline static const float ms_CollisionRotations[] = { -15.0f, -30.0f, -30.0f, -30.0f };

    protected:
        float ImpulseSpeedOnSpindash{};
        float ImpulseSpeedOnNormal{};
        float OutOfControl{};
        bool IsStand{};
        SJumpBoardExParam::EType Type{};
        bool IsOn{ true };
        float ElapsedTime{};

    public:
        ObjJumpBoardEx();
        void AddCallback(app::GameDocument& in_rDocument) override;
        bool ProcessMessage(app::fnd::Message& in_rMessage) override;
        void Update(const app::fnd::SUpdateInfo& in_rUpdateInfo) override;
        bool ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage);
        bool CheckPlayerVelocity(int in_playerNo);
        csl::math::Vector3 GetDirectionVector(int in_playerNo);
        csl::math::Quaternion GetLaunchOffset();
    };
}