#include "pch.h"
#include "Patches/Patches.h"

HOOK(void, __cdecl, h_CVisualSonicRegisterResource, ASLR(0x008FE300), app::Player::CPlayer& in_rPlayer, app::Player::GOCReferenceHolder& in_rRefHolder, unsigned int in_flags, bool in_isZelda)
{
	originalh_CVisualSonicRegisterResource(in_rPlayer, in_rRefHolder, in_flags, in_isZelda);

	auto* pInfo = app::ObjUtil::GetObjectInfo<app::Player::SonicExInfo>(*in_rPlayer.GetDocument(), "SonicExInfo");
	if (!pInfo)
		return;

	in_rRefHolder.GetUnit(0).m_rpAnimScript->Extend({ &pInfo->m_AnimContainer });
}

HOOK(void, __fastcall, h_Initialize, ASLR(0x008F8480), app::Player::SonicInfo* in_pThis, void* in_edx, app::GameDocument& in_rDocument)
{
	originalh_Initialize(in_pThis, in_edx, in_rDocument);

	auto* pInfo = app::ObjUtil::GetObjectInfo<app::Player::SonicExInfo>(in_rDocument, "SonicExInfo");
	if (!pInfo)
		return;
	
	pInfo->m_AnimContainer.MergeTransitions(in_pThis->m_AnimContainer);
}

HOOK(void, __cdecl, h_RegisterResourceInfos, ASLR(0x008F73F0), app::GameDocument* in_pDocument, const app::Player::ResourceInfo& in_rInfo, csl::fnd::IAllocator& in_rAlloc)
{
	auto* pContainer = in_pDocument->GetService<app::CObjInfoContainer>();
	app::Player::CreateInfo<app::Player::SonicExInfo>(*pContainer, in_rAlloc);

	originalh_RegisterResourceInfos(in_pDocument, in_rInfo, in_rAlloc);
}

void SLW_EXTRA_OBJECTS::Player::SonicInfo::InstallPatches()
{
	INSTALL_HOOK(h_RegisterResourceInfos);
	INSTALL_HOOK(h_CVisualSonicRegisterResource);
	INSTALL_HOOK(h_Initialize);
}