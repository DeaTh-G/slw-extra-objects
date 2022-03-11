#pragma once
#include "Patches.h"

namespace app
{
    struct SJumpPanelParam
    {
        enum EType
        {
            eType_Normal,
            eType_Upwards
        };

        float m_FirstSpeed{};
        float m_KeepVelocityDistance{};
        float m_OutOfControl{};
        float m_Pitch{};
        CSetObjectID m_TargetID{};
        EType m_Type{};
    };

    class ObjJumpPanelInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_jumppanel", "cmn_obj_jumppanelU" };
        inline static const char* ms_AnimationNames[] = { "cmn_obj_jumppanel_arrow-0000", "cmn_obj_jumppanel_arrow-0001", "cmn_obj_jumppanel_arrow-0002" };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);
        inline static size_t ms_AnimCount = ARRAYSIZE(ms_AnimationNames);

        hh::gfx::res::ResModel m_Models[ARRAYSIZE(ms_ModelNames)]{};
        hh::gfx::res::ResAnimTexSrt m_TextureAnimations[ARRAYSIZE(ms_ModelNames)]{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");

            for (size_t i = 0; i < ms_ModelCount; i++)
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);

            for (size_t i = 0; i < ms_AnimCount; i++)
                m_TextureAnimations[i] = ObjUtil::GetTexSrtAnimationResource(ms_AnimationNames[i], packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjJumpPanelInfo";
        }
    };

    class ObjJumpPanel : public CSetObjectListener
    {
    public:
        ObjJumpPanel() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto pInfo = ObjUtil::GetObjectInfo<ObjJumpPanelInfo>(document, "ObjJumpPanelInfo");
            auto pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto gocVisual = GetComponent<fnd::GOCVisualModel>();
            if (gocVisual)
            {
                char type = (char)pParam->m_Type;

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[type];

                gocVisual->Setup(description);

                auto pBlender = gocVisual->SetTexSrtBlender({ ObjJumpPanelInfo::ms_AnimCount });

                for (size_t i = 0; i < ObjJumpPanelInfo::ms_AnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TextureAnimations[type * 4 + i], 1 });
            }

            auto gocCollider = GetComponent<game::GOCCollider>();
            if (gocCollider)
            {
                game::GOCCollider::Description description{ 1 };
                gocCollider->Setup(description);

                game::ColliBoxShapeCInfo colliInfo;
                colliInfo.m_Size = csl::math::Vector3(14.6f, 0.85f, 19.6f);
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalPosition({ 0.0f, 0.0f, 19.0f });

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

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            auto gocSound = GetComponent<game::GOCSound>();
            auto gocTransform = GetComponent<fnd::GOCTransform>();

            auto pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());
            float speed = pParam->m_KeepVelocityDistance / pParam->m_FirstSpeed;
            auto position = gocTransform->GetLocalPosition();

            int deviceTag[3];
            TestingGrounds::GOCSound::Play3D(gocSound, deviceTag, "obj_dashpanel", 0);

            csl::math::Vector3 direction{};
            if (!pParam->m_TargetID)
            {
                auto angle = gocTransform->GetLocalRotation() * Eigen::Quaternion<float>(Eigen::AngleAxis<float>(pParam->m_Pitch * MATHF_PI / 180, Eigen::Vector3f::UnitY()));
                direction = static_cast<csl::math::Vector3>(angle * Eigen::Vector3f::UnitZ() * pParam->m_FirstSpeed);
            }
            else
            {
                csl::math::Vector3 targetPosition{};
                csl::math::Quaternion targetRotation{};
                TestingGrounds::ObjUtil::GetSetObjectTransform(m_pOwnerDocument, pParam->m_TargetID, &targetPosition, &targetRotation);

                direction = csl::math::Vector3(targetPosition.x() - position.x(), targetPosition.y() - position.y(), targetPosition.z() - position.z());
            }
            xgame::MsgSpringImpulse impulseMsg{ position, direction, pParam->m_OutOfControl, speed };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }
    };

    static void paramMap_JumpPanel(app::SetEd::CResClass rClass)
    {
        TestingGrounds::AddParamFloat(rClass, "FirstSpeed", "", 0, 200.0f, 0.0f, 1000000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "KeepVelocityDistance", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "OutOfControl", "", 4, 0.25f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamFloat(rClass, "Pitch", "", 8, 45.0f, 0.0f, 10000.0f, 1.0f);
        TestingGrounds::AddParamTarget(rClass, "Target", "", 12);
        TestingGrounds::AddParamEnum(rClass, "Type", "", 16, nullptr, 0, 0);
    }
}