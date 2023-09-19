#pragma once
#include "Patches\Functions.h"

namespace app
{
    class ObjSuperRingSuper : public CSetObjectListener
    {
    protected:
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 5.0f;
        inline static const char* ms_pEffectName = "ef_ob_com_yh1_ringget_ss";
        inline static const char* ms_pSoundName = "obj_superring";

    public:
        void AddCallback(GameDocument& in_rDocument) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCEffect>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);
            
            auto* pInfo = ObjUtil::GetObjectInfo<ObjSuperRingSuperInfo>(in_rDocument);

            fnd::GOComponent::BeginSetup(*this);

            if (auto* pVisualGoc = GetComponent<fnd::GOCVisualModel>())
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->Model;

                pVisualGoc->Setup(description);
            }

            auto* pManager = in_rDocument.GetService<Gimmick::CRingManager>();
            pManager->RegisterRotateRing(this);
            
            if (auto* pColliderGoc = GetComponent<game::GOCCollider>())
            {
                game::GOCCollider::Description description{ ms_ShapeCount };
                pColliderGoc->Setup(description);

                game::ColliSphereShapeCInfo collisionInfo;
                collisionInfo.m_Radius = ms_CollisionRadius;
                collisionInfo.m_Unk2 |= 1;
                collisionInfo.m_Unk3 = 0x20000;
                
                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, collisionInfo);
                pColliderGoc->CreateShape(collisionInfo);
            }

            game::GOCEffect::SimpleSetup(this, 0, false);
            game::GOCSound::SimpleSetup(this, 0, 0);

            fnd::GOComponent::EndSetup(*this);
        }

        bool ProcessMessage(fnd::Message& in_rMessage) override
        {
            if (PreProcessMessage(in_rMessage))
                return true;

            switch (in_rMessage.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:        return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(in_rMessage));
            default:                                            return CSetObjectListener::ProcessMessage(in_rMessage);
            }
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& in_rMessage)
        {
            xgame::MsgTakeObject msg{ (xgame::MsgTakeObject::EType)19 };
            msg.SetShapeUserID(in_rMessage.m_pOther->m_ID);
            SendMessageImm(in_rMessage.m_Sender, msg);
            if (!msg.m_Taken)
                return false;

            GetComponent<game::GOCEffect>()->CreateEffect(ms_pEffectName);
            GetComponent<game::GOCSound>()->Play3D(ms_pSoundName, 0.0f);

            SetStatusRetire();
            Kill();
            return true;
        }
    };
}