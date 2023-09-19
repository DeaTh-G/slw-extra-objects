#pragma once

namespace app::Player
{
    class SonicExInfo : public CObjInfo
    {
    private:
        inline static const char* ms_pBasePackfileName = "Sonic.pac";
        inline static const char* ms_pPackfileName = "SonicEx.pac";
        inline static const char* ms_pSkeletonName = "chr_Sonic";
        inline static const char* ms_pAnimationScriptName = "player_sonic_ex";

    public:
        inline static const char* ms_pName = "SonicExInfo";
        
        hh::gfx::res::ResSkeleton Skeleton {};
        animation::AnimationResContainer AnimationContainer{ *m_pAllocator };

        SonicExInfo() {}

    protected:
        void Load(CObjInfoFileLoader& loader) override
        {
            loader.Load(ms_pPackfileName, 0x4001);
        }

        void Initialize(GameDocument& document) override
        {
            hh::ut::PackFile packFile{ nullptr };

            packFile = ObjUtil::GetPackFile(ms_pBasePackfileName);
            Skeleton = ObjUtil::GetSkeletonResource(ms_pSkeletonName, packFile);

            packFile = ObjUtil::GetPackFile(ms_pPackfileName);
            auto animScript = ObjUtil::GetAnimationScriptResource(ms_pAnimationScriptName, packFile);

            animation::AnimationResContainer::LoadInfo loadInfo{ animScript, Skeleton, nullptr };
            AnimationContainer.LoadFromBuffer(loadInfo, packFile);
        }

        const char* GetInfoName() override
        {
            return ms_pName;
        }
    };
}
