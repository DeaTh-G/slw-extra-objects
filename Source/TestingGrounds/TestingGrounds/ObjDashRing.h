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
    public:
        hh::gfx::res::ResModel m_Models[2]{};
        hh::gfx::res::ResAnimTexSrt m_TextureAnimations[2]{};
        hh::gfx::res::ResAnimMaterial m_MaterialAnimations[2]{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");

            for (size_t i = 0; i < 2; i++)
            {
                const char* fileName = "cmn_obj_dashring";
                if (i == 1)
                    fileName = "cmn_obj_rainbowring";

                m_Models[i] = ObjUtil::GetModelResource(fileName, packFile);
                m_TextureAnimations[i] = ObjUtil::GetTexSrtAnimationResource(fileName, packFile);
                m_MaterialAnimations[i] = ObjUtil::GetMaterialAnimationResource(fileName, packFile);
            }
        }

        const char* GetInfoName() override
        {
            return "ObjDashRingInfo";
        }
    };

    class ObjDashRing : public CSetObjectListener
    {
    protected:
        float m_Time{};
        float m_DoSquash{};

    public:
        ObjDashRing()
        {
            SetUpdateFlag(0, true);
            ObjUtil::SetPropertyLockonTarget(this);
        }

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            ObjDashRingInfo* pInfo = ObjUtil::GetObjectInfo<ObjDashRingInfo>(document, "ObjDashRingInfo");
            SDashRingParam* pParam = reinterpret_cast<SDashRingParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto gocVisual = GetComponent<fnd::GOCVisualModel>();
            if (gocVisual)
            {
                char type = (char)pParam->m_Type;

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[type];

                gocVisual->Setup(description);
                
                fnd::MatAnimBlenderDesc matAnimDesc{ 1 };
                auto blender = gocVisual->SetMatAnimBlender(matAnimDesc);

                fnd::MatAnimControlDesc controlDesc{ pInfo->m_MaterialAnimations[type], 1 };
                auto control = blender->CreateControl(controlDesc);

                fnd::TexSrtDesc texAnimDesc{ pInfo->m_TextureAnimations[type], 1};
                gocVisual->SetTexSrtAnimation(texAnimDesc);
            }

            auto gocCollider = GetComponent<game::GOCCollider>();
            if (gocCollider)
            {
                game::GOCCollider::Description description{ 1 };
                gocCollider->Setup(description);

                game::ColliCylinderShapeCInfo colliInfo;
                colliInfo.m_Radius = 12.0f;
                colliInfo.m_Height = 1.0f;
                colliInfo.m_Unk2 |= 1;

                Eigen::Quaternion<float> rotation(Eigen::AngleAxis<float>(MATHF_PI / 2, Eigen::Vector3f::UnitX()));
                colliInfo.SetLocalRotation(rotation);

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

        void Update(const fnd::SUpdateInfo& info) override
        {
            if (!m_DoSquash)
                return;

            m_Time += info.deltaTime;
            if (m_Time >= 0.7f)
            {
                m_DoSquash = false;
                m_Time = 0;
                return;
            }

            float scale = cos(m_Time * 1.4285715f * 3.1415927 * 2.0f);
            csl::math::Vector3 scaleVector{ scale, scale, 1 };

            auto gocVisual = GetComponent<fnd::GOCVisualModel>();
            TestingGrounds::GOCVisualTransformed::SetLocalScale(gocVisual, &scaleVector);
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            SDashRingParam* pParam = reinterpret_cast<SDashRingParam*>(m_pAdapter->GetData());

            int deviceTag[3];
            auto gocSound = GetComponent<game::GOCSound>();
            if (gocSound)
            {
                if (pParam->m_Type == SDashRingParam::eType_Dash)
                {
                    TestingGrounds::GOCSound::Play(gocSound, deviceTag, "obj_dashring", 0);
                }
                else
                {
                    TestingGrounds::GOCSound::Play(gocSound, deviceTag, "obj_rainbowring", 0);
                }
            }

            float finalSpeed = pParam->m_KeepVelocityDistance / pParam->m_FirstSpeed;

            auto gocTransform = GetComponent<fnd::GOCTransform>();
            auto position = gocTransform->GetLocalPosition();
            auto rotation = gocTransform->GetLocalRotation();

            csl::math::Vector3 forwardVector{ 0, 0, 1 };
            csl::math::Vector3 direction{};
            TestingGrounds::math::Vector3Rotate(&direction, &rotation, &forwardVector);
            
            csl::math::Vector3 destination{ direction.x() * pParam->m_FirstSpeed, direction.y() * pParam->m_FirstSpeed, direction.z() * pParam->m_FirstSpeed };
            xgame::MsgSpringImpulse impulseMsg{ position, destination, pParam->m_OutOfControl, finalSpeed };
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