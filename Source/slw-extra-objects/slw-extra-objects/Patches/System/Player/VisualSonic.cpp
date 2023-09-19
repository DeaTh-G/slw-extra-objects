#include "pch.h"
#include "VisualSonic.h"

HOOK(void, __cdecl, RegisterResourceHook, ASLR(0x008FE300), app::Player::CPlayer& in_rPlayer, app::Player::GOCReferenceHolder& in_rRefHolderGoc, uint in_unk, bool in_isZelda)
{
	originalRegisterResourceHook(in_rPlayer, in_rRefHolderGoc, in_unk, in_isZelda);

	auto* pInfo = app::ObjUtil::GetObjectInfo<slw_extra_objects::Player::SonicExInfo>(*in_rPlayer.GetDocument());
	if (!pInfo)
		return;

	in_rRefHolderGoc.GetUnit(0).m_rpAnimScript->Extend({ &pInfo->AnimationContainer });
}

void slw_extra_objects::Player::CVisualSonic::InstallHooks()
{
	INSTALL_HOOK(RegisterResourceHook);
}