#pragma once

namespace app
{
    class ObjJumpBoardExInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_jumppanelM_15S", "cmn_obj_jumppanelM_30S", "cmn_obj_jumppanelM_30M", "cmn_obj_jumppanelM_30L" };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);

        hh::gfx::res::ResModel m_Models[ARRAYSIZE(ms_ModelNames)]{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");
            for (size_t i = 0; i < ms_ModelCount; i++)
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjJumpBoardExInfo";
        }
    };
}