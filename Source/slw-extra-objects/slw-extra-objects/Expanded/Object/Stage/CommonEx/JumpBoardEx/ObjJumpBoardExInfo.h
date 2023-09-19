#pragma once

namespace slw_extra_objects
{
    class ObjJumpBoardExInfo : public app::CObjInfo
    {
    private:
        inline static const char* ms_pPackfileName = "CommonObjectEx.pac";
        inline static const char* ms_pModelNames[] = { "cmn_obj_jumpboard_15S", "cmn_obj_jumpboard_30S", "cmn_obj_jumpboard_30M", "cmn_obj_jumpboard_30L" };
        inline static const char* ms_pCollisionNames[] = { "cmn_obj_jumpboard_15S", "cmn_obj_jumpboard_30S", "cmn_obj_jumpboard_30M", "cmn_obj_jumpboard_30L" };
        inline static const char* ms_pBeltAnimationNames[] = { "cmn_obj_jumpboard_belt-0000", "cmn_obj_jumpboard_belt-0001" };
        inline static const char* ms_pTextureAnimatuionName = "cmn_obj_jumpboard_arrow";

    public:
        inline static const char* ms_pName = "ObjJumpBoardExInfo";

        inline static size_t ms_ModelCount = ARRAYSIZE(ms_pModelNames);
        inline static size_t ms_BeltAnimationCount = ARRAYSIZE(ms_pBeltAnimationNames);

        hh::gfx::res::ResModel Models[ARRAYSIZE(ms_pModelNames)]{};
        app::ResPhysicsMesh Collisions[ARRAYSIZE(ms_pModelNames)]{};
        hh::gfx::res::ResAnimTexSrt BeltTextureAnimations[ARRAYSIZE(ms_pBeltAnimationNames)]{};
        hh::gfx::res::ResAnimTexSrt ArrowTextureAnimation{};

    protected:
        void Initialize(app::GameDocument& in_rDocument) override;
        const char* GetInfoName() override;
    };
}