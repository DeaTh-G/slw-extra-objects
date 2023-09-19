#include "pch.h"
#include "ObjSuperRingSuper.h"

slw_extra_objects::ObjSuperRingSuper::ObjSuperRingSuper() : app::CSetObjectListener()
{

}

void slw_extra_objects::ObjSuperRingSuper::AddCallback(app::GameDocument& in_rDocument)
{
    app::fnd::GOComponent::Create<app::fnd::GOCVisualModel>(*this);
    app::fnd::GOComponent::Create<app::game::GOCCollider>(*this);
    app::fnd::GOComponent::Create<app::game::GOCEffect>(*this);
    app::fnd::GOComponent::Create<app::game::GOCSound>(*this);

    auto* pInfo = app::ObjUtil::GetObjectInfo<ObjSuperRingSuperInfo>(in_rDocument);

    app::fnd::GOComponent::BeginSetup(*this);

    if (auto* pVisualGoc = GetComponent<app::fnd::GOCVisualModel>())
    {
        app::fnd::GOCVisualModel::Description description{};
        description.m_Model = pInfo->Model;

        pVisualGoc->Setup(description);
    }

    auto* pManager = in_rDocument.GetService<app::Gimmick::CRingManager>();
    pManager->RegisterRotateRing(this);

    if (auto* pColliderGoc = GetComponent<app::game::GOCCollider>())
    {
        app::game::GOCCollider::Description description{ ms_ShapeCount };
        pColliderGoc->Setup(description);

        app::game::ColliSphereShapeCInfo collisionInfo;
        collisionInfo.m_Radius = ms_CollisionRadius;
        collisionInfo.m_Unk2 |= 1;
        collisionInfo.m_Unk3 = 0x20000;

        app::ObjUtil::SetupCollisionFilter(app::ObjUtil::eFilter_Default, collisionInfo);
        pColliderGoc->CreateShape(collisionInfo);
    }

    if (GetComponent<app::game::GOCEffect>())
        app::game::GOCEffect::SimpleSetup(this, 0, false);
    
    app::game::GOCSound::SimpleSetup(this, 0, 0);

    app::fnd::GOComponent::EndSetup(*this);
}

bool slw_extra_objects::ObjSuperRingSuper::ProcessMessage(app::fnd::Message& in_rMessage)
{
    if (PreProcessMessage(in_rMessage))
        return true;

    switch (in_rMessage.GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:       return ProcMsgHitEventCollision(reinterpret_cast<app::xgame::MsgHitEventCollision&>(in_rMessage));
    default:                                                return CSetObjectListener::ProcessMessage(in_rMessage);
    }
}

bool slw_extra_objects::ObjSuperRingSuper::ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage)
{
    app::xgame::MsgTakeObject msg{ (app::xgame::MsgTakeObject::EType)19 };
    msg.SetShapeUserID(in_rMessage.m_pOther->m_ID);
    SendMessageImm(in_rMessage.m_Sender, msg);
    if (!msg.m_Taken)
        return false;

    GetComponent<app::game::GOCEffect>()->CreateEffect(ms_pEffectName);
    GetComponent<app::game::GOCSound>()->Play3D(ms_pSoundName, 0.0f);

    SetStatusRetire();
    Kill();
    return true;
}