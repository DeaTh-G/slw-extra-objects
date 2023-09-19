#include "pch.h"
#include "PlayerResourceInfoRegister.h"

HOOK(void, __cdecl, RegisterResourceInfosHook, ASLR(0x008F73F0), app::GameDocument* in_pDocument, const app::Player::ResourceInfo& in_rResourceInfo, csl::fnd::IAllocator& in_rAllocator)
{
	auto* pContainer = in_pDocument->GetService<app::CObjInfoContainer>();
	app::Player::CreateInfo<app::Player::SonicExInfo>(*pContainer, in_rAllocator);

	originalRegisterResourceInfosHook(in_pDocument, in_rResourceInfo, in_rAllocator);
}

void slw_extra_objects::Player::InstallHooks()
{
	INSTALL_HOOK(RegisterResourceInfosHook);
}