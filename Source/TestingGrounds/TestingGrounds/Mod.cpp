#include "pch.h"
#include "LostCodeLoader.h"
#include "Patches.h"

extern "C"
{
	void _declspec(dllexport) __cdecl Init(ModInfo* modInfo)
	{
		MessageBox(NULL, L"AAAAAAAAAA", NULL, MB_ICONERROR);

		TestingGrounds::CSetObjectFactory::InstallPatches();
	}
}
