#pragma once

namespace slw_extra_objects::Player
{
    class SonicExInfo : public app::CObjInfo
    {
    private:
        inline static const char* ms_pBasePackfileName = "Sonic.pac";
        inline static const char* ms_pPackfileName = "SonicEx.pac";
        inline static const char* ms_pSkeletonName = "chr_Sonic";
        inline static const char* ms_pAnimationScriptName = "player_sonic_ex";

    public:
        inline static const char* ms_pName = "SonicExInfo";
        
        hh::gfx::res::ResSkeleton Skeleton {};
        app::animation::AnimationResContainer AnimationContainer{ *m_pAllocator };

        SonicExInfo();

    protected:
        void Load(app::CObjInfoFileLoader& loader) override;
        void Initialize(app::GameDocument& document) override;
        const char* GetInfoName() override;
    };
}