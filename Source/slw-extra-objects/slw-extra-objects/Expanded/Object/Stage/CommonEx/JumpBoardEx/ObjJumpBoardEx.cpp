#include "pch.h"
#include "ObjJumpBoardEx.h"

slw_extra_objects::ObjJumpBoardEx::ObjJumpBoardEx() : app::CSetObjectListener()
{

}

void slw_extra_objects::ObjJumpBoardEx::AddCallback(app::GameDocument& in_rDocument)
{
    app::fnd::GOComponent::Create<app::fnd::GOCVisualModel>(*this);
    app::fnd::GOComponent::Create<app::game::GOCCollider>(*this);
    app::fnd::GOComponent::Create<app::game::GOCPhysics>(*this);
    app::fnd::GOComponent::Create<app::game::GOCSound>(*this);

    auto* pInfo = app::ObjUtil::GetObjectInfo<ObjJumpBoardExInfo>(in_rDocument);
    auto* pParam = m_pAdapter->GetData<SJumpBoardExParam>();

    ImpulseSpeedOnSpindash = pParam->ImpulseSpeedOnSpindash;
    ImpulseSpeedOnNormal = pParam->ImpulseSpeedOnNormal;
    OutOfControl = pParam->OutOfControl;
    IsStand = pParam->IsStand;
    Type = pParam->Type;

    app::fnd::GOComponent::BeginSetup(*this);

    if (auto* pVisualGoc = GetComponent<app::fnd::GOCVisualModel>())
    {
        app::fnd::GOCVisualModel::Description description{};
        description.m_Model = pInfo->Models[Type];
        description.field_0C = 1;

        pVisualGoc->Setup(description);

        auto* pBlender = pVisualGoc->SetTexSrtBlender({ ms_TextureAnimationCount });
        for (size_t i = 0; i < ms_TextureAnimationCount - 1; i++)
            pBlender->CreateControl({ pInfo->BeltTextureAnimations[i], 1 });

        pBlender->CreateControl({ pInfo->ArrowTextureAnimation, 1 });
    }

    if (auto* pColliderGoc = GetComponent<app::game::GOCCollider>())
    {
        pColliderGoc->Setup({ ms_ShapeCount });

        app::game::ColliBoxShapeCInfo collisionInfo{};
        collisionInfo.m_Size = ms_CollisionSizes[Type];
        collisionInfo.m_Unk2 |= 1;

        collisionInfo.SetLocalPosition(ms_CollisionOffsets[Type]);
        collisionInfo.SetLocalRotation({ Eigen::AngleAxisf(TO_RAD(ms_CollisionRotations[Type]), csl::math::Vector3::UnitX()) });

        app::ObjUtil::SetupCollisionFilter(app::ObjUtil::eFilter_Unk12, collisionInfo);

        pColliderGoc->CreateShape(collisionInfo);
    }

    if (auto* pPhysics = GetComponent<app::game::GOCPhysics>())
    {
        pPhysics->Setup({ ms_PhysicsShapeCount });

        app::game::ColliMeshShapeCInfo collisionInfo{};
        collisionInfo.m_Flags = 8;
        collisionInfo.m_Unk2 |= 0x100;
        collisionInfo.m_Unk3 = 3;
        collisionInfo.m_Mesh = pInfo->Collisions[Type];

        pPhysics->CreateShape(collisionInfo);
    }

    app::game::GOCSound::SimpleSetup(this, 0, 0);

    app::fnd::GOComponent::EndSetup(*this);
}

bool slw_extra_objects::ObjJumpBoardEx::ProcessMessage(app::fnd::Message& in_rMessage)
{
    if (PreProcessMessage(in_rMessage))
        return true;

    switch (in_rMessage.GetType())
    {
    case app::xgame::MsgHitEventCollision::MessageID:       return ProcMsgHitEventCollision(reinterpret_cast<app::xgame::MsgHitEventCollision&>(in_rMessage));
    default:                                                return CSetObjectListener::ProcessMessage(in_rMessage);
    }
}

void slw_extra_objects::ObjJumpBoardEx::Update(const app::fnd::SUpdateInfo& in_rUpdateInfo)
{
    if (IsOn)
        return;

    ElapsedTime += in_rUpdateInfo.deltaTime;
    if (ElapsedTime >= 0.7f)
    {
        GetComponent<app::game::GOCPhysics>()->SetEnable(true);
        ElapsedTime = 0.0f;
        IsOn = true;
        return;
    }
}

bool slw_extra_objects::ObjJumpBoardEx::ProcMsgHitEventCollision(app::xgame::MsgHitEventCollision& in_rMessage)
{
    if (!IsOn)
        return false;

    int playerNo = app::ObjUtil::GetPlayerNo(*GetDocument(), in_rMessage.m_Sender);
    if (playerNo < 0)
        return false;

    if (!CheckPlayerVelocity(playerNo))
        return false;

    GetComponent<app::game::GOCSound>()->Play3D("obj_dashpanel", {}, 0);
    GetComponent<app::game::GOCPhysics>()->SetEnable(false);

    IsOn = false;

    csl::math::Vector3 playerPosition{};
    app::xgame::MsgGetPosition playerPosMsg{ playerPosition };
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

    app::xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(playerNo), OutOfControl, 0.0f };
    app::ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

    return true;
}

bool slw_extra_objects::ObjJumpBoardEx::CheckPlayerVelocity(int in_playerNo)
{
    if (!IsStand)
        return true;

    auto* pPlayer = dynamic_cast<app::Player::CPlayer*>(m_pMessageManager->GetActor(app::ObjUtil::GetPlayerActorID(*GetDocument(), in_playerNo)));
    float velocity = pPlayer->m_rpPhysics->GetHorzVelocity().dot(GetComponent<app::fnd::GOCTransform>()->GetLocalRotation() * csl::math::Vector3::UnitZ());

    if (velocity < 100.0f)
        return false;

    return true;
}

csl::math::Vector3 slw_extra_objects::ObjJumpBoardEx::GetDirectionVector(int in_playerNo)
{
    app::xgame::PlayerInformation* pPlayerInfo = app::ObjUtil::GetPlayerInformation(*GetDocument(), in_playerNo);
    if (!pPlayerInfo)
        return {};

    auto direction = GetComponent<app::fnd::GOCTransform>()->GetLocalRotation() * GetLaunchOffset() * csl::math::Vector3::UnitZ();

    if (pPlayerInfo->Unk37)
        return { direction * ImpulseSpeedOnSpindash };

    return { direction * ImpulseSpeedOnNormal };
}

csl::math::Quaternion slw_extra_objects::ObjJumpBoardEx::GetLaunchOffset()
{
    return { Eigen::AngleAxisf(TO_RAD(ms_CollisionRotations[Type]), csl::math::Vector3::UnitX()) };
}