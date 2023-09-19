#include "pch.h"
#include "ObjUpReel.h"

slw_extra_objects::ObjUpReel::ObjUpReel() : app::CSetObjectListener(), app::TTinyFsm<ObjUpReel>(&ObjUpReel::StateIdle)
{

}

void slw_extra_objects::ObjUpReel::AddCallback(app::GameDocument& in_rDocument)
{
    app::fnd::GOComponent::Create<app::fnd::GOCVisualContainer>(*this);
    app::fnd::GOComponent::Create<app::game::GOCCollider>(*this);

    auto* pInfo = app::ObjUtil::GetObjectInfo<ObjUpReelInfo>(in_rDocument);
    auto* pParam = GetAdapter()->GetData<SUpReelParam>();

    Length = pParam->Length + 4.0f;
    ImpulseVelocity = pParam->ImpulseVelocity;
    OutOfControl = pParam->OutOfControl;
    UpSpeedMax = pParam->UpSpeedMax;
    IsOneTimeUp = pParam->IsOneTimeUp;

    app::fnd::GOComponent::BeginSetup(*this);

    if (auto* pVisualContainerGoc = GetComponent<app::fnd::GOCVisualContainer>())
    {
        pVisualContainerGoc->Setup({ ms_ModelCount });

        for (size_t i = 0; i < ms_ModelCount; i++)
        {
            if (auto* pVisualModelGoc = pVisualContainerGoc->CreateSingle<app::fnd::GOCVisualModel>(*this))
            {
                app::fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->Models[i];
                if (i == 2)
                    description.m_Skeleton = pInfo->WireSkeleton;

                pVisualModelGoc->Setup(description);

                pVisualContainerGoc->Add(pVisualModelGoc);
            }
        }
    }

    if (auto* pColliderGoc = GetComponent<app::game::GOCCollider>())
    {
        pColliderGoc->Setup({ ms_ShapeCount });

        app::game::ColliSphereShapeCInfo collisionInfo{};
        collisionInfo.m_Radius = ms_CollisionRadius;
        collisionInfo.m_Unk2 |= 1;
        collisionInfo.m_Unk3 = 0x20000;

        collisionInfo.SetLocalPosition({ 0.0f, -Length, 0.0f });

        app::ObjUtil::SetupCollisionFilter(app::ObjUtil::eFilter_Default, collisionInfo);
        pColliderGoc->CreateShape(collisionInfo);
    }

    app::fnd::GOComponent::EndSetup(*this);

    if (pParam->IsWaitUp && !GetExtUserData(eExtUserDataType_High))
    {
        SetHandleDistance(-11.9f);
        ChangeState(&ObjUpReel::StateWaitUp);

        return;
    }

    SetHandleDistance(-Length);

    InitFSM();
}

bool slw_extra_objects::ObjUpReel::ProcessMessage(app::fnd::Message& in_rMessage)
{
    if (PreProcessMessage(in_rMessage))
        return true;

    switch (in_rMessage.GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:           return ProcMsgHitEventCollision(static_cast<app::xgame::MsgHitEventCollision&>(in_rMessage));
    case app::xgame::MsgPLGetHomingTargetInfo::MessageID:       return ProcMsgPLGetHomingTargetInfo(static_cast<app::xgame::MsgPLGetHomingTargetInfo&>(in_rMessage));
    case app::xgame::MsgGetExternalMovePosition::MessageID:     return ProcMsgGetExternalMovePosition(static_cast<app::xgame::MsgGetExternalMovePosition&>(in_rMessage));
    case app::xgame::MsgNotifyObjectEvent::MessageID:           return ProcMsgNotifyObjectEvent(static_cast<app::xgame::MsgNotifyObjectEvent&>(in_rMessage));
    default:                                                    return CSetObjectListener::ProcessMessage(in_rMessage);
    }
}

bool slw_extra_objects::ObjUpReel::ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage)
{
    TTinyFsm::DispatchFSM(app::TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));

    return true;
}

bool slw_extra_objects::ObjUpReel::ProcMsgPLGetHomingTargetInfo(app::xgame::MsgPLGetHomingTargetInfo& in_rMessage)
{
    TTinyFsm::DispatchFSM(app::TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));

    return true;
}

bool slw_extra_objects::ObjUpReel::ProcMsgGetExternalMovePosition(app::xgame::MsgGetExternalMovePosition& in_rMessage)
{
    TTinyFsm::DispatchFSM(app::TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));

    return true;
}

