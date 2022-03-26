#pragma once

namespace app
{
    class ObjUpReel : public CSetObjectListener
    {
    protected:
        float m_Length{};
        float m_Time{};
        bool m_Move{};
        bool m_LaunchUp{};
        int m_PlayerNo{};

    public:
        ObjUpReel() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualContainer>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjUpReelInfo>(document, "ObjUpReelInfo");
            auto* pParam = reinterpret_cast<SUpReelParam*>(m_pAdapter->GetData());

            m_Length = pParam->m_Length;

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisualContainer = GetComponent<fnd::GOCVisualContainer>();
            if (pVisualContainer)
            {
                pVisualContainer->Setup({ ObjUpReelInfo::ms_ModelCount });

                for (size_t i = 0; i < ObjUpReelInfo::ms_ModelCount; i++)
                {
                    auto* pVisual = pVisualContainer->CreateSingle<fnd::GOCVisualModel>(*this);
                    if (!pVisual)
                        continue;

                    fnd::GOCVisualModel::Description description{};
                    description.m_Model = pInfo->m_Models[i];
                    if (i == 2)
                        description.m_Skeleton = pInfo->m_WireSkeleton;

                    pVisual->Setup(description);

                    pVisualContainer->Add(pVisual);
                }
                
                SetWireLength(-m_Length);
                SetHandleDistance();
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliSphereShapeCInfo colliInfo{};
                colliInfo.m_Radius = 6.0f;
                colliInfo.m_Unk2 |= 1;
                colliInfo.m_Unk3 = 0x20000;

                colliInfo.SetLocalPosition({ 0.0f, -pParam->m_Length - 4.0f, 0.0f });

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, colliInfo);
                pCollider->CreateShape(colliInfo);
            }

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
            case xgame::MsgGetExternalMovePosition::MessageID:
                return ProcMsgGetExternalMovePosition(reinterpret_cast<xgame::MsgGetExternalMovePosition&>(msg));
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
        }

        void Update(const fnd::SUpdateInfo& rInfo) override
        {
            if (!m_Move)
                return;
         
            auto pHandleVisual = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);

            m_Time += rInfo.deltaTime;
            float position = m_Length - (m_Time * 550.0f);
            if (position < 11.9f)
            {
                position = 11.9f;

                xgame::MsgGetPosition playerPosMsg{};
                ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, playerPosMsg);

                if (!m_LaunchUp)
                {
                    xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), static_cast<Vector3>(Vector3::UnitY() * 150.0f), 0.25f, 0.0f };
                    ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, impulseMsg);

                    m_LaunchUp = true;
                }
                
                auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*m_pOwnerDocument, m_PlayerNo)));
                float velocity = pPlayer->m_pPhysics->GetVertVelocity().y();

                if (velocity > 20.0f)
                    return;

                m_Move = false;

                xgame::MsgSpringImpulse impulse2ndMsg{ playerPosMsg.GetPosition(), static_cast<Vector3>(Vector3(0.0f, 0.5f, -0.5f) * 150.0f), 0.25f, 0.0f };
                impulse2ndMsg.field_50.set(12);
                ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, impulse2ndMsg);
            }

            SetWireLength(-position);
            pHandleVisual->SetLocalTranslation({ 0, -position, 0 });
            GetComponent<game::GOCCollider>()->GetShape()->SetLocalTranslation({ 0, -position, 0 });
        }

    private:
        bool ProcMsgGetExternalMovePosition(xgame::MsgGetExternalMovePosition& msg)
        {
            auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                return false;

            pHandleModel->GetMatrix(msg.m_pTransform);

            Quaternion rotation(*msg.m_pTransform);
            rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), Vector3::UnitY());
            
            for (size_t i = 0; i < 3; i++)
                msg.m_pTransform->SetColumn(i, static_cast<Vector3>(rotation.toRotationMatrix().row(i)));

            msg.m_pTransform->GetTransVector().y() += -10.5f;

            return true;
        }

        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            m_PlayerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (m_PlayerNo < 0)
                return false;

            m_Move = true;

            xgame::MsgCatchPlayer catchMessage{};
            catchMessage.m_Unk3 = 13;
            ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, catchMessage);

            return true;
        }

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& msg)
        {
            auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                return false;
            
            Matrix34 handleTransform{};
            pHandleModel->GetMatrix(&handleTransform);
            
            msg.m_CursorPosition = handleTransform.GetTransVector();

            return true;
        }

        void SetWireLength(float length)
        {
            auto* pWireModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[2]);
            if (!pWireModel)
                return;

            Transform transform{};

            pWireModel->GetNodeTransform(2, "Cos_top", &transform);
            transform.m_Position.y() = length + 6.0f;
            pWireModel->SetNodeTransform(2, "Cos_top", transform);
            
            auto bounds = pWireModel->GetBounds();
            bounds.m_Max.y() = length + 6.0f;
            pWireModel->SetBounds(bounds);
        }

        void SetHandleDistance()
        {
            reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->SetLocalTranslation({ 0, -m_Length - 4.0f, 0 });
        }
    };
}