#include "pch.h"
#include "Patches\Patches.h"

/*HOOK(void, __fastcall, h_CVisualGOCChangeAnimation, ASLR(0x008EF650), app::Player::CVisualGOC* in_pVisual, void* in_edx, const char* in_pName)
{
	for (auto unit : in_pVisual->m_rpHumanVisual->m_pGOCHolder->m_Units)
	{
		auto animClip = unit.m_rpAnimScript->m_rpCharAnimation->m_NodeManager.GetAnimationClip(in_pName);
		if (!animClip)
			continue;

		unit.m_rpAnimScript->ChangeAnimation(in_pName);
	}
}

HOOK(void, __cdecl, h_CVisualSonicRegisterResource, ASLR(0x008FE300), app::Player::CPlayer& in_rPlayer, app::Player::GOCReferenceHolder& in_rRefHolder, unsigned int in_flags, bool in_isZelda)
{
	originalh_CVisualSonicRegisterResource(in_rPlayer, in_rRefHolder, in_flags, in_isZelda);
	in_rPlayer.GetComponent<app::Player::GOCCollector>();

	app::Player::GOCReferenceHolderUnit unit{};
	unit.SetOwner(&in_rRefHolder);
	unit.m_rpModel = in_rRefHolder.GetUnit(0).m_rpModel;

	auto* pInfo = app::ObjUtil::GetObjectInfo<app::Player::SonicExInfo>(*in_rPlayer.GetDocument(), "SonicExInfo");
	if (!pInfo)
		return;
	
	auto* pAnimation = app::fnd::GOComponent::CreateSingle<app::game::GOCAnimationScript>(in_rPlayer);
	pAnimation->Setup({ &pInfo->m_AnimationContainer });

	pAnimation->m_pModel = in_rRefHolder.GetUnit(0).m_rpAnimScript->m_pModel;
	unit.AddGOCAnimation(pAnimation);

	in_rRefHolder.m_Units.push_back(unit);
}

HOOK(void, __cdecl, h_RegisterResourceInfos, ASLR(0x008F73F0), app::GameDocument* in_pDocument, const app::Player::ResourceInfo& in_rInfo, csl::fnd::IAllocator& in_rAlloc)
{
	originalh_RegisterResourceInfos(in_pDocument, in_rInfo, in_rAlloc);
	app::Player::CreateInfo<app::Player::SonicExInfo>(*in_pDocument->GetService<app::CObjInfoContainer>(), in_rAlloc);
}

void SLW_EXTRA_OBJECTS::Player::InstallPatches()
{
	INSTALL_HOOK(h_CVisualGOCChangeAnimation);
	INSTALL_HOOK(h_RegisterResourceInfos);
	INSTALL_HOOK(h_CVisualSonicRegisterResource);
}*/