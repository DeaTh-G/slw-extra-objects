#include "pch.h"
#include "StageDataInitializer.h"

HOOK(void*, __fastcall, BeginLoadLevelDataHook, ASLR(0x00933450), app::StageDataInitializer* in_pThis, void* edx)
{
	originalBeginLoadLevelDataHook(in_pThis, edx);

	app::fnd::FileLoaderParam fileLoaderParam{};
	fileLoaderParam.m_Unk4 = 0x4001;
	csl::fnd::Singleton<app::fnd::FileLoader>::GetInstance()->LoadFile("CommonObjectEx.pac", fileLoaderParam);

	return in_pThis;
}

void slw_extra_objects::StageDataInitializer::InstallHooks()
{
	INSTALL_HOOK(BeginLoadLevelDataHook);
}