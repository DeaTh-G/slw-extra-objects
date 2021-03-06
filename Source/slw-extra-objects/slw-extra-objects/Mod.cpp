#include "pch.h"
#include "LostCodeLoader.h"
#include "Patches\Patches.h"

extern "C"
{
	void _declspec(dllexport) __cdecl Init(ModInfo* modInfo)
	{
		MessageBoxA(nullptr, "", "", 0);
		SLW_EXTRA_OBJECTS::CSetObjectFactory::InstallPatches();
		SLW_EXTRA_OBJECTS::StageDataInitializer::InstallPatches();
		SLW_EXTRA_OBJECTS::CStateBase::InstallPatches();
		SLW_EXTRA_OBJECTS::Player::SonicInfo::InstallPatches();
	}
}