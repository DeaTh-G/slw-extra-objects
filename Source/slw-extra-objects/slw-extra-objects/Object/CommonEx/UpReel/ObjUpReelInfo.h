#pragma once

namespace app
{
    class ObjUpReelInfo : public CObjInfo
    {
    private:
        inline static const char* ms_ModelNames[] = { "cmn_obj_upreel_stand", "cmn_obj_upreel_handle", "cmn_obj_upreel_wire" };

    public:
        inline static size_t ms_ModelCount = ARRAYSIZE(ms_ModelNames);

        hh::gfx::res::ResModel m_Models[ARRAYSIZE(ms_ModelNames)]{};
        hh::gfx::res::ResSkeleton m_WireSkeleton{};

    protected:
        void Initialize(GameDocument& document) override
        {
            auto packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");

            for (size_t i = 0; i < ms_ModelCount; i++)
                m_Models[i] = ObjUtil::GetModelResource(ms_ModelNames[i], packFile);

            m_WireSkeleton = ObjUtil::GetSkeletonResource("cmn_obj_upreel_wire", packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjUpReelInfo";
        }
    };
}