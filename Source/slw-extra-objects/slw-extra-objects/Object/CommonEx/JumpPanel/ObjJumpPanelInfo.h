#pragma once

namespace app
{
    class ObjJumpPanelInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_jumppanel", "cmn_obj_jumppanelU" };
        inline static const char* ms_AnimationNames[] = { "cmn_obj_jumppanel_belt-0000", "cmn_obj_jumppanel_belt-0001", "cmn_obj_jumppanel_belt-0002" };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);
        inline static size_t ms_AnimCount = ARRAYSIZE(ms_AnimationNames);

        hh::gfx::res::ResModel m_Models[ARRAYSIZE(ms_ModelNames)]{};
        hh::gfx::res::ResAnimTexSrt m_TextureAnimations[ARRAYSIZE(ms_ModelNames)]{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");

            for (size_t i = 0; i < ms_ModelCount; i++)
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);

            for (size_t i = 0; i < ms_AnimCount; i++)
                m_TextureAnimations[i] = ObjUtil::GetTexSrtAnimationResource(ms_AnimationNames[i], packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjJumpPanelInfo";
        }
    };
}