#include "pch.h"
#include "Patches\Patches.h"

static app::CSetObjClass SUPER_RING_SUPER_CLASS{ app::CSetObjClass::Create<app::ObjSuperRingSuper, app::ObjSuperRingSuperInfo>("SuperRingSuper", "OBJECT", "Item") };
static app::CSetObjClass DASH_RING_CLASS{ app::CSetObjClass::Create<app::ObjDashRing, app::ObjDashRingInfo>("DashRing", "OBJECT", "Gimmick", app::paramMap_DashRing) };
static app::CSetObjClass JUMP_PANEL_CLASS{ app::CSetObjClass::Create<app::ObjJumpPanel, app::ObjJumpPanelInfo>("JumpPanel", "OBJECT", "Gimmick", app::paramMap_JumpPanel) };
static app::CSetObjClass JUMP_BOARD_EX_CLASS{ app::CSetObjClass::Create<app::ObjJumpBoardEx, app::ObjJumpBoardExInfo>("JumpBoardEx", "OBJECT", "Gimmick", app::paramMap_JumpBoardEx) };
static app::CSetObjClass WIDE_SPRING_CLASS{ app::CSetObjClass::Create<app::ObjWideSpring, app::ObjWideSpringInfo>("WideSpring", "OBJECT", "Gimmick", app::paramMap_WideSpring) };

HOOK(app::CSetObjectFactory*, __fastcall, h_CSetObjectFactoryCtor, ASLR(0x008443D0), app::CSetObjectFactory* This, void* edx)
{
	originalh_CSetObjectFactoryCtor(This, edx);

	This->BeginRegistration();
	//This->RegisterClass(&SUPER_RING_SUPER_CLASS);
	This->RegisterClass(&DASH_RING_CLASS);
	This->RegisterClass(&JUMP_PANEL_CLASS);
	This->RegisterClass(&JUMP_BOARD_EX_CLASS);
	This->RegisterClass(&WIDE_SPRING_CLASS);
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

void SLW_EXTRA_OBJECTS::CSetObjectFactory::InstallPatches()
{
	INSTALL_HOOK(h_CSetObjectFactoryCtor);
	INSTALL_HOOK(h_CSetObjectFactoryRegisterContainer);
	INSTALL_HOOK(h_CSetObjectFactoryCreateParamMap);
}