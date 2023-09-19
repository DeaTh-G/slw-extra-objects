#pragma once

namespace app
{
    class ObjJumpBoardExInfo : public CObjInfo
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
        ResPhysicsMesh Collisions[ARRAYSIZE(ms_pModelNames)]{};
        hh::gfx::res::ResAnimTexSrt BeltTextureAnimations[ARRAYSIZE(ms_pBeltAnimationNames)]{};
        hh::gfx::res::ResAnimTexSrt ArrowTextureAnimation{};

    protected:
        void Initialize(GameDocument& in_rDocument) override
        {
            auto packFile = ObjUtil::GetPackFile(ms_pPackfileName);
            for (size_t i = 0; i < ms_ModelCount; i++)
            {
                Models[i] = ObjUtil::GetModelResource(ms_pModelNames[i], packFile);
                Collisions[i] = ObjUtil::GetPhysicsMeshResource(ms_pCollisionNames[i], packFile);
            }

            for (size_t i = 0; i < ms_BeltAnimationCount; i++)
                BeltTextureAnimations[i] = ObjUtil::GetTexSrtAnimationResource(ms_pBeltAnimationNames[i], packFile);

            ArrowTextureAnimation = ObjUtil::GetTexSrtAnimationResource(ms_pTextureAnimatuionName, packFile);
        }

        const char* GetInfoName() override
        {
            return ms_pName;
        }
    };
}