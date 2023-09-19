#pragma once

namespace slw_extra_objects
{
    class ObjJumpPanel : public app::CSetObjectListener
    {
    private:
        inline static const size_t ms_TextureAnimationCount = 3;
        inline static const csl::math::Vector3 ms_CollisionSizes[] = { { 8.6f, 0.85f, 19.6f }, { 8.6f, 0.85f, 5.5f }, { 8.6f, 0.85f, 15.2f } };
        inline static const csl::math::Vector3 ms_CollisionOffsets[] = { { 0.0f, 0.0f, 19.0f }, { 0.0f, 1.4f, 4.8f }, { 0.0f, 9.2f, 23.755f } };
        inline static const float ms_UpRotations[] = { -15.0f, -25.0f };
        inline static const float ms_LaunchOffsets[] = { -5.0f, -30.0f };
        inline static const char* ms_pSoundName = "obj_dashpanel";

    protected:
        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};
        float Pitch{};
        app::CSetObjectID TargetID{};
        SJumpPanelParam::EType Type{};
        bool IsOn{ true };
        float ElapsedTime{};

    public:
        ObjJumpPanel();
        void AddCallback(app::GameDocument& in_rDocument) override;
        bool ProcessMessage(app::fnd::Message& in_rMessage) override;
        void Update(const app::fnd::SUpdateInfo& in_rUpdateInfo) override;
        bool ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage);
        csl::math::Vector3 GetDirectionVector();
        csl::math::Quaternion GetLaunchOffset();
        csl::math::Quaternion GetPitchCorrection();
    };
}