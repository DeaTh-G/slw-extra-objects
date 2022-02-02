#pragma once

namespace TestingGrounds
{
	namespace ObjUtil
	{
		inline static FUNCTION_PTR(int*, __cdecl, GetModelResource, ASLR(0x0073D490), int* dataPtr, const char* name, hh::ut::PackFile& packFile);
	}

	class CSetObjectFactory
	{
	public:
		static void InstallPatches();
	};
}