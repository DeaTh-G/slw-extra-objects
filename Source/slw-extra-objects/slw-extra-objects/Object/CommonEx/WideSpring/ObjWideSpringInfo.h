#pragma once

namespace app
{
    class ObjWideSpringInfo : public CObjInfo
    {
    public:
        hh::gfx::res::ResModel m_Model{};
        hh::gfx::res::ResSkeleton m_Skeleton{};
        hh::gfx::res::ResAnimSkeleton m_Animation{};

    protected:
        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile = ObjUtil::GetPackFile("CommonObjectEx.pac");
            m_Model = ObjUtil::GetModelResource("cmn_obj_widespring", packFile);
            m_Skeleton = ObjUtil::GetSkeletonResource("cmn_obj_widespring", packFile);
            m_Animation = ObjUtil::GetAnimationResource("cmn_obj_widespring", packFile);
        }

        const char* GetInfoName() override
        {
            return "ObjWideSpringInfo";
        }
    };
}