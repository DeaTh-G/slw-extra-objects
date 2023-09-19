#pragma once
#include "Patches\Functions.h"

using namespace csl::math;

namespace app
{
    class ObjJumpPanel : public CSetObjectListener
    {
    private:
        inline static const size_t ms_TextureAnimationCount = 3;
        inline static const Vector3 ms_CollisionSizes[] = { { 8.6f, 0.85f, 19.6f }, { 8.6f, 0.85f, 5.5f }, { 8.6f, 0.85f, 15.2f } };
        inline static const Vector3 ms_CollisionOffsets[] = { { 0.0f, 0.0f, 19.0f }, { 0.0f, 1.4f, 4.8f }, { 0.0f, 9.2f, 23.755f } };
        inline static const float ms_UpRotations[] = { -15.0f, -25.0f };
        inline static const float ms_LaunchOffsets[] = { -5.0f, -30.0f };
        inline static const char* ms_pSoundName = "obj_dashpanel";

    protected:
        float FirstSpeed{};
        float KeepVelocityDistance{};
        float OutOfControl{};
        float Pitch{};
        CSetObjectID TargetID{};
        SJumpPanelParam::EType Type{};
        bool IsOn{ true };
        float ElapsedTime{};

    public:
        ObjJumpPanel() {}

        void AddCallback(GameDocument& in_rDocument) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjJumpPanelInfo>(in_rDocument);
            auto* pParam = GetAdapter()->GetData<SJumpPanelParam>();

            FirstSpeed = pParam->FirstSpeed;
            KeepVelocityDistance = pParam->KeepVelocityDistance;
            OutOfControl = pParam->OutOfControl;
            Pitch = pParam->Pitch;
            TargetID = pParam->TargetID;
            Type = pParam->Type;

            fnd::GOComponent::BeginSetup(*this);

            if (auto* pVisualGoc = GetComponent<fnd::GOCVisualModel>())
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->Models[Type];

                pVisualGoc->Setup(description);

                auto* pBlender = pVisualGoc->SetTexSrtBlender({ ms_TextureAnimationCount });

                for (size_t i = 0; i < ms_TextureAnimationCount; i++)
                    pBlender->CreateControl({ pInfo->TextureAnimations[i], 1 });
            }

            if (auto* pColliderGoc = GetComponent<game::GOCCollider>())
            {
                pColliderGoc->Setup({ Type + 1 });

                for (size_t i = 0; i < pColliderGoc->m_Shapes.capacity(); i++)
                {
                    game::ColliBoxShapeCInfo collisionInfo{};
                    collisionInfo.m_Size = ms_CollisionSizes[Type + i];
                    collisionInfo.m_Unk2 |= 1;

                    collisionInfo.SetLocalPosition(ms_CollisionOffsets[Type + i]);

                    if (Type == SJumpPanelParam::eType_Upwards)
                        collisionInfo.SetLocalRotation(Eigen::AngleAxisf(TO_RAD(ms_UpRotations[i]), csl::math::Vector3::UnitX()));

                    ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, collisionInfo);
                    pColliderGoc->CreateShape(collisionInfo);
                }
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
                GetComponent<game::GOCCollider>()->SetEnable(true);
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

            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, in_rMessage.m_Sender);
            if (playerNo < 0)
                return false;

            GetComponent<game::GOCSound>()->Play3D(ms_pSoundName, 0.0f);
            GetComponent<game::GOCCollider>()->SetEnable(false);

            IsOn = false;

            csl::math::Vector3 playerPosition{};
            xgame::MsgGetPosition playerPosMsg{ playerPosition };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, playerPosMsg);

            xgame::MsgSpringImpulse impulseMsg{ playerPosMsg.GetPosition(), GetDirectionVector(), OutOfControl, KeepVelocityDistance / FirstSpeed };
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);

            return true;
        }

        csl::math::Vector3 GetDirectionVector()
        {
            auto* pTransformGoc = GetComponent<fnd::GOCTransform>();
            csl::math::Vector3 direction{};

            if (!TargetID.Value)
            {
                auto launchAngle = pTransformGoc->GetLocalRotation() * GetLaunchOffset() * GetPitchCorrection();
                return { launchAngle * csl::math::Vector3::UnitZ() * FirstSpeed };
            }
            else
            {
                csl::math::Vector3 targetPosition{};
                ObjUtil::GetSetObjectTransform(*m_pOwnerDocument, TargetID, &targetPosition, nullptr);
                
                csl::math::Vector3 direction = { targetPosition - pTransformGoc->GetLocalPosition() };
                direction.normalize();

                return { direction * FirstSpeed };
            }
        }

        csl::math::Quaternion GetLaunchOffset()
        {
            return { Eigen::AngleAxisf(TO_RAD(ms_LaunchOffsets[Type]), csl::math::Vector3::UnitX()) };
        }

        csl::math::Quaternion GetPitchCorrection()
        {
            return { Eigen::AngleAxisf(TO_RAD(Pitch), csl::math::Vector3::UnitY()) };
        }
    };
}