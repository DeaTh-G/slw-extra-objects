#pragma once

namespace app
{
    class ObjJumpBoardEx : public CSetObjectListener
    {
    private:
        inline static Vector3 ms_JumpBoardSizes[] = { { 6.0f, 3.0f, 7.5f }, { 6.0f, 3.0f, 1.0f } };
        inline static Vector3 ms_JumpBoardPositions[] = { { 0.0f, -1.12f, 7.567f }, { 0.0f, 0.719f, 15.035f } };

    public:
        ObjJumpBoardEx() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);
            fnd::GOComponent::Create<game::GOCCollider>(*this);

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

            //SetupCollider(pParam->m_Type);

            fnd::GOComponent::EndSetup(*this);
        }

        bool ProcessMessage(fnd::Message& msg) override
        {
            if (PreProcessMessage(msg))
                return true;

            switch (msg.GetType())
            {
            default:
                return CSetObjectListener::ProcessMessage(msg);
            }
        }

    private:
        void SetupCollider(SJumpBoardExParam::EType type)
        {
            auto* pCollider = GetComponent<game::GOCCollider>();
            if (!pCollider)
                return;

            game::GOCCollider::Description description{ 1 };
            pCollider->Setup(description);
        }
    };
}