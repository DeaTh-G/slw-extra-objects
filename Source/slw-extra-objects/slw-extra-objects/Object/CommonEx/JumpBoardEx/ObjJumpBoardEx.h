#pragma once

namespace app
{
    class ObjJumpBoardEx : public CSetObjectListener
    {
    private:
        inline static Vector3 ms_JumpBoardSizes[] = { { 5.48f, 0.2f, 8.49f }, { 5.48f, 0.42f, 9.0f }, { 8.64f, 0.46f, 13.65f }, { 19.4f, 0.46f, 30.2f } };
        inline static Vector3 ms_JumpBoardPositions[] = { { 0.0f, 2.672f, 7.585f }, { 0.0f, 4.622f, 7.397f }, { 0.0f, 7.176f, 11.148f }, { 0.0f, 16.403f, 24.368f } };
        inline static float ms_JumpBoardRotations[] = { -15, -30, -30, -30 };

    protected:
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
            auto* pParam = reinterpret_cast<SJumpBoardExParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[pParam->m_Type];

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
                colliInfo.m_Size = ms_JumpBoardSizes[pParam->m_Type];
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalPosition(ms_JumpBoardPositions[pParam->m_Type]);
                colliInfo.SetLocalRotation(Eigen::Quaternionf(Eigen::AngleAxisf(ms_JumpBoardRotations[pParam->m_Type] * MATHF_PI / 180, Eigen::Vector3f::UnitX())));

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);

                pCollider->CreateShape(colliInfo);
            }

            auto* pPhysics = GetComponent<game::GOCPhysics>();
            if (pPhysics)
            {
                pPhysics->Setup({ 1 });

                game::ColliMeshShapeCInfo colliInfo{};
                colliInfo.m_Flags = 14;
                colliInfo.m_Unk2 |= 0x100;
                colliInfo.m_Mesh = pInfo->m_Colliders[pParam->m_Type];

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
                SLW_EXTRA_OBJECTS::GOCPhysics::SetEnable(GetComponent<game::GOCPhysics>(), true);
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

            int* playerInfo = SLW_EXTRA_OBJECTS::ObjUtil::GetPlayerInformation(*m_pOwnerDocument, playerNo);
            if (!playerInfo)
                return false;

            if ((*(Vector3*)(playerInfo + 12)).z() < 100.0f)
                return false;
            
            auto* pSound = GetComponent<game::GOCSound>();
            auto* pParam = reinterpret_cast<SJumpBoardExParam*>(m_pAdapter->GetData());

            int deviceTag[3];
            SLW_EXTRA_OBJECTS::GOCSound::Play3D(pSound, deviceTag, "obj_dashpanel", 0);

            SLW_EXTRA_OBJECTS::GOCPhysics::SetEnable(GetComponent<game::GOCPhysics>(), false);
            m_IsOn = false;
            
            xgame::MsgSpringImpulse impulseMsg{ *(Vector3*)(playerInfo + 4), GetDirectionVector(playerInfo), pParam->m_OutOfControl, 1 };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        Vector3 GetDirectionVector(int* playerInfo)
        {
            auto* pParam = reinterpret_cast<SJumpBoardExParam*>(m_pAdapter->GetData());

            auto launchAngle = GetComponent<fnd::GOCTransform>()->GetLocalRotation() * GetLaunchOffset(pParam->m_Type);

            if (*((bool*)playerInfo + 368))
                return static_cast<Vector3>(launchAngle * Eigen::Vector3f::UnitZ() * pParam->m_ImpulseSpeedOnSpindash);
            
            return static_cast<Vector3>(launchAngle * Eigen::Vector3f::UnitZ() * pParam->m_ImpulseSpeedOnNormal);
        }

        Quaternion GetLaunchOffset(SJumpBoardExParam::EType type)
        {
            return Eigen::Quaternionf(Eigen::AngleAxisf(ms_JumpBoardRotations[type] * MATHF_PI / 180, Eigen::Vector3f::UnitX()));
        }
    };
}