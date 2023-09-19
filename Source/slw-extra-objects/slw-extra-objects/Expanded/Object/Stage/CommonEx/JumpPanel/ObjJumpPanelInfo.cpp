#include "pch.h"
#include "ObjJumpPanelInfo.h"

void slw_extra_objects::ObjJumpPanelInfo::Initialize(app::GameDocument& in_rDocument)
{
    auto packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);

    for (size_t i = 0; i < ms_ModelCount; i++)
        Models[i] = app::ObjUtil::GetModelResource(ms_pModelNames[i], packFile);

    for (size_t i = 0; i < ms_TextureAnimationCount; i++)
        TextureAnimations[i] = app::ObjUtil::GetTexSrtAnimationResource(ms_pTextureAnimationNames[i], packFile);
}

const char* slw_extra_objects::ObjJumpPanelInfo::GetInfoName()
{
    return ms_pName;
}