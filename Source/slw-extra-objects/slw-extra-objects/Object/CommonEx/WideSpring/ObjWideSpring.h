#pragma once

namespace app
{
    class ObjWideSpring : public CSetObjectListener
    {
    protected:
        inline static const size_t ms_AnimationCount = 1;
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 6.0f;
        inline static const float ms_CollisionHeight = 12.0f;
        inline static const csl::math::Vector3 ms_CollisionOffset = { 0.0f, 3.5f, 0.0f };
        inline static const csl::math::Quaternion ms_CollisionRotation = { Eigen::AngleAxisf(TO_RAD(90.0f), Vector3::UnitZ()) };
        inline static const char* ms_pAnimationName = "HIT";
        inline static const char* ms_pSoundName = "obj_spring";

    public:
        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};

    public:
        ObjWideSpring()
        {
            SetProperty(0x4002, { 9 });
        }

        void AddCallback(GameDocument& in_rDocument) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);
            fnd::GOComponent::Create<game::GOCAnimationSimple>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjWideSpringInfo>(in_rDocument);
            auto* pParam = GetAdapter()->GetData<SWideSpringParam>();

            FirstSpeed = pParam->FirstSpeed;
            KeepVelocityDistance = pParam->KeepVelocityDistance;
            OutOfControl = pParam->OutOfControl;

            fnd::GOComponent::BeginSetup(*this);

            if (auto* pVisualGoc = GetComponent<fnd::GOCVisualModel>())
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->Model;
                description.m_Skeleton = pInfo->Skeleton;

                pVisualGoc->Setup(description);

                if (auto* pAnimationGoc = GetComponent<game::GOCAnimationSimple>())
                {
                    pAnimationGoc->Setup({ ms_AnimationCount });
                    pAnimationGoc->Add(ms_pAnimationName, pInfo->Animation, game::PlayPolicy::Once);
                    pVisualGoc->AttachAnimation(pAnimationGoc);
                }
            }

            if (auto* pColliderGoc = GetComponent<game::GOCCollider>())
            {
                pColliderGoc->Setup({ 1 });

                game::ColliCapsuleShapeCInfo collisionInfo{};
                collisionInfo.m_Radius = ms_CollisionRadius;
                collisionInfo.m_Height = ms_CollisionHeight;
                collisionInfo.m_Unk2 |= 1;
                collisionInfo.m_Unk3 = 0x20000;

                collisionInfo.SetLocalPosition(ms_CollisionOffset);
                collisionInfo.SetLocalRotation(ms_CollisionRotation);

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, collisionInfo);
                pColliderGoc->CreateShape(collisionInfo);
            }

            if (pParam->IsEventOn && !GetExtUserData(eExtUserDataType_High))
            {
                GetComponent<fnd::GOCVisualModel>()->SetVisible(false);
                GetComponent<game::GOCCollider>()->SetEnable(false);
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
            case xgame::MsgHitEventCollision::MessageID:            return ProcMsgHitEventCollision(reinterpret_cast<xgame::MsgHitEventCollision&>(in_rMessage));
            case xgame::MsgPLGetHomingTargetInfo::MessageID:        return ProcMsgPLGetHomingTargetInfo(reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(in_rMessage));
            case xgame::MsgNotifyObjectEvent::MessageID:            return ProcMsgNotifyObjectEvent(reinterpret_cast<xgame::MsgNotifyObjectEvent&>(in_rMessage));
            default:                                                return CSetObjectListener::ProcessMessage(in_rMessage);
            }
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& in_rMessage)
        {
            int playerNo = ObjUtil::GetPlayerNo(*GetDocument(), in_rMessage.m_Sender);
            if (playerNo < 0)
                return false;

            GetComponent<game::GOCSound>()->Play3D(ms_pSoundName, 0.0f);
            GetComponent<game::GOCAnimationSimple>()->SetAnimation(ms_pAnimationName);

            csl::math::Vector3 playerPosition{};
            xgame::MsgGetPosition playerPosMsg{ playerPosition };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

            xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(), OutOfControl, KeepVelocityDistance / FirstSpeed };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& in_rMessage)
        {
            in_rMessage.m_CursorPosition = { GetComponent<fnd::GOCTransform>()->m_Frame.m_Unk3.m_Mtx * Vector4(0.0f, 3.5f, 0.0f, 1.0f) };

            return true;
        }

        bool ProcMsgNotifyObjectEvent(xgame::MsgNotifyObjectEvent& in_rMessage)
        {
            if (!in_rMessage.GetEventType())
                return false;

            GetComponent<fnd::GOCVisualModel>()->SetVisible(true);
            GetComponent<game::GOCCollider>()->SetEnable(true);

            SetExtUserData(eExtUserDataType_High, 1);

            return true;
        }

        csl::math::Vector3 GetDirectionVector()
        {
            return { csl::math::Vector3::UnitY() * FirstSpeed };
        }
    };
}