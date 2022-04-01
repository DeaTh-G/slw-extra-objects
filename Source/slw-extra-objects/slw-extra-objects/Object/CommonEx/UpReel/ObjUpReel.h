#pragma once

namespace app
{
    class ObjUpReel : public CSetObjectListener, public TTinyFsm<ObjUpReel>
    {
    protected:
        float m_Length{};
        float m_ImpulseVelocity{};
        float m_OutOfControl{};
        float m_UpSpeedMax{};
        bool m_IsOneTimeUp{};
        int m_PlayerNo{};
        bool m_IsPlayerMoving{};

    public:
        ObjUpReel() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualContainer>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjUpReelInfo>(document, "ObjUpReelInfo");
            auto* pParam = reinterpret_cast<SUpReelParam*>(m_pAdapter->GetData());

            m_Length = pParam->m_Length + 4.0f;
            m_ImpulseVelocity = pParam->m_ImpulseVelocity;
            m_OutOfControl = pParam->m_OutOfControl;
            m_UpSpeedMax = pParam->m_UpSpeedMax;
            m_IsOneTimeUp = pParam->m_IsOneTimeUp;

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
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliSphereShapeCInfo colliInfo{};
                colliInfo.m_Radius = 6.0f;
                colliInfo.m_Unk2 |= 1;
                colliInfo.m_Unk3 = 0x20000;

