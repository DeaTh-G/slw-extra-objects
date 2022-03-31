#pragma once

namespace app
{
    class ObjUpReel : public CSetObjectListener, public TTinyFsm<ObjUpReel>
    {
    protected:
        float m_Length{};
        float m_Time{};
        float m_WaitTime{};
        bool m_Move{};
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

            ChangeState(&ObjUpReel::StateIdle);
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
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateUpdate(rInfo.deltaTime));
        }

    private:
        void ChangeState(const TTinyFsm::TiFsmState_t& rState)
        {
            FSM_TRAN(rState);
        }

        TTinyFsm::TiFsmState_t StateIdle(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            switch (rEvent.getSignal())
            {
            case TiFSM_SIGNAL_ENTER:
            {
                SetHandleDistance(-m_Length - 4.0f);

                return nullptr;
            }
            case TiFSM_SIGNAL_MESSAGE:
            {
                switch (rEvent.getMessage().GetType())
                {
                case xgame::MsgHitEventCollision::MessageID:
                {
                    auto msg = reinterpret_cast<xgame::MsgHitEventCollision&>(rEvent.getMessage());

                    m_PlayerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
                    if (m_PlayerNo < 0)
                        return nullptr;

                    m_Move = true;

                    xgame::MsgCatchPlayer catchMessage{};
                    catchMessage.m_Unk3 = 13;
                    ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, catchMessage);
                    return nullptr;
                }
                case xgame::MsgGetExternalMovePosition::MessageID:
                {
                    MovePlayerWithHandle(reinterpret_cast<xgame::MsgGetExternalMovePosition&>(rEvent.getMessage()).m_pTransform);
                    return nullptr;
                }
                case xgame::MsgPLGetHomingTargetInfo::MessageID:
                {
                    auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
                    if (!pHandleModel)
                        return nullptr;

                    Matrix34 handleTransform{};
                    pHandleModel->GetMatrix(&handleTransform);

                    reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(rEvent.getMessage()).m_CursorPosition = handleTransform.GetTransVector();
                    return nullptr;
                }
                }
            }
            case TiFSM_SIGNAL_UPDATE:
            {
                if (!m_Move)
                    return nullptr;

                auto pHandleVisual = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);

                m_Time += rEvent.getFloat();
                float position = m_Length - (m_Time * 550.0f);
                if (position < 11.9f)
                {
                    position = 11.9f;
                    ChangeState(&ObjUpReel::StateUp);
                }

                SetHandleDistance(-position);
                return nullptr;
            }
            }

            return nullptr;
        }

        TTinyFsm::TiFsmState_t StateWaitUp(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            SetHandleDistance(-11.9f);

            return nullptr;
        }

        TTinyFsm::TiFsmState_t StateUp(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            switch (rEvent.getSignal())
            {
            case TiFSM_SIGNAL_ENTER:
            {
                m_Move = false;
                return nullptr;
            }
            case TiFSM_SIGNAL_MESSAGE:
            {
                switch (rEvent.getMessage().GetType())
                {
                case xgame::MsgGetExternalMovePosition::MessageID:
                {
                    MovePlayerWithHandle(reinterpret_cast<xgame::MsgGetExternalMovePosition&>(rEvent.getMessage()).m_pTransform);
                    return nullptr;
                }
                }
            }
            case TiFSM_SIGNAL_UPDATE:
            {
                xgame::MsgGetPosition playerPosMsg{};
                ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, playerPosMsg);

                if (!m_Move)
                {
                    xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), static_cast<Vector3>(Vector3::UnitY() * 150.0f), 0.25f, 0.0f };
                    ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, impulseMsg);

                    m_Move = true;
                    return nullptr;
                }

                auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*m_pOwnerDocument, m_PlayerNo)));
                float velocity = pPlayer->m_pPhysics->GetVertVelocity().y();
                if (velocity > 20.0f)
                    return nullptr;

                xgame::MsgSpringImpulse impulse2ndMsg{ playerPosMsg.GetPosition(), static_cast<Vector3>(Vector3(0.0f, 0.75f, -0.25f) * 150.0f), 0.25f, 0.0f };
                impulse2ndMsg.field_50.set(12);
                ObjUtil::SendMessageImmToPlayer(*this, m_PlayerNo, impulse2ndMsg);

                ChangeState(&ObjUpReel::StateDown);

                return nullptr;
            }
            }

            return nullptr;
        }

        TTinyFsm::TiFsmState_t StateDown(const TiFsmBasicEvent<ObjUpReel>& rEvent)
        {
            switch (rEvent.getSignal())
            {
            case TiFSM_SIGNAL_ENTER:
            {
                m_Time = 0.0f;
                return nullptr;
            }
            case TiFSM_SIGNAL_UPDATE:
            {
                if (m_WaitTime > 2.0f)
                {
                    m_WaitTime + rEvent.getFloat();
                    return nullptr;
                }

                m_Time += rEvent.getFloat();

                Matrix34 handleMatrix{};
                reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->GetMatrix(&handleMatrix);
                float position = handleMatrix.GetTransVector().y() - (m_Time * 200.0f);
                printf("%f\n", GetComponent<fnd::GOCTransform>()->GetLocalPosition().y() - m_Length - 4.0f);
                if (position < GetComponent<fnd::GOCTransform>()->GetLocalPosition().y() -m_Length - 4.0f)
                {
                    position = -m_Length - 4.0f;
                    ChangeState(&ObjUpReel::StateIdle);
                }

                SetHandleDistance(position);

                return nullptr;
            }
            case TiFSM_SIGNAL_LEAVE:
            {
                m_Move = false;
                m_Time = false;
                m_WaitTime = false;
                return nullptr;
            }
            }

            return nullptr;
        }

        bool ProcMsgGetExternalMovePosition(xgame::MsgGetExternalMovePosition& msg)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(msg));
            return true;
        }

        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(msg));
            return true;
        }

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& msg)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(msg));
            return true;
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