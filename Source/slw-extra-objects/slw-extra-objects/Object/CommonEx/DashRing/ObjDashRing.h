#pragma once
#include "Patches\Functions.h"

namespace app
{
    class ObjDashRing : public CSetObjectListener
    {
    private:
        inline static const size_t ms_ShapeCount = 1;
        inline static const float ms_CollisionRadius = 12.0f;
        inline static const float ms_CollisionHeight = 1.0f;
        inline static const float ms_SquashEndTime = 0.7f;
        inline static const char* ms_SoundNames[] = { "obj_dashring", "obj_rainbowring" };

    protected:
        float FirstSpeed{};
        float OutOfControl{};
        float KeepVelocityDistance{};
        SDashRingParam::EType Type{};
        float ElapsedTime{};
        float DoSquash{};

    public:
        ObjDashRing() {}

        void AddCallback(GameDocument& in_rDocument) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjDashRingInfo>(in_rDocument);
            auto* pParam = GetAdapter()->GetData<SDashRingParam>();

            FirstSpeed = pParam->FirstSpeed;
            OutOfControl = pParam->OutOfControl;
            KeepVelocityDistance = pParam->KeepVelocityDistance;
            Type = pParam->Type;

            fnd::GOComponent::BeginSetup(*this);

            if (auto* pVisualGoc = GetComponent<fnd::GOCVisualModel>())
            {
                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->Models[Type];

                pVisualGoc->Setup(description);
                
                pVisualGoc->SetMaterialAnimation({ pInfo->MaterialAnimations[Type], 1 });

                auto* pBlender = pVisualGoc->SetTexSrtBlender({ 4 });
                for (size_t i = 0; i < 4; i++)
                    pBlender->CreateControl({ pInfo->TextureAnimations[Type * 4 + i], 1 });
            }

            if (auto* pColliderGoc = GetComponent<game::GOCCollider>())
            {
                pColliderGoc->Setup({ ms_ShapeCount });

                game::ColliCylinderShapeCInfo collisionInfo{};
                collisionInfo.m_Radius = ms_CollisionRadius;
                collisionInfo.m_Height = ms_CollisionHeight;
                collisionInfo.m_Unk2 |= 1;

                collisionInfo.SetLocalRotation({ Eigen::AngleAxisf(MATHF_PI / 2.0f, csl::math::Vector3::UnitX()) });

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, collisionInfo);
                pColliderGoc->CreateShape(collisionInfo);
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
            GetComponent<fnd::GOCVisualModel>()->SetLocalScale({ scale, scale, 1.0f });
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& in_rMessage)
        {
            int playerNo = ObjUtil::GetPlayerNo(*GetDocument(), in_rMessage.m_Sender);
            if (playerNo < 0)
                return false;

            GetComponent<game::GOCSound>()->Play3D(ms_SoundNames[Type], {}, 0);
            
            DoSquash = true;

            xgame::MsgSpringImpulse impulseMsg{ GetComponent<fnd::GOCTransform>()->GetLocalPosition(), GetDirectionVector(), OutOfControl, KeepVelocityDistance / FirstSpeed };
            impulseMsg.Flags.set(18);
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);
            
            return true;
        }

        csl::math::Vector3 GetDirectionVector()
        {
            return static_cast<csl::math::Vector3>(GetComponent<fnd::GOCTransform>()->GetLocalRotation() * csl::math::Vector3::UnitZ() * FirstSpeed);
        }
    };
}