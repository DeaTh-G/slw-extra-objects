#include "pch.h"
#include "ObjWideSpringInfo.h"

void slw_extra_objects::ObjWideSpringInfo::Initialize(app::GameDocument& in_rDocument)
{
    auto packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);
    Model = app::ObjUtil::GetModelResource(ms_pModelName, packFile);
    Skeleton = app::ObjUtil::GetSkeletonResource(ms_pSkeletonName, packFile);
    Animation = app::ObjUtil::GetAnimationResource(ms_pAnimationName, packFile);
}

const char* slw_extra_objects::ObjWideSpringInfo::GetInfoName()
{
    return ms_pName;
}