                colliInfo.SetLocalPosition({ 0.0f, -m_Length, 0.0f });

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, colliInfo);
                pCollider->CreateShape(colliInfo);
            }

            fnd::GOComponent::EndSetup(*this);

            if (pParam->m_IsWaitUp && !GetExtUserData(eExtUserDataType_High))
            {
                SetHandleDistance(-11.9f);
                ChangeState(&ObjUpReel::StateWaitUp);

                return;
            }

            SetHandleDistance(-m_Length);
            ChangeState(&ObjUpReel::StateIdle);
        }

        bool ProcessMessage(fnd::Message& msg) override
        {
            if (PreProcessMessage(msg))
                return true;

            switch (msg.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:
            case xgame::MsgPLGetHomingTargetInfo::MessageID:
            case xgame::MsgGetExternalMovePosition::MessageID:
            case xgame::MsgNotifyObjectEvent::MessageID:
                TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(msg));
                return true;
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
        }

        void Update(const fnd::SUpdateInfo& rInfo) override
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateUpdate(rInfo.deltaTime));
        }

    private:
        void ChangeState(const TTinyFsm::TiFsmState_t& rState)
        {
            FSM_TRAN(rState);
        }

        TTinyFsm::TiFsmState_t StateIdle(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            if (rEvent.getSignal() != TiFSM_SIGNAL_MESSAGE)
                return FSM_TOP();

            switch (rEvent.getMessage().GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:
            {
                ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(rEvent.getMessage()));
                break;
            }
            case xgame::MsgPLGetHomingTargetInfo::MessageID:
            {
                ProcMsgPLGetHomingTargetInfo(reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(rEvent.getMessage()));
                break;
            }
            }

            return FSM_TOP();
        }

        TTinyFsm::TiFsmState_t StateWaitUp(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            if (rEvent.getSignal() != TiFSM_SIGNAL_MESSAGE)
                return FSM_TOP();

            switch (rEvent.getMessage().GetType())
            {
            case xgame::MsgNotifyObjectEvent::MessageID:
            {
                ProcMsgNotifyObjectEvent(reinterpret_cast<xgame::MsgNotifyObjectEvent&>(rEvent.getMessage()));
                break;
            }
            }

            return FSM_TOP();
        }

        TTinyFsm::TiFsmState_t StateUp(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            switch (rEvent.getSignal())
            {
            case TiFSM_SIGNAL_UPDATE:
            {
                xgame::MsgGetPosition playerPosMsg{};
                ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, playerPosMsg);

                if (m_IsPlayerMoving)
                {
                    /*auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*m_pOwnerDocument, m_PlayerNo)));
                    float velocity = pPlayer->m_pPhysics->GetVertVelocity().y();

                    if (velocity > 20.0f)
                        return FSM_TOP();
                    
                    xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetForwardDirectionVector(), m_OutOfControl, 0.0f };
                    impulseMsg.field_50.set(12);
                    ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, impulseMsg);*/

                    ChangeState(&ObjUpReel::StateDown);

                    return nullptr;
                }

                auto handlePos = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->m_Transform.m_Mtx.GetTransVector();
                float position = abs(handlePos.y()) - rEvent.getFloat() * 550.0f;
                if (position < 11.9f && !m_IsPlayerMoving)
                {
                    position = 11.9f;                

                    xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), static_cast<Vector3>(Vector3::UnitY() * m_ImpulseVelocity), m_OutOfControl, 0.0f };
                    ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, impulseMsg);

                    m_IsPlayerMoving = true;
                }

                SetHandleDistance(-position);
                break;
            }
            case TiFSM_SIGNAL_MESSAGE:
            {
                switch (rEvent.getMessage().GetType())
                {
                case xgame::MsgGetExternalMovePosition::MessageID:
                {
                    ProcMsgGetExternalMovePosition(reinterpret_cast<xgame::MsgGetExternalMovePosition&>(rEvent.getMessage()));
                    break;
                }
                }
                break;
            }
            }

            return FSM_TOP();
        }

        TTinyFsm::TiFsmState_t StateDown(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            switch (rEvent.getSignal())
            {
            case TiFSM_SIGNAL_ENTER:
            {
                m_IsPlayerMoving = false;
            }
            case TiFSM_SIGNAL_UPDATE:
            {
                auto handlePos = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->m_Transform.m_Mtx.GetTransVector();

                float position = abs(handlePos.y()) + rEvent.getFloat() * 100.0f;
                if (position > m_Length)
                {
                    position = m_Length;
                    ChangeState(&ObjUpReel::StateIdle);
                }

                SetHandleDistance(-position);
                break;
            }
            case TiFSM_SIGNAL_MESSAGE:
            {
                switch (rEvent.getMessage().GetType())
                {
                case xgame::MsgHitEventCollision::MessageID:
                {
                    ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(rEvent.getMessage()));
                    break;
                }
                case xgame::MsgPLGetHomingTargetInfo::MessageID:
                {
                    ProcMsgPLGetHomingTargetInfo(reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(rEvent.getMessage()));
                    break;
                }
                }
                break;
            }
            }

            return FSM_TOP();
        }

        void ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            m_PlayerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (m_PlayerNo < 0)
                return;

            ChangeState(&ObjUpReel::StateUp);

            xgame::MsgCatchPlayer catchMessage{};
            catchMessage.m_Unk3 = 13;
            ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, catchMessage);
        }

        void ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& msg)
        {
            auto* pHandle = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandle)
                return;

            Matrix34 handleTransform{};
            pHandle->GetMatrix(&handleTransform);

            msg.m_CursorPosition = handleTransform.GetTransVector();
        }

        void ProcMsgGetExternalMovePosition(xgame::MsgGetExternalMovePosition& msg)
        {
            auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                return;

            pHandleModel->GetMatrix(msg.m_pTransform);

            Quaternion rotation(*msg.m_pTransform);
            rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), Vector3::UnitY());

            for (size_t i = 0; i < 3; i++)
                msg.m_pTransform->SetColumn(i, static_cast<Vector3>(rotation.toRotationMatrix().row(i)));

            msg.m_pTransform->GetTransVector().y() += -10.5f;
        }

        void ProcMsgNotifyObjectEvent(xgame::MsgNotifyObjectEvent& msg)
        {
            if (!msg.GetEventType())
                return;
            
            SetExtUserData(eExtUserDataType_High, 1);
            ChangeState(&ObjUpReel::StateDown);
        }

        Vector3 GetForwardDirectionVector()
        {
            return static_cast<Vector3>(GetComponent<fnd::GOCTransform>()->GetLocalRotation() * Vector3(0.0f, 0.65f, -0.35f) * m_ImpulseVelocity);
        }

        void MovePlayerWithHandle(Matrix34* transform)
        {
            auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                return;

            pHandleModel->GetMatrix(transform);

            Quaternion rotation(*transform);
            rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), Vector3::UnitY());

            for (size_t i = 0; i < 3; i++)
                transform->SetColumn(i, static_cast<Vector3>(rotation.toRotationMatrix().row(i)));

            transform->GetTransVector().y() += -10.5f;
        }

        void SetWireLength(float length)
        {
            auto* pWireModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[2]);
            if (!pWireModel)
                return;

            Transform transform{};
            pWireModel->GetNodeTransform(2, "Cos_top", &transform);
            
            transform.m_Position.y() = length;
            pWireModel->SetNodeTransform(2, "Cos_top", transform);
            
            auto bounds = pWireModel->GetBounds();
            bounds.m_Max.y() = length;
            pWireModel->SetBounds(bounds);
        }

        void SetHandleDistance(float distance)
        {
            reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->SetLocalTranslation({ 0, distance, 0 });
            SetWireLength(distance + 11.45f);
            GetComponent<game::GOCCollider>()->GetShape()->SetLocalTranslation({ 0, distance, 0 });
        }
    };
}