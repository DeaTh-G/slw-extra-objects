#pragma once

namespace app
{
    class ObjUpReel : public CSetObjectListener, public TTinyFsm<ObjUpReel>
    {
    protected:
        inline static const size_t ms_ModelCount = 3;
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 6.0f;

    public:
        float Length{};
        float ImpulseVelocity{};
        float OutOfControl{};
        float UpSpeedMax{};
        bool IsOneTimeUp{};
        int PlayerNo{};
        float PositionOffset{};
        float Time{};

    public:
        ObjUpReel() : CSetObjectListener(), TTinyFsm<ObjUpReel>(&ObjUpReel::StateIdle)
        {

        }

        void AddCallback(GameDocument& in_rDocument) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualContainer>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjUpReelInfo>(in_rDocument);
            auto* pParam = GetAdapter()->GetData<SUpReelParam>();

            Length = pParam->Length + 4.0f;
            ImpulseVelocity = pParam->ImpulseVelocity;
            OutOfControl = pParam->OutOfControl;
            UpSpeedMax = pParam->UpSpeedMax;
            IsOneTimeUp = pParam->IsOneTimeUp;

            fnd::GOComponent::BeginSetup(*this);

            if (auto* pVisualContainerGoc = GetComponent<fnd::GOCVisualContainer>())
            {
                pVisualContainerGoc->Setup({ ms_ModelCount });

                for (size_t i = 0; i < ms_ModelCount; i++)
                {
                    if (auto* pVisualModelGoc = pVisualContainerGoc->CreateSingle<fnd::GOCVisualModel>(*this))
                    {
                        fnd::GOCVisualModel::Description description{};
                        description.m_Model = pInfo->Models[i];
                        if (i == 2)
                            description.m_Skeleton = pInfo->WireSkeleton;

                        pVisualModelGoc->Setup(description);

                        pVisualContainerGoc->Add(pVisualModelGoc);
                    }
                }
            }

            if (auto* pColliderGoc = GetComponent<game::GOCCollider>())
            {
                pColliderGoc->Setup({ ms_ShapeCount });

                game::ColliSphereShapeCInfo collisionInfo{};
                collisionInfo.m_Radius = ms_CollisionRadius;
                collisionInfo.m_Unk2 |= 1;
                collisionInfo.m_Unk3 = 0x20000;

                collisionInfo.SetLocalPosition({ 0.0f, -Length, 0.0f });

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, collisionInfo);
                pColliderGoc->CreateShape(collisionInfo);
            }

            fnd::GOComponent::EndSetup(*this);

            if (pParam->IsWaitUp && !GetExtUserData(eExtUserDataType_High))
            {
                SetHandleDistance(-11.9f);
                ChangeState(&ObjUpReel::StateWaitUp);

                return;
            }

            SetHandleDistance(-Length);

