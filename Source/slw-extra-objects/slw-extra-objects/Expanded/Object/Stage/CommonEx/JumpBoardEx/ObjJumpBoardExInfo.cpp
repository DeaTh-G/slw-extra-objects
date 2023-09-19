#include "pch.h"
#include "ObjJumpBoardExInfo.h"

void slw_extra_objects::ObjJumpBoardExInfo::Initialize(app::GameDocument& in_rDocument)
{
    auto packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);
    for (size_t i = 0; i < ms_ModelCount; i++)
    {
        Models[i] = app::ObjUtil::GetModelResource(ms_pModelNames[i], packFile);
        Collisions[i] = app::ObjUtil::GetPhysicsMeshResource(ms_pCollisionNames[i], packFile);
    }

    for (size_t i = 0; i < ms_BeltAnimationCount; i++)
        BeltTextureAnimations[i] = app::ObjUtil::GetTexSrtAnimationResource(ms_pBeltAnimationNames[i], packFile);

    ArrowTextureAnimation = app::ObjUtil::GetTexSrtAnimationResource(ms_pTextureAnimatuionName, packFile);
}

const char* slw_extra_objects::ObjJumpBoardExInfo::GetInfoName()
{
    return ms_pName;
}