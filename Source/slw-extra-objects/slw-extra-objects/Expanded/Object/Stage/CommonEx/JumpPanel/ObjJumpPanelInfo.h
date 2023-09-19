#pragma once

namespace slw_extra_objects
{
    class ObjJumpPanelInfo : public app::CObjInfo
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
        void Initialize(app::GameDocument& in_rDocument) override;
        const char* GetInfoName() override;
    };
}