#pragma once
#include "Patches\Functions.h"

using namespace csl::math;

namespace app
{
    class ObjJumpPanel : public CSetObjectListener
    {
    private:
        inline static Vector3 ms_Sizes[] = { { 8.6f, 0.85f, 19.6f }, { 8.6f, 0.85f, 5.5f }, { 8.6f, 0.85f, 15.2f } };
        inline static Vector3 ms_Positions[] = { { 0.0f, 0.0f, 19.0f }, { 0.0f, 1.4f, 4.8f }, { 0.0f, 9.2f, 23.755f } };
        inline static float ms_UpRotations[] = { -15.0f, -25.0f };
        inline static float ms_LaunchOffsets[] = { -5.0f, -30.0f };

    protected:
        float m_FirstSpeed{};
        float m_KeepVelocityDistance{};
        float m_OutOfControl{};
        float m_Pitch{};
        CSetObjectID m_TargetID{};
        SJumpPanelParam::EType m_Type{};
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

            m_FirstSpeed = pParam->m_FirstSpeed;
            m_KeepVelocityDistance = pParam->m_KeepVelocityDistance;
            m_OutOfControl = pParam->m_OutOfControl;
            m_Pitch = pParam->m_Pitch;
            m_TargetID = pParam->m_TargetID;
            m_Type = pParam->m_Type;

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[m_Type];

                pVisual->Setup(description);

                auto* pBlender = pVisual->SetTexSrtBlender({ ObjJumpPanelInfo::ms_AnimCount });

                for (size_t i = 0; i < ObjJumpPanelInfo::ms_AnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TexAnims[i], 1 });
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ m_Type + 1 });
                for (size_t i = 0; i < pCollider->m_Shapes.capacity(); i++)
                {
                    game::ColliBoxShapeCInfo colliInfo{};
                    colliInfo.m_Size = ms_Sizes[m_Type + i];
                    colliInfo.m_Unk2 |= 1;

                    colliInfo.SetLocalPosition(ms_Positions[m_Type + i]);

                    if (m_Type == SJumpPanelParam::eType_Upwards)
                        colliInfo.SetLocalRotation(Eigen::AngleAxisf(TO_RAD(ms_UpRotations[i]), Vector3::UnitX()));

                    ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                    pCollider->CreateShape(colliInfo);
                }
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
            if (m_IsOn)
                return;

            m_Time += rInfo.deltaTime;
            if (m_Time >= 0.7f)
            {
                GetComponent<game::GOCCollider>()->SetEnable(true);
                m_Time = 0.0f;
                m_IsOn = true;
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

            GetComponent<game::GOCSound>()->Play3D("obj_dashpanel", {}, 0);
            GetComponent<game::GOCCollider>()->SetEnable(false);

            m_IsOn = false;

            xgame::MsgGetPosition playerPosMsg{};
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

            xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(), m_OutOfControl, m_KeepVelocityDistance / m_FirstSpeed };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        Vector3 GetDirectionVector()
        {
            auto* pTransform = GetComponent<fnd::GOCTransform>();
            Vector3 direction{};

            if (!m_TargetID)
            {
                auto launchAngle = pTransform->GetLocalRotation() * GetLaunchOffset() * GetPitchCorrection();
                return static_cast<Vector3>(launchAngle * Vector3::UnitZ() * m_FirstSpeed);
            }
            else
            {
                Vector3 targetPosition{};
                ObjUtil::GetSetObjectTransform(*m_pOwnerDocument, m_TargetID, &targetPosition, nullptr);
                
                Vector3 direction = static_cast<Vector3>(targetPosition - pTransform->GetLocalPosition());
                direction.normalize();

                return static_cast<Vector3>(direction * m_FirstSpeed);
            }
        }

        Quaternion GetLaunchOffset()
        {
            return Eigen::AngleAxisf(TO_RAD(ms_LaunchOffsets[m_Type]), Vector3::UnitX());
        }

        Quaternion GetPitchCorrection()
        {
            return Eigen::AngleAxisf(TO_RAD(m_Pitch), Vector3::UnitY());
        }
    };
}