bool slw_extra_objects::ObjUpReel::ProcMsgNotifyObjectEvent(app::xgame::MsgNotifyObjectEvent& in_rMessage)
{
    TTinyFsm::DispatchFSM(app::TiFsmBasicEvent<ObjUpReel>::CreateMessage(in_rMessage));

    return true;
}

void slw_extra_objects::ObjUpReel::Update(const app::fnd::SUpdateInfo& in_rUpdateInfo)
{
    TTinyFsm::DispatchFSM(app::TiFsmBasicEvent<ObjUpReel>::CreateUpdate(in_rUpdateInfo.deltaTime));
}

void slw_extra_objects::ObjUpReel::ChangeState(TiFsmState_t in_state)
{
    FSM_TRAN(in_state);
}

app::TTinyFsm<slw_extra_objects::ObjUpReel>::TiFsmState_t slw_extra_objects::ObjUpReel::StateIdle(const TiFsmEvent_t& in_rEvent)
{
    if (in_rEvent.getSignal() != TiFSM_SIGNAL_MESSAGE)
        return FSM_TOP();

    switch (in_rEvent.getMessage().GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:
    {
        auto& message = static_cast<app::xgame::MsgHitEventCollision&>(in_rEvent.getMessage());

        PlayerNo = app::ObjUtil::GetPlayerNo(*GetDocument(), message.m_Sender);
        if (PlayerNo < 0)
            break;

        ChangeState(&ObjUpReel::StateUp);

        app::xgame::MsgCatchPlayer catchMessage{};
        catchMessage.m_Unk3 = 21;
        app::ObjUtil::SendMessageImmToPlayer(*this, PlayerNo, catchMessage);
        break;
    }
    case app::xgame::MsgPLGetHomingTargetInfo::MessageID:
    {
        auto& message = static_cast<app::xgame::MsgPLGetHomingTargetInfo&>(in_rEvent.getMessage());

        auto* pHandleModel = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1]);
        if (!pHandleModel)
            break;

        csl::math::Matrix34 handleTransform{};
        pHandleModel->GetMatrix(&handleTransform);

        message.m_CursorPosition = handleTransform.GetTransVector();
        break;
    }
    }

    return FSM_TOP();
}

app::TTinyFsm<slw_extra_objects::ObjUpReel>::TiFsmState_t slw_extra_objects::ObjUpReel::StateWaitUp(const TiFsmEvent_t& in_rEvent)
{
    if (in_rEvent.getSignal() != TiFSM_SIGNAL_MESSAGE)
        return FSM_TOP();

    switch (in_rEvent.getMessage().GetType())
    {
    case app::xgame::MsgNotifyObjectEvent::MessageID:
    {
        auto& message = static_cast<app::xgame::MsgNotifyObjectEvent&>(in_rEvent.getMessage());

        if (!message.GetEventType())
            break;

        SetExtUserData(eExtUserDataType_High, 1);
        ChangeState(&ObjUpReel::StateDown);
        break;
    }
    }

    return FSM_TOP();
}

app::TTinyFsm<slw_extra_objects::ObjUpReel>::TiFsmState_t slw_extra_objects::ObjUpReel::StateUp(const TiFsmEvent_t& in_rEvent)
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
        auto handlePos = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1])->m_Transform.m_Mtx.GetTransVector();

        PositionOffset += in_rEvent.getFloat() * UpSpeedMax * 0.2f;
        if (abs(handlePos.y()) - PositionOffset >= 11.9f)
        {
            SetHandleDistance(-(abs(handlePos.y()) - PositionOffset));
            return FSM_TOP();
        }

        SetHandleDistance(-11.9f);

        auto position = GetComponent<app::fnd::GOCTransform>()->GetLocalPosition();
        position.y() += 2.9f;

        auto* pPlayer = dynamic_cast<app::Player::CPlayer*>(m_pMessageManager->GetActor(app::ObjUtil::GetPlayerActorID(*GetDocument(), PlayerNo)));
        pPlayer->GetPlayerGOC<app::Player::CVisualGOC>()->GetHumanAnimationPtr()->ExitLoop();

        /*auto* pPlayer = dynamic_cast<app::Player::CPlayer*>(m_pMessageManager->GetActor(app::ObjUtil::GetPlayerActorID(*m_pOwnerDocument, 0)));
        pPlayer->GetStateGOC()->ChangeAnimation("UPREEL_END");*/

        /*app::xgame::MsgSpringImpulse msg{position, GetForwardDirectionVector(1.0f, -0.4f), OutOfControl, 0.0f};
        msg.Flags.set(12);
        app::ObjUtil::SendMessageImmToPlayer(*this, PlayerNo, msg);

        ChangeState(&ObjUpReel::StateDown);*/

        break;
    }
    case TiFSM_SIGNAL_MESSAGE:
    {
        switch (in_rEvent.getMessage().GetType())
        {
        case app::xgame::MsgGetExternalMovePosition::MessageID:
        {
            auto& message = static_cast<app::xgame::MsgGetExternalMovePosition&>(in_rEvent.getMessage());

            auto* pHandleModel = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                break;

            pHandleModel->GetMatrix(message.pTrsMatrix);

            csl::math::Quaternion rotation{ *message.pTrsMatrix };
            rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), csl::math::Vector3::UnitY());

            for (size_t i = 0; i < 3; i++)
                message.pTrsMatrix->SetColumn(i, static_cast<csl::math::Vector3>(rotation.toRotationMatrix().col(i)));

            //message.m_pTransform->GetTransVector().y() += -10.5f;

            break;
        }
        }
        break;
    }
    }

    return FSM_TOP();
}

