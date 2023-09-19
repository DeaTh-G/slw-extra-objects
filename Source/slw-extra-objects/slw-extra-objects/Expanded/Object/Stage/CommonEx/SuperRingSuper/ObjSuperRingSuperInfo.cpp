#include "pch.h"
#include "ObjSuperRingSuperInfo.h"

void slw_extra_objects::ObjSuperRingSuperInfo::Initialize(app::GameDocument& in_rDocument)
{
    hh::ut::PackFile packFile = app::ObjUtil::GetPackFile(ms_pPackfileName);
    Model = app::ObjUtil::GetModelResource(ms_pModelName, packFile);
}

const char* slw_extra_objects::ObjSuperRingSuperInfo::GetInfoName()
{
    return ms_pName;
}