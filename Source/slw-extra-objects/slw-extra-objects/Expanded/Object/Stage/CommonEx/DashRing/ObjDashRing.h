#pragma once

namespace slw_extra_objects
{
    class ObjDashRing : public app::CSetObjectListener
    {
    private:
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 12.0f;
        inline static const float ms_CollisionHeight = 1.0f;
        inline static const float ms_SquashEndTime = 0.7f;
        inline static const char* ms_SoundNames[] = { "obj_dashring", "obj_rainbowring" };

    protected:
        float FirstSpeed{};
        float OutOfControl{};
        float KeepVelocityDistance{};
        SDashRingParam::EType Type{};
        float ElapsedTime{};
        float DoSquash{};

    public:
        ObjDashRing();
        void AddCallback(app::GameDocument& in_rDocument) override;
        bool ProcessMessage(app::fnd::Message& in_rMessage) override;
        void Update(const app::fnd::SUpdateInfo& in_rUpdateInfo) override;
        bool ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage);
        csl::math::Vector3 GetDirectionVector();
    };
}