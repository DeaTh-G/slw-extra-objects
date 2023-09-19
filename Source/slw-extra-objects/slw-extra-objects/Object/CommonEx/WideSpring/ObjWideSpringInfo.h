#pragma once

namespace app
{
    class ObjWideSpringInfo : public CObjInfo
    {
    private:
        inline static const char* ms_pPackfileName = "CommonObjectEx.pac";
        inline static const char* ms_pModelName = "cmn_obj_widespring";
        inline static const char* ms_pSkeletonName = "cmn_obj_widespring";
        inline static const char* ms_pAnimationName = "cmn_obj_widespring";

    public:
        inline static const char* ms_pName = "ObjWideSpringInfo";
        
        hh::gfx::res::ResModel Model{};
        hh::gfx::res::ResSkeleton Skeleton{};
        hh::gfx::res::ResAnimSkeleton Animation{};

    protected:
        void Initialize(GameDocument& document) override
        {
            auto packFile = ObjUtil::GetPackFile(ms_pPackfileName);
            Model = ObjUtil::GetModelResource(ms_pModelName, packFile);
            Skeleton = ObjUtil::GetSkeletonResource(ms_pSkeletonName, packFile);
            Animation = ObjUtil::GetAnimationResource(ms_pAnimationName, packFile);
        }

        const char* GetInfoName() override
        {
            return ms_pName;
        }
    };
}