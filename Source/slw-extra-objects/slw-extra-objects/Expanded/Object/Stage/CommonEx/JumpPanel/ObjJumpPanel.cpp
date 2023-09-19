#include "pch.h"
#include "ObjJumpPanel.h"

slw_extra_objects::ObjJumpPanel::ObjJumpPanel() : app::CSetObjectListener()
{

}

void slw_extra_objects::ObjJumpPanel::AddCallback(app::GameDocument& in_rDocument)
{
    app::fnd::GOComponent::Create<app::fnd::GOCVisualModel>(*this);
    app::fnd::GOComponent::Create<app::game::GOCCollider>(*this);
    app::fnd::GOComponent::Create<app::game::GOCSound>(*this);

    auto* pInfo = app::ObjUtil::GetObjectInfo<ObjJumpPanelInfo>(in_rDocument);
    auto* pParam = GetAdapter()->GetData<SJumpPanelParam>();

    FirstSpeed = pParam->FirstSpeed;
    KeepVelocityDistance = pParam->KeepVelocityDistance;
    OutOfControl = pParam->OutOfControl;
    Pitch = pParam->Pitch;
    TargetID = pParam->TargetID;
    Type = pParam->Type;

    app::fnd::GOComponent::BeginSetup(*this);

    if (auto* pVisualGoc = GetComponent<app::fnd::GOCVisualModel>())
    {
        app::fnd::GOCVisualModel::Description description{};
        description.m_Model = pInfo->Models[Type];

        pVisualGoc->Setup(description);

        auto* pBlender = pVisualGoc->SetTexSrtBlender({ ms_TextureAnimationCount });

        for (size_t i = 0; i < ms_TextureAnimationCount; i++)
            pBlender->CreateControl({ pInfo->TextureAnimations[i], 1 });
    }

    if (auto* pColliderGoc = GetComponent<app::game::GOCCollider>())
    {
        pColliderGoc->Setup({ Type + 1 });

        for (size_t i = 0; i < pColliderGoc->m_Shapes.capacity(); i++)
        {
            app::game::ColliBoxShapeCInfo collisionInfo{};
            collisionInfo.m_Size = ms_CollisionSizes[Type + i];
            collisionInfo.m_Unk2 |= 1;

            collisionInfo.SetLocalPosition(ms_CollisionOffsets[Type + i]);

            if (Type == SJumpPanelParam::eType_Upwards)
                collisionInfo.SetLocalRotation(Eigen::AngleAxisf(TO_RAD(ms_UpRotations[i]), csl::math::Vector3::UnitX()));

            app::ObjUtil::SetupCollisionFilter(app::ObjUtil::eFilter_Unk12, collisionInfo);
            pColliderGoc->CreateShape(collisionInfo);
        }
    }

    app::game::GOCSound::SimpleSetup(this, 0, 0);

    app::fnd::GOComponent::EndSetup(*this);
}

bool slw_extra_objects::ObjJumpPanel::ProcessMessage(app::fnd::Message& in_rMessage)
{
    if (PreProcessMessage(in_rMessage))
        return true;

    switch (in_rMessage.GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:       return ProcMsgHitEventCollision(reinterpret_cast<app::xgame::MsgHitEventCollision&>(in_rMessage));
    default:                                                return CSetObjectListener::ProcessMessage(in_rMessage);
    }
}

void slw_extra_objects::ObjJumpPanel::Update(const app::fnd::SUpdateInfo& in_rUpdateInfo)
{
    if (IsOn)
        return;

    ElapsedTime += in_rUpdateInfo.deltaTime;
    if (ElapsedTime >= 0.7f)
    {
        GetComponent<app::game::GOCCollider>()->SetEnable(true);
        ElapsedTime = 0.0f;
        IsOn = true;
        return;
    }
}

bool slw_extra_objects::ObjJumpPanel::ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage)
{
    if (!IsOn)
        return false;

    int playerNo = app::ObjUtil::GetPlayerNo(*m_pOwnerDocument, in_rMessage.m_Sender);
    if (playerNo < 0)
        return false;

    GetComponent<app::game::GOCSound>()->Play3D(ms_pSoundName, 0.0f);
    GetComponent<app::game::GOCCollider>()->SetEnable(false);

    IsOn = false;

    csl::math::Vector3 playerPosition{};
    app::xgame::MsgGetPosition playerPosMsg{ playerPosition };
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

    app::xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(), OutOfControl, KeepVelocityDistance / FirstSpeed };
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

    return true;
}

csl::math::Vector3 slw_extra_objects::ObjJumpPanel::GetDirectionVector()
{
    auto* pTransformGoc = GetComponent<app::fnd::GOCTransform>();
    csl::math::Vector3 direction{};

    if (!TargetID.Value)
    {
        auto launchAngle = pTransformGoc->GetLocalRotation() * GetLaunchOffset() * GetPitchCorrection();
        return { launchAngle * csl::math::Vector3::UnitZ() * FirstSpeed };
    }
    else
    {
        csl::math::Vector3 targetPosition{};
        app::ObjUtil::GetSetObjectTransform(*m_pOwnerDocument, TargetID, &targetPosition, nullptr);

        csl::math::Vector3 direction = { targetPosition - pTransformGoc->GetLocalPosition() };
        direction.normalize();

        return { direction * FirstSpeed };
    }
}

csl::math::Quaternion slw_extra_objects::ObjJumpPanel::GetLaunchOffset()
{
    return { Eigen::AngleAxisf(TO_RAD(ms_LaunchOffsets[Type]), csl::math::Vector3::UnitX()) };
}

csl::math::Quaternion slw_extra_objects::ObjJumpPanel::GetPitchCorrection()
{
    return { Eigen::AngleAxisf(TO_RAD(Pitch), csl::math::Vector3::UnitY()) };
}