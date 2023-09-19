#include "pch.h"
#include "ObjUpReelInfo.h"

void slw_extra_objects::ObjUpReelInfo::Initialize(app::GameDocument& in_rDocument)
{
    auto packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);

    for (size_t i = 0; i < ms_ModelCount; i++)
        Models[i] = app::ObjUtil::GetModelResource(ms_pModelNames[i], packFile);

    WireSkeleton = app::ObjUtil::GetSkeletonResource(ms_pWireSkeletonName, packFile);
}

const char* slw_extra_objects::ObjUpReelInfo::GetInfoName()
{
    return ms_pName;
}