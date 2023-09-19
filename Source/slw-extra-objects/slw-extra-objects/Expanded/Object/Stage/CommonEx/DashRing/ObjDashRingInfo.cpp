#include "pch.h"
#include "ObjDashRingInfo.h"

void slw_extra_objects::ObjDashRingInfo::Initialize(app::GameDocument& in_rDocument)
{
    auto packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);

    for (size_t i = 0; i < ms_ModelCount; i++)
    {
        Models[i] = app::ObjUtil::GetModelResource(ms_ModelNames[i], packFile);
        MaterialAnimations[i] = app::ObjUtil::GetMaterialAnimationResource(ms_ModelNames[i], packFile);
    }

    for (size_t i = 0; i < ms_AnimCount; i++)
        TextureAnimations[i] = app::ObjUtil::GetTexSrtAnimationResource(ms_AnimNames[i], packFile);
}

const char* slw_extra_objects::ObjDashRingInfo::GetInfoName()
{
	return ms_pName;
}