#include "pch.h"
#include "ObjDashRing.h"

slw_extra_objects::ObjDashRing::ObjDashRing() : app::CSetObjectListener()
{

}

void slw_extra_objects::ObjDashRing::AddCallback(app::GameDocument& in_rDocument)
{
    app::fnd::GOComponent::Create<app::fnd::GOCVisualModel>(*this);
    app::fnd::GOComponent::Create<app::game::GOCCollider>(*this);
    app::fnd::GOComponent::Create<app::game::GOCSound>(*this);

    auto* pInfo = app::ObjUtil::GetObjectInfo<ObjDashRingInfo>(in_rDocument);
    auto* pParam = GetAdapter()->GetData<SDashRingParam>();

    FirstSpeed = pParam->FirstSpeed;
    OutOfControl = pParam->OutOfControl;
    KeepVelocityDistance = pParam->KeepVelocityDistance;
    Type = pParam->Type;

    app::fnd::GOComponent::BeginSetup(*this);

    if (auto* pVisualGoc = GetComponent<app::fnd::GOCVisualModel>())
    {
        app::fnd::GOCVisualModel::Description description{};
        description.m_Model = pInfo->Models[Type];

        pVisualGoc->Setup(description);

        pVisualGoc->SetMaterialAnimation({ pInfo->MaterialAnimations[Type], 1 });

        auto* pBlender = pVisualGoc->SetTexSrtBlender({ 4 });
        for (size_t i = 0; i < 4; i++)
            pBlender->CreateControl({ pInfo->TextureAnimations[Type * 4 + i], 1 });
    }

    if (auto* pColliderGoc = GetComponent<app::game::GOCCollider>())
    {
        pColliderGoc->Setup({ ms_ShapeCount });

        app::game::ColliCylinderShapeCInfo collisionInfo{};
        collisionInfo.m_Radius = ms_CollisionRadius;
        collisionInfo.m_Height = ms_CollisionHeight;
        collisionInfo.m_Unk2 |= 1;

        collisionInfo.SetLocalRotation({ Eigen::AngleAxisf(MATHF_PI / 2.0f, csl::math::Vector3::UnitX()) });

        app::ObjUtil::SetupCollisionFilter(app::ObjUtil::eFilter_Unk12, collisionInfo);
        pColliderGoc->CreateShape(collisionInfo);
    }

    app::game::GOCSound::SimpleSetup(this, 0, 0);

    app::fnd::GOComponent::EndSetup(*this);
}

bool slw_extra_objects::ObjDashRing::ProcessMessage(app::fnd::Message& in_rMessage)
{
    if (PreProcessMessage(in_rMessage))
        return true;

    switch (in_rMessage.GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:       return ProcMsgHitEventCollision(reinterpret_cast<app::xgame::MsgHitEventCollision&>(in_rMessage));
    default:                                                return CSetObjectListener::ProcessMessage(in_rMessage);
    }
}

void slw_extra_objects::ObjDashRing::Update(const app::fnd::SUpdateInfo& in_rUpdateInfo)
{
    if (!DoSquash)
        return;

    ElapsedTime += in_rUpdateInfo.deltaTime;
    if (ElapsedTime >= ms_SquashEndTime)
    {
        DoSquash = false;
        ElapsedTime = 0.0f;
        return;
    }

    float scale = cosf(ElapsedTime * 1.0f / ms_SquashEndTime * MATHF_PI * 2.0f);
    GetComponent<app::fnd::GOCVisualModel>()->SetLocalScale({ scale, scale, 1.0f });
}

bool slw_extra_objects::ObjDashRing::ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage)
{
    int playerNo = app::ObjUtil::GetPlayerNo(*GetDocument(), in_rMessage.m_Sender);
    if (playerNo < 0)
        return false;

    GetComponent<app::game::GOCSound>()->Play3D(ms_SoundNames[Type], {}, 0);

    DoSquash = true;

    app::xgame::MsgSpringImpulse impulseMsg{ GetComponent<app::fnd::GOCTransform>()->GetLocalPosition(), GetDirectionVector(), OutOfControl, KeepVelocityDistance / FirstSpeed };
    impulseMsg.Flags.set(18);
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

    return true;
}

csl::math::Vector3 slw_extra_objects::ObjDashRing::GetDirectionVector()
{
    return static_cast<csl::math::Vector3>(GetComponent<app::fnd::GOCTransform>()->GetLocalRotation() * csl::math::Vector3::UnitZ() * FirstSpeed);
}