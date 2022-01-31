#include "pch.h"

HOOK(app::CSetObjectFactory*, __fastcall, h_CSetObjectFactoryCtor, ASLR(0x008443D0), app::CSetObjectFactory* This, void* edx)
{
	originalh_CSetObjectFactoryCtor(This, edx);
	This->m_ObjClasses.push_back(&SUPER_RING_SUPER_CLASS);

	return This;
}

HOOK(void, __stdcall, h_CSetObjectFactoryCreateParamMap, ASLR(0x00843FD0), app::ut::RefPtr<app::SetEd::CClassProject>& pProject)
{
	originalh_CSetObjectFactoryCreateParamMap(pProject);
	pProject->AddClass(SUPER_RING_SUPER_CLASS.m_Name, SUPER_RING_SUPER_CLASS.m_CategoryName,
		SUPER_RING_SUPER_CLASS.m_RangeIn, SUPER_RING_SUPER_CLASS.m_RangeOut, 0, 0);
}

void app::CSetObjectFactory::InstallPatches()
{
	INSTALL_HOOK(h_CSetObjectFactoryCtor);
	INSTALL_HOOK(h_CSetObjectFactoryCreateParamMap);
};