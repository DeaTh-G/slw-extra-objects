#pragma once
#include "Patches\Functions.h"

using namespace csl::math;

namespace app
{
    class ObjJumpPanel : public CSetObjectListener
    {
    private:
        inline static Vector3 ms_JumpPanelSize{ 14.6f, 0.85f, 19.6f };
        inline static Vector3 ms_JumpPanelPosition{ 0.0f, 0.0f, 19.0f };
        inline static Vector3 ms_JumpPanelUSizes[] = { { 15.5f, 0.85f, 5.5f }, { 15.5f, 0.85f, 15.2f } };
        inline static Vector3 ms_JumpPanelUPositions[] = { { 0.0f, 1.4f, 4.8f }, { 0.0f, 9.2f, 23.755f } };
        inline static float ms_JumpPanelURotations[] = { -15.0f, -25.0f };
        inline static float ms_JumpPanelLaunchOffsets[] = { -5.0f, -30.0f };

    protected:
        bool m_IsOn{ true };
        float m_Time{};

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
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[pParam->m_Type];

                pVisual->Setup(description);

                auto* pBlender = pVisual->SetTexSrtBlender({ ObjJumpPanelInfo::ms_AnimCount });

                for (size_t i = 0; i < ObjJumpPanelInfo::ms_AnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TextureAnimations[i], 1 });
            }

            SetupCollider(pParam->m_Type);

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
            if (m_IsOn)
                return;

            m_Time += rInfo.deltaTime;
            if (m_Time >= 0.7f)
            {
                GetComponent<game::GOCCollider>()->SetEnable(true);
                m_Time = 0.0f;
                return;
            }
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            if (!m_IsOn)
                return false;

            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            auto* pSound = GetComponent<game::GOCSound>();
            auto* pTransform = GetComponent<fnd::GOCTransform>();

            auto* pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());
            float speedDropoffTime = pParam->m_KeepVelocityDistance / pParam->m_FirstSpeed;

            int deviceTag[3];
            SLW_EXTRA_OBJECTS::GOCSound::Play3D(pSound, deviceTag, "obj_dashpanel", 0);

            xgame::MsgSpringImpulse impulseMsg{ pTransform->GetLocalPosition(), GetDirectionVector(), pParam->m_OutOfControl, speedDropoffTime };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            GetComponent<game::GOCCollider>()->SetEnable(false);
            m_IsOn = false;

            return true;
        }

        void SetupCollider(SJumpPanelParam::EType type)
        {
            auto* pCollider = GetComponent<game::GOCCollider>();
            if (!pCollider)
                return;

            if (type == SJumpPanelParam::eType_Normal)
            {
                game::GOCCollider::Description description{ 1 };
                pCollider->Setup(description);

                game::ColliBoxShapeCInfo colliInfo{};
                colliInfo.m_Size = ms_JumpPanelSize;
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalPosition(ms_JumpPanelPosition);

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                pCollider->CreateShape(colliInfo);
            }
            else if (type == SJumpPanelParam::eType_Upwards)
            {
                game::GOCCollider::Description description{ 2 };
                pCollider->Setup(description);

                for (size_t i = 0; i < description.m_ShapeCount; i++)
                {
                    game::ColliBoxShapeCInfo colliInfo{};
                    colliInfo.m_Size = ms_JumpPanelUSizes[i];
                    colliInfo.m_Unk2 |= 1;

                    colliInfo.SetLocalPosition(ms_JumpPanelUPositions[i]);
                    colliInfo.SetLocalRotation(Eigen::Quaternionf(Eigen::AngleAxisf(ms_JumpPanelURotations[i] * MATHF_PI / 180, Eigen::Vector3f::UnitX())));

                    ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                    pCollider->CreateShape(colliInfo);
                }
            }
        }

        Vector3 GetDirectionVector()
        {
            auto* pTransform = GetComponent<fnd::GOCTransform>();
            auto* pParam = reinterpret_cast<SJumpPanelParam*>(m_pAdapter->GetData());

            Vector3 direction{};

            if (!pParam->m_TargetID)
            {
                auto launchAngle = pTransform->GetLocalRotation() * GetLaunchOffset(pParam->m_Type) * GetPitchCorrection(pParam->m_Pitch);
                return static_cast<Vector3>(launchAngle * Eigen::Vector3f::UnitZ());
            }
            else
            {
                Vector3 targetPosition{};
                ObjUtil::GetSetObjectTransform(*m_pOwnerDocument, pParam->m_TargetID, &targetPosition, nullptr);
                
                Vector3 direction = static_cast<Vector3>(targetPosition - pTransform->GetLocalPosition());
                direction.normalize();

                return static_cast<Vector3>(direction * pParam->m_FirstSpeed);
            }
        }

        Quaternion GetLaunchOffset(SJumpPanelParam::EType type)
        {
            return Eigen::Quaternionf(Eigen::AngleAxisf(ms_JumpPanelLaunchOffsets[type] * MATHF_PI / 180, Eigen::Vector3f::UnitX()));
        }

        Quaternion GetPitchCorrection(float pitch)
        {
            return Eigen::Quaternionf(Eigen::AngleAxisf(pitch * MATHF_PI / 180, Eigen::Vector3f::UnitY()));
        }
    };
}