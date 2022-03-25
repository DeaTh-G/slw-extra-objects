#pragma once

namespace app
{
    class ObjUpReel : public CSetObjectListener
    {
    protected:
        float m_Length{};

    public:
        ObjUpReel() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualContainer>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjUpReelInfo>(document, "ObjUpReelInfo");
            auto* pParam = reinterpret_cast<SUpReelParam*>(m_pAdapter->GetData());

            m_Length = pParam->m_Length;

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisualContainer = GetComponent<fnd::GOCVisualContainer>();
            if (pVisualContainer)
            {
                pVisualContainer->Setup({ ObjUpReelInfo::ms_ModelCount });

                for (size_t i = 0; i < ObjUpReelInfo::ms_ModelCount; i++)
                {
                    auto* pVisual = pVisualContainer->CreateSingle<fnd::GOCVisualModel>(*this);
                    if (!pVisual)
                        continue;

                    fnd::GOCVisualModel::Description description{};
                    description.m_Model = pInfo->m_Models[i];
                    if (i == 2)
                        description.m_Skeleton = pInfo->m_WireSkeleton;

                    pVisual->Setup(description);

                    /*if (i == 1)
                        pVisual->SetLocalScale({ 0.9f, 0.9f, 0.9f });*/

                    pVisualContainer->Add(pVisual);
                }
                
                SetWireLength();
                SetHandleDistance();
            }

            auto* pCollider = GetComponent<game::GOCCollider>();
            if (pCollider)
            {
                pCollider->Setup({ 1 });

                game::ColliSphereShapeCInfo colliInfo{};
                colliInfo.m_Radius = 6.0f;
                colliInfo.m_Unk2 |= 1;
                colliInfo.m_Unk3 = 0x20000;

                colliInfo.SetLocalPosition({ 0.0f, -pParam->m_Length - 4.0f, 0.0f });

                ObjUtil::SetupCollisionFilter(ObjUtil::eFilter_Default, colliInfo);
                pCollider->CreateShape(colliInfo);
            }

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
            case xgame::MsgPLGetHomingTargetInfo::MessageID:
                return ProcMsgPLGetHomingTargetInfo(reinterpret_cast<xgame::MsgPLGetHomingTargetInfo&>(msg));
            case xgame::MsgGetExternalMovePosition::MessageID:
                return ProcMsgGetExternalMovePosition(reinterpret_cast<xgame::MsgGetExternalMovePosition&>(msg));
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
        }

    private:
        bool ProcMsgGetExternalMovePosition(xgame::MsgGetExternalMovePosition& message)
        {
            auto* pHandleModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1]);
            if (!pHandleModel)
                return false;

            *message.m_pTransform = pHandleModel->m_Impl.m_Transform;
            Quaternion rotation(pHandleModel->m_Impl.m_Transform);
            rotation = rotation * Eigen::AngleAxisf(TO_RAD(180), Vector3::UnitY());
            
            for (size_t i = 0; i < 3; i++)
                message.m_pTransform->SetColumn(i, static_cast<Vector3>(rotation.toRotationMatrix().row(i)));

            message.m_pTransform->GetTransVector().y() += -10.5f;
        }

        bool ProcMsgHitEventCollision(xgame::MsgHitEventCollision& msg)
        {
            int playerNo = ObjUtil::GetPlayerNo(*m_pOwnerDocument, msg.m_Sender);
            if (playerNo < 0)
                return false;

            xgame::MsgCatchPlayer catchMessage{};
            catchMessage.m_Unk3 = 13;
            ObjUtil::SendMessageImmToPlayer(*this, playerNo, catchMessage);

            return true;
        }

        bool ProcMsgPLGetHomingTargetInfo(xgame::MsgPLGetHomingTargetInfo& msg)
        {
            auto cursorPos = GetComponent<fnd::GOCTransform>()->m_Frame.m_Unk1.m_Mtx * Vector4(0, -m_Length - 4.0f, 0, 1);
            msg.m_CursorPosition = Vector3(cursorPos.x(), cursorPos.y(), cursorPos.z());

            return true;
        }

        void SetWireLength()
        {
            auto* pWireModel = reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[2]);
            if (!pWireModel)
                return;

            Transform transform{};

            pWireModel->GetNodeTransform(2, "Cos_top", &transform);
            transform.m_Position.y() += -m_Length + 4.0f;
            pWireModel->SetNodeTransform(2, "Cos_top", transform);
            
            pWireModel->m_Impl.m_pModelNode->m_Bounds.m_Max.y() += -m_Length + 4.0f;
        }

        void SetHandleDistance()
        {
            reinterpret_cast<fnd::GOCVisualModel*>(GetComponent<fnd::GOCVisualContainer>()->m_Visuals[1])->SetLocalTranslation({ 0, -m_Length - 4.0f, 0 });
        }
    };
}