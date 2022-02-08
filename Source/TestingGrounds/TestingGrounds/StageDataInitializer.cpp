#include "pch.h"
#include "Patches.h"

HOOK(void*, __fastcall, h_StageDataInitializerBeginLoadLevelData, ASLR(0x00933450), void* This, void* edx)
{
	originalh_StageDataInitializerBeginLoadLevelData(This, edx);

	app::fnd::FileLoaderParam fileLoaderParam{};
	fileLoaderParam.m_Unk4 = 0x4001;
	app::fnd::FileLoader* fileLoader = csl::fnd::Singleton<app::fnd::FileLoader>::GetInstance();
	fileLoader->LoadFile("CommonObjectEx.pac", fileLoaderParam);

	return This;
}

void TestingGrounds::StageDataInitializer::InstallPatches()
{
	INSTALL_HOOK(h_StageDataInitializerBeginLoadLevelData);
}