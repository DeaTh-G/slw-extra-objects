#pragma once

namespace app
{
    class ObjUpReelInfo : public CObjInfo
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
        void Initialize(GameDocument& in_rDocument) override
        {
            auto packFile = ObjUtil::GetPackFile(ms_pPackfileName);

            for (size_t i = 0; i < ms_ModelCount; i++)
                Models[i] = ObjUtil::GetModelResource(ms_pModelNames[i], packFile);

            WireSkeleton = ObjUtil::GetSkeletonResource(ms_pWireSkeletonName, packFile);
        }

        const char* GetInfoName() override
        {
            return ms_pName;
        }
    };
}