            InitFSM();
        }

        bool ProcessMessage(fnd::Message& in_rMessage) override
        {
            if (PreProcessMessage(in_rMessage))
                return true;

            switch (in_rMessage.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:            return ProcMsgHitEventCollision(static_cast<xgame::MsgHitEventCollision&>(in_rMessage));
            case xgame::MsgPLGetHomingTargetInfo::MessageID:        return ProcMsgPLGetHomingTargetInfo(static_cast<xgame::MsgPLGetHomingTargetInfo&>(in_rMessage));
            case xgame::MsgGetExternalMovePosition::MessageID:      return ProcMsgGetExternalMovePosition(static_cast<xgame::MsgGetExternalMovePosition&>(in_rMessage));
            case xgame::MsgNotifyObjectEvent::MessageID:            return ProcMsgNotifyObjectEvent(static_cast<xgame::MsgNotifyObjectEvent&>(in_rMessage));
            default:                                                return CSetObjectListener::ProcessMessage(in_rMessage);
            }
        }

        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& in_rMessage)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));
        }

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& in_rMessage)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));
        }

        bool ProcMsgGetExternalMovePosition(xgame::MsgGetExternalMovePosition& in_rMessage)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));
        }

        bool ProcMsgNotifyObjectEvent(xgame::MsgNotifyObjectEvent& in_rMessage)
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));
        }

        void Update(const fnd::SUpdateInfo& in_rUpdateInfo) override
        {
            TTinyFsm::DispatchFSM(TiFsmBasicEvent<ObjUpReel>::CreateUpdate(in_rUpdateInfo.deltaTime));
        }

    private:
        void ChangeState(TiFsmState_t in_state)
        {
            FSM_TRAN(in_state);
        }

        TTinyFsm::TiFsmState_t StateIdle(const TiFsmEvent_t& in_rEvent)
        {
            if (in_rEvent.getSignal() != TiFSM_SIGNAL_MESSAGE)
                return FSM_TOP();

            switch (in_rEvent.getMessage().GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:
            {
                auto& message = static_cast<xgame::MsgHitEventCollision&>(in_rEvent.getMessage());

                PlayerNo = ObjUtil::GetPlayerNo(*GetDocument(), message.m_Sender);
                if (PlayerNo < 0)
                    return;

                ChangeState(&ObjUpReel::StateUp);

                xgame::MsgCatchPlayer catchMessage{};
                catchMessage.m_Unk3 = 21;
                ObjUtil::SendMessageImmToPlayer(*this, PlayerNo, catchMessage);
                break;
            }
            case xgame::MsgPLGetHomingTargetInfo::MessageID:
            {
                auto& message = static_cast<xgame::MsgPLGetHomingTargetInfo&>(in_rEvent.getMessage());

                auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
                if (!pHandleModel)
                    return;

                csl::math::Matrix34 handleTransform{};
                pHandleModel->GetMatrix(&handleTransform);

                message.m_CursorPosition = handleTransform.GetTransVector();
                break;
            }
            }

            return FSM_TOP();
        }

        TTinyFsm::TiFsmState_t StateWaitUp(const TiFsmBasicEvent<ObjUpReel>& in_rEvent)
        {
            if (in_rEvent.getSignal() != TiFSM_SIGNAL_MESSAGE)
                return FSM_TOP();

            switch (in_rEvent.getMessage().GetType())
            {
            case xgame::MsgNotifyObjectEvent::MessageID:
            {
                auto& message = static_cast<xgame::MsgNotifyObjectEvent&>(in_rEvent.getMessage());

                if (!message.GetEventType())
                    return;

                SetExtUserData(eExtUserDataType_High, 1);
                ChangeState(&ObjUpReel::StateDown);
                break;
            }
            }

            return FSM_TOP();
        }

        TTinyFsm::TiFsmState_t StateUp(const TiFsmBasicEvent<ObjUpReel>& in_rEvent)
        {
            switch (in_rEvent.getSignal())
            {
            case TiFSM_SIGNAL_ENTER:
            {
                PositionOffset = 0.0f;
                break;
            }
            case TiFSM_SIGNAL_UPDATE:
            {
                auto handlePos = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->m_Transform.m_Mtx.GetTransVector();

                PositionOffset += in_rEvent.getFloat() * UpSpeedMax * 0.2f;
                if (abs(handlePos.y()) - PositionOffset >= 11.9f)
                {
                    SetHandleDistance(-(abs(handlePos.y()) - PositionOffset));
                    return FSM_TOP();
                }

                SetHandleDistance(-11.9f);

                auto position = GetComponent<fnd::GOCTransform>()->GetLocalPosition();
                position.y() += 2.9f;

                auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*GetDocument(), PlayerNo)));
                pPlayer->GetPlayerGOC<app::Player::CVisualGOC>()->GetHumanAnimationPtr()->ExitLoop();

                /*auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*m_pOwnerDocument, 0)));
                pPlayer->GetStateGOC()->ChangeAnimation("UPREEL_END");*/

                /*xgame::MsgSpringImpulse msg{position, GetForwardDirectionVector(1.0f, -0.4f), OutOfControl, 0.0f};
                msg.Flags.set(12);
                ObjUtil::SendMessageImmToPlayer(*this, PlayerNo, msg);

                ChangeState(&ObjUpReel::StateDown);*/

                break;
            }
            case TiFSM_SIGNAL_MESSAGE:
            {
                switch (in_rEvent.getMessage().GetType())
                {
                case xgame::MsgGetExternalMovePosition::MessageID:
                {
                    auto& message = static_cast<xgame::MsgGetExternalMovePosition&>(in_rEvent.getMessage());

                    auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
                    if (!pHandleModel)
                        return;

                    pHandleModel->GetMatrix(message.pTrsMatrix);

                    csl::math::Quaternion rotation{ *message.pTrsMatrix };
                    rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), csl::math::Vector3::UnitY());

                    for (size_t i = 0; i < 3; i++)
                        message.pTrsMatrix->SetColumn(i, static_cast<csl::math::Vector3>(rotation.toRotationMatrix().col(i)));

                    //msg.m_pTransform->GetTransVector().y() += -10.5f;

                    break;
                }
                }
                break;
            }
            }

            return FSM_TOP();
        }

        TTinyFsm::TiFsmState_t StateDown(const TiFsmBasicEvent<ObjUpReel>& in_rEvent)
        {
            switch (in_rEvent.getSignal())
            {
            case TiFSM_SIGNAL_ENTER:
            {
                PositionOffset = 0.0f;
                Time = 0.0f;
                break;
            }
            case TiFSM_SIGNAL_UPDATE:
            {
                if (IsOneTimeUp)
                    return FSM_TOP();

                Time += in_rEvent.getFloat();
                if (Time < 0.45f)
                    return FSM_TOP();

                auto handlePos = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->m_Transform.m_Mtx.GetTransVector();

                PositionOffset += in_rEvent.getFloat() * 5.0f;

                float position = abs(handlePos.y()) + PositionOffset;
                if (position > Length)
                {
                    position = Length;
                    ChangeState(&ObjUpReel::StateIdle);
                }

                SetHandleDistance(-position);
                break;
            }
            case TiFSM_SIGNAL_MESSAGE:
            {
                switch (in_rEvent.getMessage().GetType())
                {
                case xgame::MsgHitEventCollision::MessageID:
                {
                    auto& message = static_cast<xgame::MsgHitEventCollision&>(in_rEvent.getMessage());

                    PlayerNo = ObjUtil::GetPlayerNo(*GetDocument(), message.m_Sender);
                    if (PlayerNo < 0)
                        return;

                    ChangeState(&ObjUpReel::StateUp);

                    xgame::MsgCatchPlayer catchMessage{};
                    catchMessage.m_Unk3 = 21;
                    ObjUtil::SendMessageImmToPlayer(*this, PlayerNo, catchMessage);
                    break;
                }
                case xgame::MsgPLGetHomingTargetInfo::MessageID:
                {
                    auto& message = static_cast<xgame::MsgPLGetHomingTargetInfo&>(in_rEvent.getMessage());

                    auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
                    if (!pHandleModel)
                        return;

                    csl::math::Matrix34 handleTransform{};
                    pHandleModel->GetMatrix(&handleTransform);

                    message.m_CursorPosition = handleTransform.GetTransVector();
                    break;
                }
                }
                break;
            }
            }

            return FSM_TOP();
        }

        csl::math::Vector3 GetForwardDirectionVector(float in_up, float in_forward)
        {
            return static_cast<csl::math::Vector3>(GetComponent<fnd::GOCTransform>()->GetLocalRotation() * csl::math::Vector3(0.0f, in_up, in_forward) * ImpulseVelocity);
        }

        void MovePlayerWithHandle(csl::math::Matrix34* in_pTransform)
        {
            auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                return;

            pHandleModel->GetMatrix(in_pTransform);

            csl::math::Quaternion rotation{ *in_pTransform };
            rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), csl::math::Vector3::UnitY());

            for (size_t i = 0; i < 3; i++)
                in_pTransform->SetColumn(i, static_cast<csl::math::Vector3>(rotation.toRotationMatrix().row(i)));

            in_pTransform->GetTransVector().y() += -10.5f;
        }

        void SetWireLength(float in_Length)
        {
            auto* pWireModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[2]);
            if (!pWireModel)
                return;

            math::Transform transform{};
            pWireModel->GetNodeTransform(2, "Cos_top", &transform);
            
            transform.m_Position.y() = in_Length;
            pWireModel->SetNodeTransform(2, "Cos_top", transform);
            
            auto bounds = pWireModel->GetBounds();
            bounds.m_Max.y() = in_Length;
            pWireModel->SetBounds(bounds);
        }

        void SetHandleDistance(float in_Distance)
        {
            reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->SetLocalTranslation({ 0.0f, in_Distance, 0.0f });
            SetWireLength(in_Distance + 11.45f);
            GetComponent<game::GOCCollider>()->GetShape()->SetLocalTranslation({ 0.0f, in_Distance, 0.0f });
        }
    };
}