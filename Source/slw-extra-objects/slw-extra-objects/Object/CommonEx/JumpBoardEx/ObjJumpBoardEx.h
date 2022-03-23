#pragma once

namespace app
{
    class ObjJumpBoardEx : public CSetObjectListener
    {
    private:
        inline static Vector3 ms_JumpBoardSizes[] = { { 2.48f, 0.2f, 3.0f }, { 2.48f, 0.42f, 3.5f }, { 5.64f, 0.46f, 13.65f }, { 15.4f, 0.46f, 30.2f } };
        inline static Vector3 ms_JumpBoardPositions[] = { { 0.0f, 2.672f, 7.585f }, { 0.0f, 4.622f, 7.397f }, { 0.0f, 7.176f, 11.148f }, { 0.0f, 16.403f, 24.368f } };
        inline static float ms_JumpBoardRotations[] = { -15, -30, -30, -30 };

    protected:
        float m_ImpulseSpeedOnSpindash{};
        float m_ImpulseSpeedOnNormal{};
        float m_OutOfControl{};
        bool m_IsStand{};
        SJumpBoardExParam::EType m_Type{};
        bool m_IsOn{ true };
        float m_Time{};

    public:
        ObjJumpBoardEx() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCPhysics>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjJumpBoardExInfo>(document, "ObjJumpBoardExInfo");
            auto pParam = reinterpret_cast<SJumpBoardExParam*>(m_pAdapter->GetData());

            m_ImpulseSpeedOnSpindash = pParam->m_ImpulseSpeedOnSpindash;
            m_ImpulseSpeedOnNormal = pParam->m_ImpulseSpeedOnNormal;
            m_OutOfControl = pParam->m_OutOfControl;
            m_IsStand = pParam->m_IsStand;
            m_Type = pParam->m_Type;

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[m_Type];

                pVisual->Setup(description);

                auto* pBlender = pVisual->SetTexSrtBlender({ ObjJumpBoardExInfo::ms_BeltAnimCount + 1 });
                for (size_t i = 0; i < ObjJumpBoardExInfo::ms_BeltAnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_BeltAnimations[i], 1 });

                pBlender->CreateControl({ pInfo->m_ArrowAnimation, 1 });
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliBoxShapeCInfo colliInfo{};
                colliInfo.m_Size = ms_JumpBoardSizes[m_Type];
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalPosition(ms_JumpBoardPositions[m_Type]);
                colliInfo.SetLocalRotation(Eigen::Quaternionf(Eigen::AngleAxisf(TO_RAD(ms_JumpBoardRotations[m_Type]), Vector3::UnitX())));

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);

                pCollider->CreateShape(colliInfo);
            }

            auto* pPhysics = GetComponent<game::GOCPhysics>();
            if (pPhysics)
            {
                pPhysics->Setup({ 1 });

                game::ColliMeshShapeCInfo colliInfo{};
                colliInfo.m_Flags = 8;
                colliInfo.m_Unk2 |= 0x100;
                colliInfo.m_Unk3 = 3;
                colliInfo.m_Mesh = pInfo->m_Colliders[m_Type];

                pPhysics->CreateShape(colliInfo);
            }

            game::GOCSound::SimpleSetup(this, 0, 0);

            fnd::GOComponent::EndSetup(*this);
        }

        bool ProcessMessage(fnd::Message& msg) override
        {
            if (PreProcessMessage(msg))
                return true;

            switch (msg.GetType())
            {
            case xgame::MsgHitEventCollision::MessageID:
                return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(msg));
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
        }

        void Update(const fnd::SUpdateInfo& rInfo) override
        {
            if (m_IsOn)
                return;

            m_Time += rInfo.deltaTime;
            if (m_Time >= 0.7f)
            {
                GetComponent<game::GOCPhysics>()->SetEnable(true);
                m_Time = 0.0f;
                m_IsOn = true;
                return;
            }
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            if (!m_IsOn)
                return false;

            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            if (!CheckPlayerVelocity(playerNo))
                return false;

            GetComponent<game::GOCSound>()->Play3D("obj_dashpanel", {}, 0);
            GetComponent<game::GOCPhysics>()->SetEnable(false);

            m_IsOn = false;

            xgame::MsgGetPosition playerPosMsg{};
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);
            
            xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(playerNo), m_OutOfControl, 0 };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        bool CheckPlayerVelocity(int playerNo)
        {
            if (!m_IsStand)
                return true;

            auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(ObjUtil::GetPlayerActorID(*m_pOwnerDocument, playerNo)));
            float velocity = pPlayer->m_pPhysics->GetHorzVelocity().dot(GetComponent<fnd::GOCTransform>()->GetLocalRotation() * Vector3::UnitZ());

            if (velocity < 100.0f)
                return false;

            return true;
        }

        Vector3 GetDirectionVector(int playerNo)
        {
            int* playerInfo = SLW_EXTRA_OBJECTS::ObjUtil::GetPlayerInformation(*m_pOwnerDocument, playerNo);
            if (!playerInfo)
                return {};

            auto direction = GetComponent<fnd::GOCTransform>()->GetLocalRotation() * GetLaunchOffset() * Vector3::UnitZ();

            if (*((bool*)playerInfo + 368))
                return static_cast<Vector3>(direction * m_ImpulseSpeedOnSpindash);

            return static_cast<Vector3>(direction * m_ImpulseSpeedOnNormal);
        }

        Quaternion GetLaunchOffset()
        {
            return Eigen::Quaternionf(Eigen::AngleAxisf(TO_RAD(ms_JumpBoardRotations[m_Type]), Vector3::UnitX()));
        }
    };
}