app::TTinyFsm<slw_extra_objects::ObjUpReel>::TiFsmState_t slw_extra_objects::ObjUpReel::StateDown(const TiFsmEvent_t& in_rEvent)
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

        auto handlePos = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1])->m_Transform.m_Mtx.GetTransVector();

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
        case app::xgame::MsgHitEventCollision::MessageID:
        {
            auto& message = static_cast<app::xgame::MsgHitEventCollision&>(in_rEvent.getMessage());

            PlayerNo = app::ObjUtil::GetPlayerNo(*GetDocument(), message.m_Sender);
            if (PlayerNo < 0)
                break;

            ChangeState(&ObjUpReel::StateUp);

            app::xgame::MsgCatchPlayer catchMessage{};
            catchMessage.m_Unk3 = 21;
            app::ObjUtil::SendMessageImmToPlayer(*this, PlayerNo, catchMessage);
            break;
        }
        case app::xgame::MsgPLGetHomingTargetInfo::MessageID:
        {
            auto& message = static_cast<app::xgame::MsgPLGetHomingTargetInfo&>(in_rEvent.getMessage());

            auto* pHandleModel = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                break;

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

csl::math::Vector3 slw_extra_objects::ObjUpReel::GetForwardDirectionVector(float in_up, float in_forward)
{
    return static_cast<csl::math::Vector3>(GetComponent<app::fnd::GOCTransform>()->GetLocalRotation() * csl::math::Vector3(0.0f, in_up, in_forward) * ImpulseVelocity);
}

void slw_extra_objects::ObjUpReel::MovePlayerWithHandle(csl::math::Matrix34* in_pTransform)
{
    auto* pHandleModel = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1]);
    if (!pHandleModel)
        return;

    pHandleModel->GetMatrix(in_pTransform);

    csl::math::Quaternion rotation{ *in_pTransform };
    rotation = { rotation * Eigen::AngleAxisf(TO_RAD(180), csl::math::Vector3::UnitY()) };

    for (size_t i = 0; i < 3; i++)
        in_pTransform->SetColumn(i, static_cast<csl::math::Vector3>(rotation.toRotationMatrix().row(i)));

    in_pTransform->GetTransVector().y() += -10.5f;
}

void slw_extra_objects::ObjUpReel::SetWireLength(float in_Length)
{
    auto* pWireModel = reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[2]);
    if (!pWireModel)
        return;

    app::math::Transform transform{};
    pWireModel->GetNodeTransform(2, "Cos_top", &transform);

    transform.m_Position.y() = in_Length;
    pWireModel->SetNodeTransform(2, "Cos_top", transform);

    auto bounds = pWireModel->GetBounds();
    bounds.m_Max.y() = in_Length;
    pWireModel->SetBounds(bounds);
}

void slw_extra_objects::ObjUpReel::SetHandleDistance(float in_Distance)
{
    reinterpret_cast<app::fnd::GOCVisualModel*>(GetComponent<app::fnd::GOCVisualContainer>()->m_Visuals[1])->SetLocalTranslation({ 0.0f, in_Distance, 0.0f });
    SetWireLength(in_Distance + 11.45f);
    GetComponent<app::game::GOCCollider>()->GetShape()->SetLocalTranslation({ 0.0f, in_Distance, 0.0f });
}
