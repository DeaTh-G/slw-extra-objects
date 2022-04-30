#pragma once

namespace app::Player
{
    class SonicExInfo : public CObjInfo
    {
    public:
        hh::gfx::res::ResSkeleton m_Skeleton {};
        animation::AnimationResContainer m_AnimContainer{ *m_pAllocator };

        SonicExInfo() {}

    protected:
        void Load(CObjInfoFileLoader& loader) override
        {
            loader.Load("SonicEx.pac", 0x4001);
        }

        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile{ nullptr };

            packFile = ObjUtil::GetPackFile("Sonic.pac");
            m_Skeleton = ObjUtil::GetSkeletonResource("chr_Sonic", packFile);

            packFile = ObjUtil::GetPackFile("SonicEx.pac");
            auto animScript = ObjUtil::GetAnimationScriptResource("player_sonic_ex", packFile);

            animation::AnimationResContainer::LoadInfo loadInfo{ animScript, m_Skeleton, nullptr };
            m_AnimContainer.LoadFromBuffer(loadInfo, packFile);
        }

        const char* GetInfoName() override
        {
            return "SonicExInfo";
        }
    };
}
