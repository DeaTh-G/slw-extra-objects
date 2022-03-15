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

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                size_t modelAnimCount = ObjDashRingInfo::ms_AnimCount / ObjDashRingInfo::ms_ModelCount;

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[pParam->m_Type];

                pVisual->Setup(description);
                
                pVisual->SetMaterialAnimation({ pInfo->m_MaterialAnimations[pParam->m_Type], 1 });

                auto pBlender = pVisual->SetTexSrtBlender({ modelAnimCount });
                for (size_t i = 0; i < modelAnimCount; i++)
                    pBlender->CreateControl({ pInfo->m_TextureAnimations[pParam->m_Type * 4 + i], 1 });
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliCylinderShapeCInfo colliInfo{};
                colliInfo.m_Radius = 12.0f;
                colliInfo.m_Height = 1.0f;
                colliInfo.m_Unk2 |= 1;

                colliInfo.SetLocalRotation(Eigen::Quaternionf(Eigen::AngleAxisf(MATHF_PI / 2, Eigen::Vector3f::UnitX())));

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

            auto* pSound = GetComponent<game::GOCSound>();
            auto* pTransform = GetComponent<fnd::GOCTransform>();

            auto* pParam = reinterpret_cast<SDashRingParam*>(m_pAdapter->GetData());
            float speed = pParam->m_KeepVelocityDistance / pParam->m_FirstSpeed;

            int deviceTag[3];
            SLW_EXTRA_OBJECTS::GOCSound::Play3D(pSound, deviceTag, ms_SoundNames[pParam->m_Type], 0);

            Vector3 direction{ pTransform->GetLocalRotation() * Eigen::Vector3f::UnitZ() * pParam->m_FirstSpeed };
            xgame::MsgSpringImpulse impulseMsg{ pTransform->GetLocalPosition(), direction, pParam->m_OutOfControl, speed };
            impulseMsg.field_50.set(18);
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, impulseMsg);
            
            m_DoSquash = true;
            return true;
        }
    };
}