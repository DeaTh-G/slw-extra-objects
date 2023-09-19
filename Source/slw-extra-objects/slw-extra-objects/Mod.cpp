#include "pch.h"
#include "ModLoader.h"

extern "C"
{
	void _declspec(dllexport) __cdecl Init(ModInfo_t* modInfo)
	{
		MessageBoxA(nullptr, "", "", 0);

		//slw_extra_objects::CHudGameMainDisplay::InstallPatches();
		slw_extra_objects::CSetObjectFactory::InstallHooks();
		slw_extra_objects::StageDataInitializer::InstallHooks();
		//slw_extra_objects::Player::CStateActionBase::InstallHooks();
		//slw_extra_objects::Player::CStateCaught::InstallHooks();
		//slw_extra_objects::Player::SonicInfo::InstallHooks();
	}
}