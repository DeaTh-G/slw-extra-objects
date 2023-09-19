#pragma once

namespace slw_extra_objects
{
    class ObjWideSpringInfo : public app::CObjInfo
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
        void Initialize(app::GameDocument& document) override;
        const char* GetInfoName() override;
    };
}