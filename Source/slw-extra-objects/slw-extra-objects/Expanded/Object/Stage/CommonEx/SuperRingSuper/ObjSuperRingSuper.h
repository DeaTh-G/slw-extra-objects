#pragma once

namespace slw_extra_objects
{
    class ObjSuperRingSuper : public app::CSetObjectListener
    {
    protected:
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 5.0f;
        inline static const char* ms_pEffectName = "ef_ob_com_yh1_ringget_ss";
        inline static const char* ms_pSoundName = "obj_superring";

    public:
        ObjSuperRingSuper();
        void AddCallback(app::GameDocument& in_rDocument) override;
        bool ProcessMessage(app::fnd::Message& in_rMessage) override;
        bool ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage);
    };
}