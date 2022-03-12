#include "pch.h"
#include "LostCodeLoader.h"
#include "Patches\Patches.h"

extern "C"
{
	void _declspec(dllexport) __cdecl Init(ModInfo* modInfo)
	{
		SLW_EXTRA_OBJECTS::CSetObjectFactory::InstallPatches();
		SLW_EXTRA_OBJECTS::StageDataInitializer::InstallPatches();
		SLW_EXTRA_OBJECTS::CStateBase::InstallPatches();
	}
}