#include "pch.h"
#include "ObjWideSpring.h"

slw_extra_objects::ObjWideSpring::ObjWideSpring() : app::CSetObjectListener()
{
    SetProperty(0x4002, { 9 });
}

void slw_extra_objects::ObjWideSpring::AddCallback(app::GameDocument& in_rDocument)
{
    app::fnd::GOComponent::Create<app::fnd::GOCVisualModel>(*this);
    app::fnd::GOComponent::Create<app::game::GOCCollider>(*this);
    app::fnd::GOComponent::Create<app::game::GOCSound>(*this);
    app::fnd::GOComponent::Create<app::game::GOCAnimationSimple>(*this);

    auto* pInfo = app::ObjUtil::GetObjectInfo<ObjWideSpringInfo>(in_rDocument);
    auto* pParam = GetAdapter()->GetData<SWideSpringParam>();

    FirstSpeed = pParam->FirstSpeed;
    KeepVelocityDistance = pParam->KeepVelocityDistance;
    OutOfControl = pParam->OutOfControl;

    app::fnd::GOComponent::BeginSetup(*this);

    if (auto* pVisualGoc = GetComponent<app::fnd::GOCVisualModel>())
    {
        app::fnd::GOCVisualModel::Description description{};
        description.m_Model = pInfo->Model;
        description.m_Skeleton = pInfo->Skeleton;

        pVisualGoc->Setup(description);

        if (auto* pAnimationGoc = GetComponent<app::game::GOCAnimationSimple>())
        {
            pAnimationGoc->Setup({ ms_AnimationCount });
            pAnimationGoc->Add(ms_pAnimationName, pInfo->Animation, app::game::PlayPolicy::Once);
            pVisualGoc->AttachAnimation(pAnimationGoc);
        }
    }

    if (auto* pColliderGoc = GetComponent<app::game::GOCCollider>())
    {
        pColliderGoc->Setup({ 1 });

        app::game::ColliCapsuleShapeCInfo collisionInfo{};
        collisionInfo.m_Radius = ms_CollisionRadius;
        collisionInfo.m_Height = ms_CollisionHeight;
        collisionInfo.m_Unk2 |= 1;
        collisionInfo.m_Unk3 = 0x20000;

        collisionInfo.SetLocalPosition(ms_CollisionOffset);
        collisionInfo.SetLocalRotation(ms_CollisionRotation);

        app::ObjUtil::SetupCollisionFilter(app::ObjUtil::eFilter_Default, collisionInfo);
        pColliderGoc->CreateShape(collisionInfo);
    }

    if (pParam->IsEventOn && !GetExtUserData(eExtUserDataType_High))
    {
        GetComponent<app::fnd::GOCVisualModel>()->SetVisible(false);
        GetComponent<app::game::GOCCollider>()->SetEnable(false);
    }

    app::game::GOCSound::SimpleSetup(this, 0, 0);

    app::fnd::GOComponent::EndSetup(*this);
}

bool slw_extra_objects::ObjWideSpring::ProcessMessage(app::fnd::Message& in_rMessage)
{
    if (PreProcessMessage(in_rMessage))
        return true;

    switch (in_rMessage.GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:           return ProcMsgHitEventCollision(reinterpret_cast<app::xgame::MsgHitEventCollision&>(in_rMessage));
    case app::xgame::MsgPLGetHomingTargetInfo::MessageID:       return ProcMsgPLGetHomingTargetInfo(reinterpret_cast<app::xgame::MsgPLGetHomingTargetInfo&>(in_rMessage));
    case app::xgame::MsgNotifyObjectEvent::MessageID:           return ProcMsgNotifyObjectEvent(reinterpret_cast<app::xgame::MsgNotifyObjectEvent&>(in_rMessage));
    default:                                                    return CSetObjectListener::ProcessMessage(in_rMessage);
    }
}

bool slw_extra_objects::ObjWideSpring::ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage)
{
    int playerNo = app::ObjUtil::GetPlayerNo(*GetDocument(), in_rMessage.m_Sender);
    if (playerNo < 0)
        return false;

    GetComponent<app::game::GOCSound>()->Play3D(ms_pSoundName, 0.0f);
    GetComponent<app::game::GOCAnimationSimple>()->SetAnimation(ms_pAnimationName);

    csl::math::Vector3 playerPosition{};
    app::xgame::MsgGetPosition playerPosMsg{ playerPosition };
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

    app::xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(), OutOfControl, KeepVelocityDistance / FirstSpeed };
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

    return true;
}

bool slw_extra_objects::ObjWideSpring::ProcMsgPLGetHomingTargetInfo(app::xgame::MsgPLGetHomingTargetInfo& in_rMessage)
{
    in_rMessage.m_CursorPosition = { GetComponent<app::fnd::GOCTransform>()->m_Frame.m_Unk3.m_Mtx * csl::math::Vector4(0.0f, 3.5f, 0.0f, 1.0f) };

    return true;
}

bool slw_extra_objects::ObjWideSpring::ProcMsgNotifyObjectEvent(app::xgame::MsgNotifyObjectEvent& in_rMessage)
{
    if (!in_rMessage.GetEventType())
        return false;

    GetComponent<app::fnd::GOCVisualModel>()->SetVisible(true);
    GetComponent<app::game::GOCCollider>()->SetEnable(true);

    SetExtUserData(eExtUserDataType_High, 1);

    return true;
}

csl::math::Vector3 slw_extra_objects::ObjWideSpring::GetDirectionVector()
{
    return { csl::math::Vector3::UnitY() * FirstSpeed };
}
