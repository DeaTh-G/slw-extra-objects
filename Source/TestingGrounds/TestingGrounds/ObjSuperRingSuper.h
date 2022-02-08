#pragma once
#include "Patches.h"

namespace app
{
    class ObjSuperRingSuperInfo : public CObjInfo
    {
    public:
        hh::gfx::res::ResModel m_Model;

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");
            m_Model = ObjUtil::GetModelResource("cmn_obj_super_ring_super", packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjSuperRingSuperInfo";
        }
    };

    class ObjSuperRingSuper : public CSetObjectListener
    {
    public:
        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCEffect>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);
            
            ObjSuperRingSuperInfo* info = ObjUtil::GetObjectInfo<ObjSuperRingSuperInfo>(document, "ObjSuperRingSuperInfo");

            fnd::GOComponent::BeginSetup(*this);

            auto gocVisual = GetComponent<fnd::GOCVisualModel>();
            if (gocVisual)
            {
                fnd::GOCVisualModel::Description description {};
                description.m_Model = info->m_Model;

                gocVisual->Setup(description);
            }

            auto ringManager = document.GetService<Gimmick::CRingManager>();
            ringManager->RegisterRotateRing(this);
            
            auto gocCollider = GetComponent<game::GOCCollider>();
            if (gocCollider)
            {
                game::GOCCollider::Description description{ 1 };
                gocCollider->Setup(description);

                game::ColliSphereShapeCInfo colliInfo;
                colliInfo.m_Radius = 5;
                colliInfo.m_Unk2 |= 1;
                
                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                gocCollider->CreateShape(colliInfo);
            }

            game::GOCEffect::SimpleSetup(this, 1, false);
            game::GOCSound::SimpleSetup(this, 0, 0);

            fnd::GOComponent::EndSetup(*this);
        }

        bool ProcessMessage(fnd::Message& msg) override
        {
            if (PreProcessMessage(msg))
                return true;

            if (msg.IsOfType<xgame::MsgHitEventCollision>())
                return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(msg));

            return CSetObjectListener::ProcessMessage(msg);
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            xgame::MsgTakeObject takeMsg{ (xgame::MsgTakeObject::EType)19 };
            takeMsg.SetShapeUserID(msg.m_pOther->m_ID);
            SendMessageImm(msg.m_Sender, takeMsg);
            if (!takeMsg.m_Taken)
                return false;

            auto gocEffect = GetComponent<game::GOCEffect>();
            if (gocEffect)
                gocEffect->CreateEffect("ef_ob_com_yh1_ringget_ss");

            auto gocSound = GetComponent<game::GOCSound>();
            if (gocSound)
                gocSound->Play("obj_superring", 0);

            SetStatusRetire();
            Kill();
            return true;
        }
    };
}