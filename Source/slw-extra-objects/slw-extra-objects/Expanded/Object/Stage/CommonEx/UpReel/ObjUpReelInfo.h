#pragma once

namespace slw_extra_objects
{
    class ObjUpReelInfo : public app::CObjInfo
    {
    private:
        inline static const char* ms_pPackfileName = "CommonObjectEx.pac";
        inline static const char* ms_pModelNames[] = { "cmn_obj_upreel_stand", "cmn_obj_upreel_handle", "cmn_obj_upreel_wire" };
        inline static const char* ms_pWireSkeletonName = "cmn_obj_upreel_wire";

    public:
        inline static const char* ms_pName = "ObjUpReelInfo";

        inline static size_t ms_ModelCount = ARRAYSIZE(ms_pModelNames);

        hh::gfx::res::ResModel Models[ARRAYSIZE(ms_pModelNames)]{};
        hh::gfx::res::ResSkeleton WireSkeleton{};

    protected:
        void Initialize(app::GameDocument& in_rDocument) override;
        const char* GetInfoName() override;
    };
}