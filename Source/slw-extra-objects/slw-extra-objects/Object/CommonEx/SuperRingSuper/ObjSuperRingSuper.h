#pragma once
#include "Patches\Functions.h"

namespace app
{
    class ObjSuperRingSuper : public CSetObjectListener
    {
    public:
        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCEffect>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);
            
            auto* pInfo = ObjUtil::GetObjectInfo<ObjSuperRingSuperInfo>(document, "ObjSuperRingSuperInfo");

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Model;

                pVisual->Setup(description);
            }

            auto* pManager = document.GetService<Gimmick::CRingManager>();
            pManager->RegisterRotateRing(this);
            
            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                game::GOCCollider::Description description{ 1 };
                pCollider->Setup(description);

                game::ColliSphereShapeCInfo colliInfo;
                colliInfo.m_Radius = 5;
                colliInfo.m_Unk2 |= 1;
                colliInfo.m_Unk3 = 0x20000;
                
                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, colliInfo);
                pCollider->CreateShape(colliInfo);
            }

            game::GOCEffect::SimpleSetup(this, 0, false);
            game::GOCSound::SimpleSetup(this, 0, 0);

            fnd::GOComponent::EndSetup(*this);
        }

        bool ProcessMessage(fnd::Message& msg) override
        {
            if (PreProcessMessage(msg))
                return true;

            switch (msg.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:
                return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(msg));
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            auto* pSound = GetComponent<game::GOCSound>();

            xgame::MsgTakeObject takeMsg{ (xgame::MsgTakeObject::EType)19 };
            takeMsg.SetShapeUserID(msg.m_pOther->m_ID);
            SendMessageImm(msg.m_Sender, takeMsg);
            if (!takeMsg.m_Taken)
                return false;

            GetComponent<game::GOCEffect>()->CreateEffect("ef_ob_com_yh1_ringget_ss");

            int deviceTag[3];
            SLW_EXTRA_OBJECTS::GOCSound::Play3D(pSound, deviceTag, "obj_superring", 0);

            SetStatusRetire();
            Kill();
            return true;
        }
    };
}