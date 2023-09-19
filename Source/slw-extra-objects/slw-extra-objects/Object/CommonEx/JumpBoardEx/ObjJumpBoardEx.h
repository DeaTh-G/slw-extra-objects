#pragma once

namespace app
{
    class ObjJumpBoardEx : public CSetObjectListener
    {
    private:
        inline static const size_t ms_TextureAnimationCount = 3;
        inline static const size_t ms_ShapeCount = 1;
        inline static const size_t ms_PhysicsShapeCount = 1;
        inline static const Vector3 ms_CollisionSizes[] = { { 2.48f, 0.2f, 3.0f }, { 2.48f, 0.42f, 3.5f }, { 5.64f, 0.46f, 13.65f }, { 15.4f, 0.46f, 30.2f } };
        inline static const Vector3 ms_CollisionOffsets[] = { { 0.0f, 2.672f, 7.585f }, { 0.0f, 4.622f, 7.397f }, { 0.0f, 7.176f, 11.148f }, { 0.0f, 16.403f, 24.368f } };
        inline static const float ms_CollisionRotations[] = { -15.0f, -30.0f, -30.0f, -30.0f };

    protected:
        float ImpulseSpeedOnSpindash{};
        float ImpulseSpeedOnNormal{};
        float OutOfControl{};
        bool IsStand{};
        SJumpBoardExParam::EType Type{};
        bool IsOn{ true };
        float ElapsedTime{};

    public:
        ObjJumpBoardEx() {}

        void AddCallback(GameDocument& in_rDocument) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCPhysics>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjJumpBoardExInfo>(in_rDocument);
            auto* pParam = m_pAdapter->GetData<SJumpBoardExParam>();

            ImpulseSpeedOnSpindash = pParam->ImpulseSpeedOnSpindash;
            ImpulseSpeedOnNormal = pParam->ImpulseSpeedOnNormal;
            OutOfControl = pParam->OutOfControl;
            IsStand = pParam->IsStand;
            Type = pParam->Type;

            fnd::GOComponent::BeginSetup(*this);

            if (auto* pVisualGoc = GetComponent<fnd::GOCVisualModel>())
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->Models[Type];
                description.field_0C = 1;

                pVisualGoc->Setup(description);

                auto* pBlender = pVisualGoc->SetTexSrtBlender({ ms_TextureAnimationCount });
                for (size_t i = 0; i < ms_TextureAnimationCount - 1; i++)
                    pBlender->CreateControl({ pInfo->BeltTextureAnimations[i], 1 });

                pBlender->CreateControl({ pInfo->ArrowTextureAnimation, 1 });
            }

            if (auto* pColliderGoc = GetComponent<game::GOCCollider>())
            {
                pColliderGoc->Setup({ ms_ShapeCount });

                game::ColliBoxShapeCInfo collisionInfo{};
                collisionInfo.m_Size = ms_CollisionSizes[Type];
                collisionInfo.m_Unk2 |= 1;

                collisionInfo.SetLocalPosition(ms_CollisionOffsets[Type]);
                collisionInfo.SetLocalRotation(Eigen::AngleAxisf(TO_RAD(ms_CollisionRotations[Type]), Vector3::UnitX()));

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, collisionInfo);

                pColliderGoc->CreateShape(collisionInfo);
            }

            if (auto* pPhysics = GetComponent<game::GOCPhysics>())
            {
                pPhysics->Setup({ ms_PhysicsShapeCount });

                game::ColliMeshShapeCInfo colliInfo{};
                colliInfo.m_Flags = 8;
                colliInfo.m_Unk2 |= 0x100;
                colliInfo.m_Unk3 = 3;
                colliInfo.m_Mesh = pInfo->Collisions[Type];

                pPhysics->CreateShape(colliInfo);
            }

            game::GOCSound::SimpleSetup(this, 0, 0);

            fnd::GOComponent::EndSetup(*this);
        }

        bool ProcessMessage(fnd::Message& in_rMessage) override
        {
            if (PreProcessMessage(in_rMessage))
                return true;

            switch (in_rMessage.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:        return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(in_rMessage));
            default:                                            return CSetObjectListener::ProcessMessage(in_rMessage);
            }
        }

        void Update(const fnd::SUpdateInfo& in_rUpdateInfo) override
        {
            if (IsOn)
                return;

            ElapsedTime += in_rUpdateInfo.deltaTime;
            if (ElapsedTime >= 0.7f)
            {
                GetComponent<game::GOCPhysics>()->SetEnable(true);
                ElapsedTime = 0.0f;
                IsOn = true;
                return;
            }
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& in_rMessage)
        {
            if (!IsOn)
                return false;

            int playerNo = ObjUtil::GetPlayerNo(*GetDocument(), in_rMessage.m_Sender);
            if (playerNo < 0)
                return false;

            if (!CheckPlayerVelocity(playerNo))
                return false;

            GetComponent<game::GOCSound>()->Play3D("obj_dashpanel", {}, 0);
            GetComponent<game::GOCPhysics>()->SetEnable(false);

            IsOn = false;

            csl::math::Vector3 playerPosition{};
            xgame::MsgGetPosition playerPosMsg{ playerPosition };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);
            
            xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(playerNo), OutOfControl, 0.0f };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        bool CheckPlayerVelocity(int in_playerNo)
        {
            if (!IsStand)
                return true;

            auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*GetDocument(), in_playerNo)));
            float velocity = pPlayer->m_rpPhysics->GetHorzVelocity().dot(GetComponent<fnd::GOCTransform>()->GetLocalRotation() * csl::math::Vector3::UnitZ());

            if (velocity < 100.0f)
                return false;

            return true;
        }

        csl::math::Vector3 GetDirectionVector(int playerNo)
        {
            xgame::PlayerInformation* pPlayerInfo = ObjUtil::GetPlayerInformation(*GetDocument(), playerNo);
            if (!pPlayerInfo)
                return {};

            auto direction = GetComponent<fnd::GOCTransform>()->GetLocalRotation() * GetLaunchOffset() * csl::math::Vector3::UnitZ();

            if (pPlayerInfo->Unk37)
                return { direction * ImpulseSpeedOnSpindash };

            return { direction * ImpulseSpeedOnNormal };
        }

        csl::math::Quaternion GetLaunchOffset()
        {
            return { Eigen::AngleAxisf(TO_RAD(ms_CollisionRotations[Type]), csl::math::Vector3::UnitX()) };
        }
    };
}