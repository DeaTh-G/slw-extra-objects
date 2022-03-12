#pragma once
#include "Patches.h"

using namespace csl::math;

namespace app
{
    class ObjJumpPanel : public CSetObjectListener
    {
    public:
        ObjJumpPanel() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjJumpPanelInfo>(document, "ObjJumpPanelInfo");
            auto* pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                char type = (char)pParam->m_Type;

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[type];

                pVisual->Setup(description);

                auto* pBlender = pVisual->SetTexSrtBlender({ ObjJumpPanelInfo::ms_AnimCount });

                for (size_t i = 0; i < ObjJumpPanelInfo::ms_AnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TextureAnimations[i], 1 });
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                game::GOCCollider::Description description{ 1 };
                pCollider->Setup(description);

                game::ColliBoxShapeCInfo colliInfo;
                colliInfo.m_Size = csl::math::Vector3(14.6f, 0.85f, 19.6f);
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalPosition({ 0.0f, 0.0f, 19.0f });

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                pCollider->CreateShape(colliInfo);
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

            auto* pSound = GetComponent<game::GOCSound>();
            auto* pTransform = GetComponent<fnd::GOCTransform>();

            auto* pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());
            float speed = pParam->m_KeepVelocityDistance / pParam->m_FirstSpeed;

            int deviceTag[3];
            TestingGrounds::GOCSound::Play3D(pSound, deviceTag, "obj_dashpanel", 0);

            xgame::MsgSpringImpulse impulseMsg{ pTransform->GetLocalPosition(), GetDirectionVector(), pParam->m_OutOfControl, speed };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        Vector3 GetDirectionVector()
        {
            auto* pTransform = GetComponent<fnd::GOCTransform>();
            auto* pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());

            if (!pParam->m_TargetID)
            {
                auto angle = pTransform->GetLocalRotation() * Eigen::Quaternionf(Eigen::AngleAxisf(pParam->m_Pitch * MATHF_PI / 180, Eigen::Vector3f::UnitY()));
                return static_cast<Vector3>(angle * Eigen::Vector3f::UnitZ() * pParam->m_FirstSpeed);
            }
            else
            {
                Vector3 targetPosition{};
                Quaternion targetRotation{};
                TestingGrounds::ObjUtil::GetSetObjectTransform(m_pOwnerDocument, pParam->m_TargetID, &targetPosition, &targetRotation);

                return static_cast<Vector3>(targetPosition - pTransform->GetLocalPosition());
            }
        }
    };
}