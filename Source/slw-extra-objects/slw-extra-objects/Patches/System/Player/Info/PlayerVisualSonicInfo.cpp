#include "pch.h"

HOOK(void, __fastcall, InitializeHook, ASLR(0x008F8480), app::Player::SonicInfo* in_pThis, void* edx, app::GameDocument& in_rDocument)
{
	originalInitializeHook(in_pThis, edx, in_rDocument);

	auto* pInfo = app::ObjUtil::GetObjectInfo<slw_extra_objects::Player::SonicExInfo>(in_rDocument);
	if (!pInfo)
		return;
	
	pInfo->AnimationContainer.MergeTransitions(in_pThis->AnimationContainer);
}

void slw_extra_objects::Player::SonicInfo::InstallHooks()
{
	INSTALL_HOOK(InitializeHook);
}