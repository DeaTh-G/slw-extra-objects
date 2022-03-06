#pragma once
#include "Patches.h"

namespace app
{
    struct SuperRingSuperParam
    {
        float m_Scale{};
    };

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
        float m_Time;

    public:
        ObjSuperRingSuper()
        {
            ObjUtil::SetPropertyLockonTarget(this);
        }

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCEffect>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);
            
            ObjSuperRingSuperInfo* info = ObjUtil::GetObjectInfo<ObjSuperRingSuperInfo>(document, "ObjSuperRingSuperInfo");
            SuperRingSuperParam* param = reinterpret_cast<SuperRingSuperParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto gocVisual = GetComponent<fnd::GOCVisualModel>();
            if (gocVisual)
            {
                fnd::GOCVisualModel::Description description {};
                description.m_Model = info->m_Model;

                gocVisual->Setup(description);

                csl::math::Vector3 scale = csl::math::Vector3(param->m_Scale, param->m_Scale, param->m_Scale);
                TestingGrounds::GOCVisualTransformed::SetLocalScale(gocVisual, &scale);
            }

            auto ringManager = document.GetService<Gimmick::CRingManager>();
            ringManager->RegisterRotateRing(this);
            
            auto gocCollider = GetComponent<game::GOCCollider>();
            if (gocCollider)
            {
                game::GOCCollider::Description description{ 2 };
                gocCollider->Setup(description);

                game::ColliSphereShapeCInfo colliInfo;
                colliInfo.m_Radius = 5;
                colliInfo.m_Unk2 |= 1;
                colliInfo.m_Unk3 = 0x20000;
                
                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, colliInfo);
                gocCollider->CreateShape(colliInfo);

                colliInfo.m_Unk4 = 0;
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

            switch (msg.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:
                return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(msg));
            case xgame::MsgPLGetHomingTargetInfo::MessageID:
                return ProcMsgPLGetHomingTargetInfo(reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(msg));
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
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

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& message)
        {
            auto gocTransform = GetComponent<fnd::GOCTransform>();
            message.m_LockonCount = 1;
            message.m_CursorPosition = gocTransform->GetLocalPosition();

            return true;
        }
    };

    static void paramMap_SuperRingSuper(app::SetEd::CResClass rClass)
    {
        TestingGrounds::AddParamFloat(rClass, "Scale", "Scale of the object.", 0, 10.0f, 0.0f, 10000.0f, 1.0f);
    }
}