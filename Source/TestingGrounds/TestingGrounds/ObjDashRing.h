#pragma once
#include "Patches.h"

namespace app
{
    struct SDashRingParam
    {
        enum EType
        {
            eType_Dash,
            eType_Rainbow
        };

        float m_FirstSpeed{};
        float m_OutOfControl{};
        float m_KeepVelocityDistance{};
        EType m_Type{};
    };

    class ObjDashRingInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_dashring", "cmn_obj_rainbowring" };
        inline static const char* ms_AnimationNames[] = {
            "cmn_obj_dashring-0000", "cmn_obj_dashring-0001", "cmn_obj_dashring-0002", "cmn_obj_dashring-0003",
            "cmn_obj_rainbowring-0000", "cmn_obj_rainbowring-0001", "cmn_obj_rainbowring-0002", "cmn_obj_rainbowring-0003"
        };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);
        inline static size_t ms_AnimCount = ARRAYSIZE(ms_AnimationNames);

        hh::gfx::res::ResModel m_Models[sizeof(ms_ModelNames) / sizeof(ms_ModelNames[0])]{};
        hh::gfx::res::ResAnimTexSrt m_TextureAnimations[sizeof(ms_ModelNames) / sizeof(ms_ModelNames[0])]{};
        hh::gfx::res::ResAnimMaterial m_MaterialAnimations[sizeof(ms_AnimationNames) / sizeof(ms_AnimationNames[0])]{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");

            for (size_t i = 0; i < ms_ModelCount; i++)
            {
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);
                m_MaterialAnimations[i] = ObjUtil::GetMaterialAnimationResource(ms_ModelNames[i], packFile);
            }

            for (size_t i = 0; i < ms_AnimCount; i++)
                m_TextureAnimations[i] = ObjUtil::GetTexSrtAnimationResource(ms_AnimationNames[i], packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjDashRingInfo";
        }
    };

    class ObjDashRing : public CSetObjectListener
    {
    private:
        inline static const char* ms_SoundNames[] = { "obj_dashring", "obj_rainbowring" };

    protected:
        float m_Time{};
        float m_DoSquash{};

    public:
        ObjDashRing()
        {
            SetUpdateFlag(0, true);
        }

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto pInfo = ObjUtil::GetObjectInfo<ObjDashRingInfo>(document, "ObjDashRingInfo");
            auto pParam = reinterpret_cast<SDashRingParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto gocVisual = GetComponent<fnd::GOCVisualModel>();
            if (gocVisual)
            {
                char type = (char)pParam->m_Type;
                size_t modelAnimCount = ObjDashRingInfo::ms_AnimCount / ObjDashRingInfo::ms_ModelCount;

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[type];

                gocVisual->Setup(description);
                
                gocVisual->SetMaterialAnimation({ pInfo->m_MaterialAnimations[type], 1 });

                auto pBlender = gocVisual->SetTexSrtBlender({ modelAnimCount });
                for (size_t i = 0; i < modelAnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TextureAnimations[type * 4 + i], 1 });
            }

            auto gocCollider = GetComponent<game::GOCCollider>();
            if (gocCollider)
            {
                game::GOCCollider::Description description{ 1 };
                gocCollider->Setup(description);

                game::ColliCylinderShapeCInfo colliInfo{};
                colliInfo.m_Radius = 12.0f;
                colliInfo.m_Height = 1.0f;
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalRotation(Eigen::Quaternion<float>(Eigen::AngleAxis<float>(MATHF_PI / 2, Eigen::Vector3f::UnitX())));

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                gocCollider->CreateShape(colliInfo);
            }

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

        void Update(const fnd::SUpdateInfo& rInfo) override
        {
            if (!m_DoSquash)
                return;

            m_Time += rInfo.deltaTime;
            if (m_Time >= 0.7f)
            {
                m_DoSquash = false;
                m_Time = 0;
                return;
            }

            float scale = cos(m_Time * 1.4285715f * 3.1415927f * 2.0f);
            GetComponent<fnd::GOCVisualModel>()->SetLocalScale({ scale, scale, 1 });
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            auto gocSound = GetComponent<game::GOCSound>();
            auto gocTransform = GetComponent<fnd::GOCTransform>();

            auto pParam = reinterpret_cast<SDashRingParam*>(m_pAdapter->GetData());
            float speed = pParam->m_KeepVelocityDistance / pParam->m_FirstSpeed;
            char type = (char)pParam->m_Type;

            int deviceTag[3];
            TestingGrounds::GOCSound::Play3D(gocSound, deviceTag, ms_SoundNames[type], 0);

            csl::math::Vector3 direction{ gocTransform->GetLocalRotation() * Eigen::Vector3f::UnitZ() * pParam->m_FirstSpeed };
            xgame::MsgSpringImpulse impulseMsg{ gocTransform->GetLocalPosition(), direction, pParam->m_OutOfControl, speed };
            impulseMsg.field_50.set(18);
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);
            
            m_DoSquash = true;
            return true;
        }
    };

    static void paramMap_DashRing(app::SetEd::CResClass rClass)
    {
        TestingGrounds::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "KeepVelocityDistance", "", 8, 50.0f, 0.0f, 1000000.0f, 1.0f);
        TestingGrounds::AddParamEnum(rClass, "Type", "", 12, nullptr, 0, 0);
    }
}