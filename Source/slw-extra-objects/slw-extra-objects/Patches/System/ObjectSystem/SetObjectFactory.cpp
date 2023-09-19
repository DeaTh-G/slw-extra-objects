#include "pch.h"

HOOK(app::CSetObjectFactory*, __fastcall, ConstructorHook, ASLR(0x008443D0), app::CSetObjectFactory* in_pThis, void* edx)
{
	auto* result = originalConstructorHook(in_pThis, edx);

	result->BeginRegistration();
	//result->RegisterClass(&slw_extra_objects::objectObjectReflections::SUPER_RING_SUPER_CLASS);
	result->RegisterClass(&slw_extra_objects::objectObjectReflections::DASH_RING_CLASS);
	result->RegisterClass(&slw_extra_objects::objectObjectReflections::JUMP_PANEL_CLASS);
	result->RegisterClass(&slw_extra_objects::objectObjectReflections::JUMP_BOARD_EX_CLASS);
	result->RegisterClass(&slw_extra_objects::objectObjectReflections::WIDE_SPRING_CLASS);
	result->RegisterClass(&slw_extra_objects::objectObjectReflections::UP_REEL_CLASS);
	result->EndRegistration();

	return result;
}

HOOK(void, __stdcall, RegisterContainerHook, ASLR(0x008445F0), app::GameDocument& in_rDocument, std::dummy::vector<const char*>& in_rObjects)
{
	in_rDocument.GetService<app::CSetObjectManager>()->m_spObjectFactory->RegisterContainer(in_rDocument, in_rObjects);
}

HOOK(void, __stdcall, CreateParamMapHook, ASLR(0x00843FD0), app::ut::RefPtr<app::SetEd::CClassProject>& in_rClassProject)
{
	app::GameDocument::GetSingleton()->GetService<app::CSetObjectManager>()->m_spObjectFactory->CreateParamMap(in_rClassProject);
}

void slw_extra_objects::CSetObjectFactory::InstallHooks()
{
	INSTALL_HOOK(ConstructorHook);
	INSTALL_HOOK(RegisterContainerHook);
	INSTALL_HOOK(CreateParamMapHook);
}