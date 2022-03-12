#pragma once

namespace app
{
    class ObjJumpBoardEx : public CSetObjectListener
    {
    public:
        ObjJumpBoardEx() {}

        void AddCallback(GameDocument& document) override
        {
            fnd::GOComponent::Create<fnd::GOCVisualModel>(*this);

            auto* pInfo = ObjUtil::GetObjectInfo<ObjJumpBoardExInfo>(document, "ObjJumpBoardExInfo");
            auto* pParam = reinterpret_cast<SJumpBoardExParam*>(m_pAdapter->GetData());

            fnd::GOComponent::BeginSetup(*this);

            auto* pVisual = GetComponent<fnd::GOCVisualModel>();
            if (pVisual)
            {
                auto type = static_cast<char>(pParam->m_Type);

                fnd::GOCVisualModel::Description description{};
                description.m_Model = pInfo->m_Models[type];

                pVisual->Setup(description);
            }        

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
    };
}