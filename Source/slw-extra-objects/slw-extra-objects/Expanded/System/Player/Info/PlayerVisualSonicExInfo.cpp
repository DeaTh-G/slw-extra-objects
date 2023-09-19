#include "pch.h"
#include "PlayerVisualSonicExInfo.h"

slw_extra_objects::Player::SonicExInfo::SonicExInfo() : app::CObjInfo()
{

}

void slw_extra_objects::Player::SonicExInfo::Load(app::CObjInfoFileLoader& loader)
{
    loader.Load(ms_pPackfileName, 0x4001);
}

void slw_extra_objects::Player::SonicExInfo::Initialize(app::GameDocument& document)
{
    hh::ut::PackFile packFile{ nullptr };

    packFile = app::ObjUtil::GetPackFile(ms_pBasePackfileName);
    Skeleton = app::ObjUtil::GetSkeletonResource(ms_pSkeletonName, packFile);

    packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);
    auto animScript = app::ObjUtil::GetAnimationScriptResource(ms_pAnimationScriptName, packFile);

    app::animation::AnimationResContainer::LoadInfo loadInfo{ animScript, Skeleton, nullptr };
    AnimationContainer.LoadFromBuffer(loadInfo, packFile);
}

const char* slw_extra_objects::Player::SonicExInfo::GetInfoName()
{
    return ms_pName;
}
