#include "pch.h"
#include "Patches.h"

static app::CSetObjClass SUPER_RING_SUPER_CLASS{ app::CSetObjClass::Create<app::ObjSuperRingSuper, app::ObjSuperRingSuperInfo>("SuperRingSuper", "OBJECT", "Item", app::paramMap_SuperRingSuper) };
static app::CSetObjClass DASH_RING_CLASS{ app::CSetObjClass::Create<app::ObjDashRing, app::ObjDashRingInfo>("DashRing", "OBJECT", "Gimmick", app::paramMap_DashRing) };
static app::CSetObjClass JUMP_PANEL_CLASS{ app::CSetObjClass::Create<app::ObjJumpPanel, app::ObjJumpPanelInfo>("JumpPanel", "OBJECT", "Gimmick", app::paramMap_JumpPanel) };
static app::CSetObjClass JUMP_BOARD_EX_CLASS{ app::CSetObjClass::Create<app::ObjJumpBoardEx, app::ObjJumpBoardExInfo>("JumpBoardEx", "OBJECT", "Gimmick", app::paramMap_JumpBoardEx) };

HOOK(app::CSetObjectFactory*, __fastcall, h_CSetObjectFactoryCtor, ASLR(0x008443D0), app::CSetObjectFactory* This, void* edx)
{
	originalh_CSetObjectFactoryCtor(This, edx);

	This->BeginRegistration();
	This->RegisterClass(&SUPER_RING_SUPER_CLASS);
	This->RegisterClass(&DASH_RING_CLASS);
	This->RegisterClass(&JUMP_PANEL_CLASS);
	This->RegisterClass(&JUMP_BOARD_EX_CLASS);
	This->EndRegistration();

	return This;
}

HOOK(void, __stdcall, h_CSetObjectFactoryRegisterContainer, ASLR(0x008445F0), app::GameDocument& document, std::dummy::vector<const char*>& objects)
{
	document.GetService<app::CSetObjectManager>()->m_spObjectFactory->RegisterContainer(document, objects);
}

HOOK(void, __stdcall, h_CSetObjectFactoryCreateParamMap, ASLR(0x00843FD0), app::ut::RefPtr<app::SetEd::CClassProject>& classProject)
{
	app::GameDocument::GetSingleton()->GetService<app::CSetObjectManager>()->m_spObjectFactory->CreateParamMap(classProject);
}

void TestingGrounds::CSetObjectFactory::InstallPatches()
{
	INSTALL_HOOK(h_CSetObjectFactoryCtor);
	INSTALL_HOOK(h_CSetObjectFactoryRegisterContainer);
	INSTALL_HOOK(h_CSetObjectFactoryCreateParamMap);
}