#pragma once

namespace app
{
    class ObjJumpPanelInfo : public CObjInfo
    {
    private:
        inline static const char* ms_pPackfileName = "CommonObjectEx.pac";
        inline static const char* ms_pModelNames[] = { "cmn_obj_jumppanel", "cmn_obj_jumppanelU" };
        inline static const char* ms_pTextureAnimationNames[] = { "cmn_obj_jumppanel_belt-0000", "cmn_obj_jumppanel_belt-0001", "cmn_obj_jumppanel_belt-0002" };

    public:
        inline static const char* ms_pName = "ObjJumpPanelInfo";

        inline static size_t ms_ModelCount = ARRAYSIZE(ms_pModelNames);
        inline static size_t ms_TextureAnimationCount = ARRAYSIZE(ms_pTextureAnimationNames);

        hh::gfx::res::ResModel Models[ARRAYSIZE(ms_pModelNames)]{};
        hh::gfx::res::ResAnimTexSrt TextureAnimations[ARRAYSIZE(ms_pTextureAnimationNames)]{};

    protected:
        void Initialize(GameDocument& in_rDocument) override
        {
            auto packFile = ObjUtil::GetPackFile(ms_pPackfileName);

            for (size_t i = 0; i < ms_ModelCount; i++)
                Models[i] = ObjUtil::GetModelResource(ms_pModelNames[i], packFile);

            for (size_t i = 0; i < ms_TextureAnimationCount; i++)
                TextureAnimations[i] = ObjUtil::GetTexSrtAnimationResource(ms_pTextureAnimationNames[i], packFile);
        }

        const char* GetInfoName() override
        {
            return ms_pName;
        }
    };
}