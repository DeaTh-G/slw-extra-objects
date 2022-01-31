#include "pch.h"
#include "LostCodeLoader.h"

app::CSetObjClass SUPER_RING_SUPER_CLASS;

void InitializeCustomObjects()
{
	auto create_ObjSuperRingSuper = &app::create_ObjSuperRingSuper;
	auto createObjInfo_ObjSuperRingSuperInfo = &app::createObjInfo_ObjSuperRingSuperInfo;

	csl::ut::Enum<app::ESetObjectSpawnType, uint8> spawnType;
	spawnType = 0;
	SUPER_RING_SUPER_CLASS = app::CSetObjClass("SuperRingSuper", "OBJECT", create_ObjSuperRingSuper, createObjInfo_ObjSuperRingSuperInfo, nullptr, nullptr, nullptr, 1000.0f, 1200.0f, spawnType, "Item");
}

extern "C"
{
	void _declspec(dllexport) __cdecl Init(ModInfo* modInfo)
	{
		MessageBox(NULL, L"AAAAAAAAAA", NULL, MB_ICONERROR);

		InitializeCustomObjects();

		app::CSetObjectFactory::InstallPatches();
	}
}
