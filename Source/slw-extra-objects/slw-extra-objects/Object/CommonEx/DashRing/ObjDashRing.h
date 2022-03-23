#pragma once
#include "Patches\Functions.h"

using namespace csl::math;

namespace app
{
    class ObjDashRing : public CSetObjectListener
    {
    private:
        inline static const char* ms_SoundNames[] = { "obj_dashring", "obj_rainbowring" };

    protected:
        float m_FirstSpeed{};
        float m_OutOfControl{};
        float m_KeepVelocityDistance{};
        SDashRingParam::EType m_Type{};
        float m_Time{};
        float m_DoSquash{};

    public:
        ObjDashRing() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);
            fnd::GOComponent::Create<game::GOCSound>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjDashRingInfo>(document, "ObjDashRingInfo");
            auto* pParam = reinterpret_cast<SDashRingParam*>(m_pAdapter->GetData());

            m_FirstSpeed = pParam->m_FirstSpeed;
            m_OutOfControl = pParam->m_OutOfControl;
            m_KeepVelocityDistance = pParam->m_KeepVelocityDistance;
            m_Type = pParam->m_Type;

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                size_t modelAnimCount = ObjDashRingInfo::ms_AnimCount / ObjDashRingInfo::ms_ModelCount;

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[m_Type];

                pVisual->Setup(description);
                
                pVisual->SetMaterialAnimation({ pInfo->m_MaterialAnimations[m_Type], 1 });

                auto pBlender = pVisual->SetTexSrtBlender({ modelAnimCount });
                for (size_t i = 0; i < modelAnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TextureAnimations[m_Type * 4 + i], 1 });
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliCylinderShapeCInfo colliInfo{};
                colliInfo.m_Radius = 12.0f;
                colliInfo.m_Height = 1.0f;
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalRotation(Eigen::AngleAxisf(MATHF_PI / 2, Vector3::UnitX()));

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Unk12, colliInfo);
                pCollider->CreateShape(colliInfo);
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
            if (!m_DoSquash)
                return;

            m_Time += rInfo.deltaTime;
            if (m_Time >= 0.7f)
            {
                m_DoSquash = false;
                m_Time = 0.0f;
                return;
            }

            float scale = cos(m_Time * 1.4285715f * MATHF_PI * 2.0f);
            GetComponent<fnd::GOCVisualModel>()->SetLocalScale({ scale, scale, 1 });
        }

    private:
        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            GetComponent<game::GOCSound>()->Play3D(ms_SoundNames[m_Type], {}, 0);
            
            m_DoSquash = true;

            xgame::MsgSpringImpulse impulseMsg{ GetComponent<fnd::GOCTransform>()->GetLocalPosition(), GetDirectionVector(), m_OutOfControl, m_KeepVelocityDistance / m_FirstSpeed };
            impulseMsg.field_50.set(18);
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);
            
            return true;
        }

        Vector3 GetDirectionVector()
        {
            return static_cast<Vector3>(GetComponent<fnd::GOCTransform>()->GetLocalRotation() * Vector3::UnitZ() * m_FirstSpeed);
        }
    };
}