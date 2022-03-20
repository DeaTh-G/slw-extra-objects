#pragma once

namespace app
{
    class ObjWideSpring : public CSetObjectListener
    {
    protected:
        float m_FirstSpeed{};
        float m_KeepVelocityDistance{};
        float m_OutOfControl{};

    public:
        ObjWideSpring()
        {
            SetProperty(0x4002, { 9 });
        }

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjWideSpringInfo>(document, "ObjWideSpringInfo");
            auto* pParam = reinterpret_cast<SWideSpringParam*>(m_pAdapter->GetData());

            m_FirstSpeed = pParam->m_FirstSpeed;
            m_KeepVelocityDistance = pParam->m_KeepVelocityDistance;
            m_OutOfControl = pParam->m_OutOfControl;

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Model;

                pVisual->Setup(description);
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliCapsuleShapeCInfo colliInfo{};
                colliInfo.m_Radius = 6.0f;
                colliInfo.m_Height = 12.0f;
                colliInfo.m_Unk2 |= 1;
                colliInfo.m_Unk3 = 0x20000;

                colliInfo.SetLocalPosition({ 0.0f, 3.5f, 0.0f });
                colliInfo.SetLocalRotation(Eigen::Quaternionf(Eigen::AngleAxisf(90 * MATHF_PI / 180, Vector3::UnitZ())));

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, colliInfo);
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
            case xgame::MsgPLGetHomingTargetInfo::MessageID:
                return ProcMsgPLGetHomingTargetInfo(reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(msg));
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

            GetComponent<game::GOCSound>()->Play3D("obj_spring", {}, 0);

            xgame::MsgGetPosition playerPosMsg{};
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

            xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(), m_OutOfControl, m_KeepVelocityDistance / m_FirstSpeed };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& msg)
        {
            auto cursorPos = GetComponent<fnd::GOCTransform>()->m_Frame.m_Unk1.m_Mtx * Eigen::Vector4f(0, 3.5f, 0, 1);
            msg.m_CursorPosition = Vector3(cursorPos.x(), cursorPos.y(), cursorPos.z());

            return true;
        }

        Vector3 GetDirectionVector()
        {
            auto* pParam = reinterpret_cast<SWideSpringParam*>(m_pAdapter->GetData());
            
            return static_cast<Vector3>(Vector3::UnitY() * m_FirstSpeed);
        }
    };
}